#pragma once
#include <JuceHeader.h>

class MixerComponent : public juce::Component
{
public:
    MixerComponent()
    {
        addAndMakeVisible(label);
        label.setText("Mixer", juce::dontSendNotification);
    }
    void resized() override { label.setBounds(5,5,getWidth()-10,20); }
private:
    juce::Label label;
};
