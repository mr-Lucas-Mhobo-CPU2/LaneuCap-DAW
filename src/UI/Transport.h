#pragma once
#include <JuceHeader.h>
#include "../AudioEngine.h"

class TransportComponent : public juce::Component
{
public:
    TransportComponent()
    {
        addAndMakeVisible(playButton);
        addAndMakeVisible(stopButton);
        playButton.setButtonText("Play");
        stopButton.setButtonText("Stop");

        playButton.onClick = [this]() { AudioEngine::getInstance()->start(); };
        stopButton.onClick = [this]() { AudioEngine::getInstance()->stop(); };
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced(4);
        playButton.setBounds(r.removeFromLeft(80));
        stopButton.setBounds(r.removeFromLeft(80));
    }

private:
    juce::TextButton playButton;
    juce::TextButton stopButton;
};
