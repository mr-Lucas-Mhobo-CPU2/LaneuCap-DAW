#pragma once
#include <JuceHeader.h>

class ChannelRackComponent : public juce::Component
{
public:
    ChannelRackComponent()
    {
        addAndMakeVisible(label);
        label.setText("Channel Rack", juce::dontSendNotification);
    }
    void resized() override { label.setBounds(5,5,getWidth()-10,20); }
private:
    juce::Label label;
};
