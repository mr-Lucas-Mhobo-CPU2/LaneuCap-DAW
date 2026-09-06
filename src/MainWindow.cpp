#include "MainWindow.h"
#include "AudioEngine.h"
#include "UI/Browser.h"
#include "UI/Playlist.h"
#include "UI/ChannelRack.h"
#include "UI/PianoRoll.h"
#include "UI/Mixer.h"
#include "UI/Transport.h"

MainWindow::MainWindow(juce::String name)
    : DocumentWindow(name,
                     juce::Colours::lightgrey,
                     DocumentWindow::allButtons)
{
    // Main content component that lays out panels
    auto content = std::make_unique<juce::Component>();

    // Simple layout: top transport, left browser, center playlist, right mixer
    auto transport = std::make_unique<TransportComponent>();
    auto browser = std::make_unique<BrowserComponent>();
    auto playlist = std::make_unique<PlaylistComponent>();
    auto channelRack = std::make_unique<ChannelRackComponent>();
    auto pianoRoll = std::make_unique<PianoRollComponent>();
    auto mixer = std::make_unique<MixerComponent>();

    // Add to content
    content->addAndMakeVisible(transport.get());
    content->addAndMakeVisible(browser.get());
    content->addAndMakeVisible(playlist.get());
    content->addAndMakeVisible(channelRack.get());
    content->addAndMakeVisible(pianoRoll.get());
    content->addAndMakeVisible(mixer.get());

    // We'll keep ownership by leaking into the content component for now
    // (simpler scaffolding). In a full app use proper ownership and layout.
    content->addChildComponent(transport.release());
    content->addChildComponent(browser.release());
    content->addChildComponent(playlist.release());
    content->addChildComponent(channelRack.release());
    content->addChildComponent(pianoRoll.release());
    content->addChildComponent(mixer.release());

    setContentOwned(content.release(), true);

    centreWithSize(1200, 800);
    setVisible(true);

    // Initialize audio engine
    AudioEngine::getInstance()->initialise();
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
