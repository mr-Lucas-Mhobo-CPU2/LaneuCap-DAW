#pragma once
#include <JuceHeader.h>

class BrowserComponent : public juce::Component
{
public:
    BrowserComponent()
    {
        addAndMakeVisible(label);
        label.setText("Browser", juce::dontSendNotification);
    }

    void resized() override
    {
        label.setBounds(5, 5, getWidth() - 10, 20);
    }
private:
    juce::Label label;
};
