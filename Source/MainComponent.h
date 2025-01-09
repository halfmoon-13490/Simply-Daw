#pragma once

#include <JuceHeader.h>
#include "TrackThumbnail.h"
#include "CropWindow.h"
#include "EqWindow.h"
#include "EqTrackWindow.h"
#include "RenderingWindow.h"
#include "LevelMeterComp.h"
#include "RenameWindow.h"
#include "ReverbWindow.h"
#include "DelayWindow.h"
#include "CompressorWindow.h"

#define PIXEL 0.25
#define TRACKCONTROL_SIZE 400

namespace te = tracktion_engine;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class MainComponent : public juce::AudioAppComponent,
    public juce::Timer,
    public Slider::Listener,
    public Button::Listener
    
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    class PlayTrack;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    

private:
    //==============================================================================
    // Your private member variables go here...
    tracktion_engine::Engine  engine{ ProjectInfo::projectName };
    tracktion_engine::Edit edit{ engine, tracktion_engine::createEmptyEdit(engine), tracktion_engine::Edit::EditRole::forEditing, nullptr, 0 };

    juce::DrawableButton playButton;
    juce::DrawableButton stopButton;
    juce::DrawableButton addButton;
    juce::DrawableButton recordButton;
    juce::DrawableButton nextButton;
    juce::DrawableButton previousButton;
    juce::DrawableButton eqButton;
    juce::DrawableButton renderButton;

    TextButton moveButton;
    TextEditor moveE;

    DrawableImage di;


    Slider volumeSlider;
    Slider playSlider;

    juce::TextButton testButton;
    TextButton testButton2;

    Label notice1;
    Label notice2;
    Label notice3;

    Label title;

    te::AudioTrack* trackptr;

    OwnedArray<PlayTrack> playTracks;

    TimeLine timeLine;

    Component::SafePointer<CropWindow> window;
    Component::SafePointer<EqWindow> eqWindow;
    Component::SafePointer<EqTrackWindow> eqTrackWindow;
    Component::SafePointer<RenderingWindow> renderingWindow;
    Component::SafePointer<RenameWindow> renameWindow;
    Component::SafePointer<ReverbWindow> reverbWindow;
    Component::SafePointer<DelayWindow> delayWindow;
    Component::SafePointer<CompressorWindow> compressorWindow;

    LevelMeterComp* levelMeterComp;

    void addTrack(int y);
    void masterInterfaceInit();
    
    void testButtonClicked();
    void testButton2Clicked();
    void renderButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void addButtonClicked();
    void recordButtonClicked();
    void nextButtonClicked();
    void previousButtonClicked();
    void eqButtonClicked();
    void moveButtonClicked();

    void openTrack(int trackNo);
    void muteTrack(int trackNo);
    void soloTrack(int trackNo);
    void recordTrack(int trackNo);
    void removeTrack(int trackNo);
    void forwardTrack(int trackNo);
    void backwardTrack(int trackNo);
    void optionTrack(int trackNo);
    void cropMenu(int trackNo);
    void cropTrack(int trackNo, int idx);
    
    void playSliderChanged();

    void cropWindowOpen(int trackNo);
    void dragCrop(int trackNo);

    void renameWindowOpen(int trackNo);
    void changeTrackColor(int trackNo);

    void selectMoveUnit(int trackNo, float unit);

    void Inputs();

    void eqMaster();

    void renderingAudio();

    void eqWindowOpen(int trackNo);
    void reverbWindowOpen(int trackNo);
    void delayWindowOpen(int trackNo);
    void compressorWindowOpen(int trackNo);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
