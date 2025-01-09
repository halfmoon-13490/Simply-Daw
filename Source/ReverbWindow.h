#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

namespace te = tracktion_engine;

class ReverbWindow : public DocumentWindow,
    public Slider::Listener
{
public:
    ReverbWindow(String name, te::AudioTrack* newTrack, int pluginIdx) : DocumentWindow(name, Colours::grey, DocumentWindow::allButtons)
    {
        idx = pluginIdx;
        track = newTrack;

        centreWithSize(400, 280);
        setVisible(true);
        setResizable(false, false);
        interfaceInit();
        valueInit();
    }

    void closeButtonPressed() override {
        delete this;

    }

    void resized() override {
        roomSize.setBounds(110, 60, 250, 50);
        damp.setBounds(110, 90, 250, 50);
        wet.setBounds(110, 120, 250, 50);
        dry.setBounds(110, 150, 250, 50);
        width.setBounds(110, 180, 250, 50);

        title.setBounds(getWidth() / 2 - 100, 40, 200, 20);

        roomSizeL.setBounds(20, 75, 80, 20);
        dampL.setBounds(20, 105, 80, 20);
        wetL.setBounds(20, 135, 80, 20);
        dryL.setBounds(20, 165, 80, 20);
        widthL.setBounds(20, 195, 80, 20);


        remove.setBounds(getWidth() / 2 - 100, getHeight() - 50, 80, 30);
        cancel.setBounds(getWidth() / 2 - 10, getHeight() - 50, 80, 30);
    }

    void sliderValueChanged(Slider* slider) override {
        if (slider == &roomSize) {
            track->getAllPlugins()[idx]->getAutomatableParameter(0)->setParameter(slider->getValue() / 100.0f, sendNotification);
        }
        else if (slider == &damp) {
            track->getAllPlugins()[idx]->getAutomatableParameter(1)->setParameter(slider->getValue() / 100.0f, sendNotification);
        }
        else if (slider == &wet) {
            track->getAllPlugins()[idx]->getAutomatableParameter(2)->setParameter(slider->getValue() / 100.0f, sendNotification);
        }
        else if (slider == &dry) {
            track->getAllPlugins()[idx]->getAutomatableParameter(3)->setParameter(slider->getValue() / 100.0f, sendNotification);
        }
        else if (slider == &width) {
            track->getAllPlugins()[idx]->getAutomatableParameter(4)->setParameter(slider->getValue() / 100.0f, sendNotification);
        }
    }

    void valueInit() {
        roomSize.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(0)->getCurrentValue()) * 100.0f);
        damp.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(1)->getCurrentValue()) * 100.0f);
        wet.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(2)->getCurrentValue()) * 100.0f);
        dry.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(3)->getCurrentValue()) * 100.0f);
        width.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(4)->getCurrentValue()) * 100.0f);

    }

private:
    void interfaceInit() {

        cancel.setButtonText("exit");
        cancel.onClick = [this] { cancelClicked(); };

        remove.setButtonText("remove");
        remove.onClick = [this] { removeClicked(); };

        roomSize.setRange(0.0f, 100.f, 0.1f);
        roomSize.setValue(0);
        roomSize.addListener(this);
        roomSize.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        damp.setRange(0.0f, 100.f, 0.1f);
        damp.setValue(0);
        damp.addListener(this);
        damp.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        wet.setRange(0.0f, 100.f, 0.1f);
        wet.setValue(0);
        wet.addListener(this);
        wet.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        dry.setRange(0.0f, 100.f, 0.1f);
        dry.setValue(0);
        dry.addListener(this);
        dry.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        width.setRange(0.0f, 100.f, 0.1f);
        width.setValue(0);
        width.addListener(this);
        width.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        String str = "Reverb fx of Track ";
        str += String(track->getAudioTrackNumber());

        title.setText(str, dontSendNotification);
        roomSizeL.setText("Room Size", dontSendNotification);
        dampL.setText("Damp (%)", dontSendNotification);
        wetL.setText("Wet", dontSendNotification);
        dryL.setText("Dry", dontSendNotification);
        widthL.setText("Width (%)", dontSendNotification);

        title.setJustificationType(Justification::centred);
        roomSizeL.setJustificationType(Justification::centred);
        dampL.setJustificationType(Justification::centred);
        wetL.setJustificationType(Justification::centred);
        dryL.setJustificationType(Justification::centred);
        widthL.setJustificationType(Justification::centred);

        title.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        roomSizeL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        dampL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        wetL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        dryL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        widthL.setColour(juce::Label::backgroundColourId, juce::Colours::black);

        title.setColour(juce::Label::textColourId, juce::Colours::white);
        roomSizeL.setColour(juce::Label::textColourId, juce::Colours::white);
        dampL.setColour(juce::Label::textColourId, juce::Colours::white);
        wetL.setColour(juce::Label::textColourId, juce::Colours::white);
        dryL.setColour(juce::Label::textColourId, juce::Colours::white);
        widthL.setColour(juce::Label::textColourId, juce::Colours::white);

        addAndMakeVisible(&roomSize);
        addAndMakeVisible(&damp);
        addAndMakeVisible(&wet);
        addAndMakeVisible(&dry);
        addAndMakeVisible(&width);

        addAndMakeVisible(&cancel);
        addAndMakeVisible(&remove);

        addAndMakeVisible(&roomSizeL);
        addAndMakeVisible(&dampL);
        addAndMakeVisible(&wetL);
        addAndMakeVisible(&dryL);
        addAndMakeVisible(&widthL);

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

    Slider roomSize;
    Slider damp;
    Slider wet;
    Slider dry;
    Slider width;

    Label title;
    Label roomSizeL;
    Label dampL;
    Label wetL;
    Label dryL;
    Label widthL;

    te::AudioTrack* track;
    int idx;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbWindow)
};