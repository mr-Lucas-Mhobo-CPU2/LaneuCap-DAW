#pragma once
#include <JuceHeader.h>

class TransportComponent : public juce::Component
{
public:
    TransportComponent();
    void resized() override;

private:
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::Label bpmLabel;
    juce::Slider bpmSlider;
    juce::TextButton saveButton;
    juce::TextButton loadButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};
