#pragma once
#include <JuceHeader.h>

class MixerComponent : public juce::Component
{
public:
    MixerComponent();
    void resized() override;
    void refresh();

private:
    juce::OwnedArray<juce::Component> rows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerComponent)
};
