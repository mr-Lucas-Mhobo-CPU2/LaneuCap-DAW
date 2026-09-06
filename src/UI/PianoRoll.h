#pragma once
#include <JuceHeader.h>

class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent()
    {
        addAndMakeVisible(label);
        label.setText("Piano Roll", juce::dontSendNotification);
    }
    void resized() override { label.setBounds(5,5,getWidth()-10,20); }
private:
    juce::Label label;
};
