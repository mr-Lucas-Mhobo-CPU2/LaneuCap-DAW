#include "AudioEngine.h"

static AudioEngine* instance = nullptr;

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
    juce::String err;
    juce::AudioDeviceManager::AudioDeviceSetup setup;

    deviceManager.initialise(0, 2, nullptr, true);
    deviceManager.addAudioCallback(this);
}

void AudioEngine::shutdown()
{
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
}

void AudioEngine::start()
{
    isPlaying = true;
}

void AudioEngine::stop()
{
    isPlaying = false;
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    tempBuffer.setSize(device->getActiveOutputChannels().countNumberOfSetBits(), device->getCurrentBufferSizeSamples());
}

void AudioEngine::audioDeviceStopped()
{
    tempBuffer.setSize(0, 0);
}

void AudioEngine::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                                        float** outputChannelData, int numOutputChannels,
                                        int numSamples)
{
    juce::ignoreUnused(inputChannelData, numInputChannels);

    // Simple audio: if playing, generate a sine tone on left/right
    if (isPlaying)
    {
        static double phase = 0.0;
        double phaseDelta = 2.0 * juce::MathConstants<double>::pi * 440.0 / 44100.0;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float value = (float)std::sin(phase) * 0.1f;
            phase += phaseDelta;
            if (phase > juce::MathConstants<double>::twoPi) phase -= juce::MathConstants<double>::twoPi;

            for (int ch = 0; ch < numOutputChannels; ++ch)
            {
                outputChannelData[ch][sample] = value;
            }
        }
    }
    else
    {
        for (int ch = 0; ch < numOutputChannels; ++ch)
            juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);
    }
}
