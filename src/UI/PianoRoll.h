#pragma once
#include <JuceHeader.h>

class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent();
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    struct NoteVisual { double startBeat; double duration; int midiNote; int channel; };
    juce::Array<NoteVisual> notes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollComponent)
};
