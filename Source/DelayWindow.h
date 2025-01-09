#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

namespace te = tracktion_engine;

class DelayWindow : public DocumentWindow,
    public Slider::Listener
{
public:
    DelayWindow(String name, te::AudioTrack* newTrack, int pluginIdx) : DocumentWindow(name, Colours::grey, DocumentWindow::allButtons)
    {
        idx = pluginIdx;
        track = newTrack;

        centreWithSize(400, 190);
        setVisible(true);
        setResizable(false, false);
        interfaceInit();
        valueInit();
    }

    void closeButtonPressed() override {
        delete this;

    }

    void resized() override {
        feedBack.setBounds(110, 60, 250, 50);
        mix.setBounds(110, 90, 250, 50);
       
        title.setBounds(getWidth() / 2 - 100, 40, 200, 20);

        feedBackL.setBounds(20, 75, 80, 20);
        mixL.setBounds(20, 105, 80, 20);
        
        remove.setBounds(getWidth() / 2 - 100, getHeight() - 50, 80, 30);
        cancel.setBounds(getWidth() / 2 - 10, getHeight() - 50, 80, 30);
    }

    void sliderValueChanged(Slider* slider) override {
        if (slider == &feedBack) {
            track->getAllPlugins()[idx]->getAutomatableParameter(0)->setParameter(slider->getValue(), sendNotification);
        }
        else if (slider == &mix) {
            track->getAllPlugins()[idx]->getAutomatableParameter(1)->setParameter(slider->getValue() / 100, sendNotification);
        }
    }

    void valueInit() {
        feedBack.setValue(track->getAllPlugins()[idx]->getAutomatableParameter(0)->getCurrentValue());
        mix.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(1)->getCurrentValue()) * 100.0f);
    }

private:
    void interfaceInit() {

        cancel.setButtonText("exit");
        cancel.onClick = [this] { cancelClicked(); };

        remove.setButtonText("remove");
        remove.onClick = [this] { removeClicked(); };

        feedBack.setRange(-30.0f, 0.0f, 0.1f);
        feedBack.setValue(0);
        feedBack.addListener(this);
        feedBack.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        mix.setRange(0.0f, 100.0f, 0.1f);
        mix.setValue(0);
        mix.addListener(this);
        mix.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        String str = "Delay fx of Track ";
        str += String(track->getAudioTrackNumber());

        title.setText(str, dontSendNotification);
        feedBackL.setText("Feedback (dB)", dontSendNotification);
        mixL.setText("Mix (% wet)", dontSendNotification);

        title.setJustificationType(Justification::centred);
        feedBackL.setJustificationType(Justification::centred);
        mixL.setJustificationType(Justification::centred);
        
        title.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        feedBackL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        mixL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        
        title.setColour(juce::Label::textColourId, juce::Colours::white);
        feedBackL.setColour(juce::Label::textColourId, juce::Colours::white);
        mixL.setColour(juce::Label::textColourId, juce::Colours::white);
        
        addAndMakeVisible(&feedBack);
        addAndMakeVisible(&mix);
        
        addAndMakeVisible(&cancel);
        addAndMakeVisible(&remove);

        addAndMakeVisible(&feedBackL);
        addAndMakeVisible(&mixL);
        
        addAndMakeVisible(&title);

    }

    void cancelClicked() {
        delete this;
    }

    void removeClicked() {

        track->getAllPlugins()[idx]->deleteFromParent();
        cancelClicked();
    }

    TextButton remove;
    TextButton cancel;

    Slider feedBack;
    Slider mix;
    

    Label title;
    Label feedBackL;
    Label mixL;

    te::AudioTrack* track;
    int idx;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayWindow)
};