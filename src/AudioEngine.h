#pragma once

#include <JuceHeader.h>

class AudioEngine : private juce::AudioIODeviceCallback
{
public:
    struct NoteEvent
    {
        int channel = 0;
        int midiNote = 60;
        int velocity = 100;
        double startBeat = 0.0;
        double durationBeats = 1.0;
        bool onSent = false;
        bool offSent = false;
    };

    struct Channel
    {
        juce::Synthesiser synth;
        float volume = 1.0f;
        bool mute = false;
        float pan = 0.0f; // -1.0 left .. 1.0 right
        bool solo = false;
    };

    static AudioEngine* getInstance();

    AudioEngine();
    ~AudioEngine();

    void initialise();
    void shutdown();

    void start();
    void stop();

    void setBpm(double b);
    double getBpm() const;

    int addChannel();
    int getNumChannels() const;
    void setChannelVolume(int index, float vol);
    float getChannelVolume(int index) const;
    void setChannelMute(int index, bool m);
    bool getChannelMute(int index) const;
    void setChannelPan(int index, float pan);
    float getChannelPan(int index) const;
    void setChannelSolo(int index, bool s);
    bool getChannelSolo(int index) const;

    // Sequencer
    void addNoteEvent(const NoteEvent& ev);
    void clearSequence();

    // Project save/load
    bool saveProject(const juce::File& file);
    bool loadProject(const juce::File& file);

private:
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float** outputChannelData, int numOutputChannels,
                               int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    juce::AudioDeviceManager deviceManager;
    juce::AudioSampleBuffer mixBuffer;
    bool isPlaying = false;

    double sampleRate = 44100.0;
    double bpm = 120.0;

    juce::CriticalSection lock;
    juce::Array<Channel> channels;
    juce::Array<NoteEvent> sequence;

    int midiChannelForSynth = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
