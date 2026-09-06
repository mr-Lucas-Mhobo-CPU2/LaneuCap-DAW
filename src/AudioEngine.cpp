#include "AudioEngine.h"

static AudioEngine* instance = nullptr;

// Simple Synth sound + voice -------------------------------------------------

struct SimpleSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

struct SimpleVoice : public juce::SynthesiserVoice
{
    SimpleVoice() {}

    bool canPlaySound (juce::SynthesiserSound* s) override { return dynamic_cast<SimpleSound*>(s) != nullptr; }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/) override
    {
        level = velocity * 0.15f;
        tailOff = 0.0f;
        currentAngle = 0.0;
        angleDelta = juce::MathConstants<double>::twoPi * juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) / getSampleRate();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailOff == 0.0) // we only start a tail-off if it's not already doing so.
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (std::sin (currentAngle) * level * tailOff);
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99; // simple exponential tail
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (std::sin (currentAngle) * level);
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};

// AudioEngine implementation --------------------------------------------------

AudioEngine* AudioEngine::getInstance()
{
    if (!instance) instance = new AudioEngine();
    return instance;
}

AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

void AudioEngine::initialise()
{
    deviceManager.initialise(0, 2, nullptr, true);
    deviceManager.addAudioCallback(this);

    // Create default channels
    for (int i = 0; i < 4; ++i)
        addChannel();
}

void AudioEngine::shutdown()
{
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();

    // cleanup
    channels.clear();
    sequence.clear();
}

void AudioEngine::start()
{
    juce::ScopedLock sl(lock);
    isPlaying = true;
    // reset sequence flags
    for (auto& ev : sequence)
    {
        ev.onSent = false;
        ev.offSent = false;
    }
}

void AudioEngine::stop()
{
    juce::ScopedLock sl(lock);
    isPlaying = false;
    // send all noteOffs
    for (auto& ch : channels)
    {
        ch.synth.allNotesOff(0, true);
    }
}

void AudioEngine::setBpm(double b)
{
    juce::ScopedLock sl(lock);
    bpm = b;
}

double AudioEngine::getBpm() const { return bpm; }

int AudioEngine::addChannel()
{
    juce::ScopedLock sl(lock);
    Channel ch;
    ch.volume = 1.0f;
    ch.mute = false;
    // add 6 voices per channel
    for (int v = 0; v < 6; ++v)
        ch.synth.addVoice(new SimpleVoice());
    ch.synth.addSound(new SimpleSound());
    channels.add(std::move(ch));
    return channels.size() - 1;
}

int AudioEngine::getNumChannels() const
{
    return channels.size();
}

void AudioEngine::setChannelVolume(int index, float vol)
{
    juce::ScopedLock sl(lock);
    if (isPositiveAndBelow(index, channels.size()))
        channels.getReference(index).volume = vol;
}

float AudioEngine::getChannelVolume(int index) const
{
    if (isPositiveAndBelow(index, channels.size()))
        return channels.getReference(index).volume;
    return 1.0f;
}

void AudioEngine::setChannelMute(int index, bool m)
{
    juce::ScopedLock sl(lock);
    if (isPositiveAndBelow(index, channels.size()))
        channels.getReference(index).mute = m;
}

bool AudioEngine::getChannelMute(int index) const
{
    if (isPositiveAndBelow(index, channels.size()))
        return channels.getReference(index).mute;
    return false;
}

void AudioEngine::addNoteEvent(const NoteEvent& ev)
{
    juce::ScopedLock sl(lock);
    sequence.add(ev);
}

void AudioEngine::clearSequence()
{
    juce::ScopedLock sl(lock);
    sequence.clear();
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    mixBuffer.setSize(device->getActiveOutputChannels().countNumberOfSetBits(), device->getCurrentBufferSizeSamples());

    // initialize synth sample rates
    for (auto& ch : channels)
        ch.synth.setCurrentPlaybackSampleRate(sampleRate);
}

void AudioEngine::audioDeviceStopped()
{
    mixBuffer.setSize(0, 0);
}

void AudioEngine::audioDeviceIOCallback(const float** /*inputChannelData*/, int /*numInputChannels*/,
                                        float** outputChannelData, int numOutputChannels,
                                        int numSamples)
{
    // clear output
    for (int ch = 0; ch < numOutputChannels; ++ch)
        juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);

    if (channels.size() == 0)
        return;

    // compute beat ranges
    const double samplesPerBeat = (sampleRate * 60.0) / bpm;

    // Lock sequence/channels while we process events
    juce::ScopedLock sl(lock);

    if (isPlaying)
    {
        // current sample index is not tracked globally here; instead compute beats based on a simple running frame counter
        static int64_t totalSamples = 0;
        const double beatStart = totalSamples / samplesPerBeat;
        const double beatEnd = (totalSamples + numSamples) / samplesPerBeat;

        for (auto& ev : sequence)
        {
            if (!ev.onSent && ev.startBeat >= beatStart && ev.startBeat < beatEnd)
            {
                if (isPositiveAndBelow(ev.channel, channels.size()))
                    channels.getReference(ev.channel).synth.noteOn(midiChannelForSynth, ev.midiNote, (juce::uint8)ev.velocity);
                ev.onSent = true;
            }
            if (!ev.offSent && (ev.startBeat + ev.durationBeats) >= beatStart && (ev.startBeat + ev.durationBeats) < beatEnd)
            {
                if (isPositiveAndBelow(ev.channel, channels.size()))
                    channels.getReference(ev.channel).synth.noteOff(midiChannelForSynth, ev.midiNote, (juce::uint8)0, true);
                ev.offSent = true;
            }
        }

        totalSamples += numSamples;
    }

    // render each channel into mixBuffer then copy to output with volume
    mixBuffer.setSize(numOutputChannels, numSamples);
    mixBuffer.clear();

    for (int i = 0; i < channels.size(); ++i)
    {
        auto& ch = channels.getReference(i);
        juce::AudioBuffer<float> tempBuf(numOutputChannels, numSamples);
        tempBuf.clear();

        ch.synth.renderNextBlock(tempBuf, juce::MidiBuffer(), 0, numSamples);

        float vol = ch.mute ? 0.0f : ch.volume;
        for (int c = 0; c < numOutputChannels; ++c)
        {
            auto* src = tempBuf.getReadPointer(c);
            auto* dst = mixBuffer.getWritePointer(c);
            for (int s = 0; s < numSamples; ++s)
                dst[s] += src[s] * vol;
        }
    }

    // copy mixBuffer to outputChannelData
    for (int c = 0; c < numOutputChannels; ++c)
    {
        auto* src = mixBuffer.getReadPointer(c);
        auto* dst = outputChannelData[c];
        for (int s = 0; s < numSamples; ++s)
            dst[s] = src[s];
    }
}

bool AudioEngine::saveProject(const juce::File& file)
{
    juce::ScopedLock sl(lock);
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty("bpm", bpm);

    juce::Array<juce::var> chArr;
    for (auto& ch : channels)
    {
        juce::DynamicObject::Ptr dob = new juce::DynamicObject();
        dob->setProperty("volume", ch.volume);
        dob->setProperty("mute", ch.mute);
        chArr.add(juce::var(dob.get()));
    }
    root->setProperty("channels", juce::var(chArr));

    juce::Array<juce::var> notesArr;
    for (auto& ev : sequence)
    {
        juce::DynamicObject::Ptr n = new juce::DynamicObject();
        n->setProperty("channel", ev.channel);
        n->setProperty("midiNote", ev.midiNote);
        n->setProperty("velocity", ev.velocity);
        n->setProperty("startBeat", ev.startBeat);
        n->setProperty("durationBeats", ev.durationBeats);
        notesArr.add(juce::var(n.get()));
    }
    root->setProperty("notes", juce::var(notesArr));

    juce::var top(root.get());
    juce::String json = juce::JSON::toString(top);
    return file.replaceWithText(json);
}

bool AudioEngine::loadProject(const juce::File& file)
{
    if (!file.existsAsFile()) return false;
    juce::String json = file.loadFileAsString();
    juce::var result = juce::JSON::parse(json);
    if (auto* root = result.getDynamicObject())
    {
        juce::ScopedLock sl(lock);
        if (root->hasProperty("bpm")) bpm = (double)root->getProperty("bpm");

        channels.clearQuick();
        if (root->hasProperty("channels"))
        {
            auto chArr = root->getProperty("channels");
            if (auto* arr = chArr.getArray())
            {
                for (auto& v : *arr)
                {
                    if (auto* dob = v.getDynamicObject())
                    {
                        Channel ch;
                        ch.volume = (float)dob->getProperty("volume");
                        ch.mute = (bool)dob->getProperty("mute");
                        for (int v = 0; v < 6; ++v) ch.synth.addVoice(new SimpleVoice());
                        ch.synth.addSound(new SimpleSound());
                        channels.add(std::move(ch));
                    }
                }
            }
        }

        sequence.clear();
        if (root->hasProperty("notes"))
        {
            auto notes = root->getProperty("notes");
            if (auto* arr = notes.getArray())
            {
                for (auto& n : *arr)
                {
                    if (auto* dob = n.getDynamicObject())
                    {
                        NoteEvent ev;
                        ev.channel = (int)dob->getProperty("channel");
                        ev.midiNote = (int)dob->getProperty("midiNote");
                        ev.velocity = (int)dob->getProperty("velocity");
                        ev.startBeat = (double)dob->getProperty("startBeat");
                        ev.durationBeats = (double)dob->getProperty("durationBeats");
                        ev.onSent = ev.offSent = false;
                        sequence.add(ev);
                    }
                }
            }
        }

        return true;
    }
    return false;
}
