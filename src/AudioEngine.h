#pragma once

#include <JuceHeader.h>

class AudioEngine : private juce::AudioIODeviceCallback
{
public:
    static AudioEngine* getInstance();

    AudioEngine();
    ~AudioEngine();

    void initialise();
    void shutdown();

    void start();
    void stop();

private:
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float** outputChannelData, int numOutputChannels,
                               int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    juce::AudioDeviceManager deviceManager;
    juce::AudioSampleBuffer tempBuffer;
    bool isPlaying = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
