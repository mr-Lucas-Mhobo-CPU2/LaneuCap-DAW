#include "PianoRoll.h"
#include "../AudioEngine.h"

PianoRollComponent::PianoRollComponent()
{
}

void PianoRollComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkslategrey);
    g.setColour(juce::Colours::white);

    // draw grid for 8 beats and 12 notes
    int beats = 16;
    int keys = 12;
    auto r = getLocalBounds().toFloat();
    for (int b = 0; b <= beats; ++b)
    {
        float x = r.getX() + (r.getWidth() * b / beats);
        g.drawLine(x, r.getY(), x, r.getBottom(), 1.0f);
    }
    for (int k = 0; k <= keys; ++k)
    {
        float y = r.getY() + (r.getHeight() * k / keys);
        g.drawLine(r.getX(), y, r.getRight(), y, 1.0f);
    }

    // draw notes
    g.setColour(juce::Colours::orange);
    for (auto& n : notes)
    {
        float x = (float)(getWidth() * (n.startBeat / 16.0));
        float w = (float)(getWidth() * (n.duration / 16.0));
        float y = (float)(getHeight() * ((127 - n.midiNote) / 12.0));
        g.fillRect(x, y, w, 12);
    }
}

void PianoRollComponent::resized()
{
}

void PianoRollComponent::mouseDown(const juce::MouseEvent& e)
{
    // convert mouse x to beat, y to midi note
    double beat = (double)e.x / (double)getWidth() * 16.0;
    int midi = 72 - (int)((double)e.y / (double)getHeight() * 12.0);
    if (midi < 0) midi = 0; if (midi > 127) midi = 127;

    NoteVisual nv;
    nv.startBeat = std::floor(beat);
    nv.duration = 1.0;
    nv.midiNote = midi;
    nv.channel = 0; // default channel 0
    notes.add(nv);

    // add to AudioEngine sequence
    AudioEngine::NoteEvent ev;
    ev.channel = nv.channel;
    ev.midiNote = nv.midiNote;
    ev.velocity = 100;
    ev.startBeat = nv.startBeat;
    ev.durationBeats = nv.duration;
    ev.onSent = ev.offSent = false;
    AudioEngine::getInstance()->addNoteEvent(ev);

    repaint();
}
