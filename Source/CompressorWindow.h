#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

namespace te = tracktion_engine;

class CompressorWindow : public DocumentWindow,
    public Slider::Listener
{
public:
    CompressorWindow(String name, te::AudioTrack* newTrack, int pluginIdx) : DocumentWindow(name, Colours::grey, DocumentWindow::allButtons)
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
        threshold.setBounds(110, 60, 250, 50);
        ratio.setBounds(110, 90, 250, 50);
        attack.setBounds(110, 120, 250, 50);
        release.setBounds(110, 150, 250, 50);
        output.setBounds(110, 180, 250, 50);

        title.setBounds(getWidth() / 2 - 100, 40, 200, 20);

        thresholdL.setBounds(20, 75, 80, 20);
        ratioL.setBounds(20, 105, 80, 20);
        attackL.setBounds(20, 135, 80, 20);
        releaseL.setBounds(20, 165, 80, 20);
        outputL.setBounds(20, 195, 80, 20);

        remove.setBounds(getWidth() / 2 - 100, getHeight() - 50, 80, 30);
        cancel.setBounds(getWidth() / 2 - 10, getHeight() - 50, 80, 30);
    }

    void sliderValueChanged(Slider* slider) override {
        if (slider == &threshold) {
            track->getAllPlugins()[idx]->getAutomatableParameter(0)->setParameter(slider->getValue() / 100.0f, sendNotification);
        }
        else if (slider == &ratio) {
            track->getAllPlugins()[idx]->getAutomatableParameter(1)->setParameter(slider->getValue() / 100.0f, sendNotification);
        }
        else if (slider == &attack) {
            track->getAllPlugins()[idx]->getAutomatableParameter(2)->setParameter(slider->getValue(), sendNotification);
        }
        else if (slider == &release) {
            track->getAllPlugins()[idx]->getAutomatableParameter(3)->setParameter(slider->getValue(), sendNotification);
        }
        else if (slider == &output) {
            track->getAllPlugins()[idx]->getAutomatableParameter(4)->setParameter(slider->getValue(), sendNotification);
        }
    }

    void valueInit() {
        threshold.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(0)->getCurrentValue()) * 100.0f);
        ratio.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(1)->getCurrentValue()) * 100.0f);
        attack.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(2)->getCurrentValue()));
        release.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(3)->getCurrentValue()));
        output.setValue((track->getAllPlugins()[idx]->getAutomatableParameter(4)->getCurrentValue()));

    }

private:
    void interfaceInit() {

        cancel.setButtonText("exit");
        cancel.onClick = [this] { cancelClicked(); };

        remove.setButtonText("remove");
        remove.onClick = [this] { removeClicked(); };

        threshold.setRange(0.0f, 100.f, 0.1f);
        threshold.setValue(0);
        threshold.addListener(this);
        threshold.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        ratio.setRange(0.0f, 95.0f, 0.1f);
        ratio.setValue(0);
        ratio.addListener(this);
        ratio.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        attack.setRange(0.3f, 200.0f, 0.1f);
        attack.setValue(0);
        attack.addListener(this);
        attack.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        release.setRange(10.0f, 300.0f, 0.1f);
        release.setValue(0);
        release.addListener(this);
        release.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        output.setRange(-10.0f, 24.0f, 0.1f);
        output.setValue(0);
        output.addListener(this);
        output.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        String str = "Compressor fx of Track ";
        str += String(track->getAudioTrackNumber());

        title.setText(str, dontSendNotification);
        thresholdL.setText("Threshold", dontSendNotification);
        ratioL.setText("Ratio", dontSendNotification);
        attackL.setText("Attack (ms)", dontSendNotification);
        releaseL.setText("Release (ms)", dontSendNotification);
        outputL.setText("Output (dB)", dontSendNotification);

        title.setJustificationType(Justification::centred);
        thresholdL.setJustificationType(Justification::centred);
        ratioL.setJustificationType(Justification::centred);
        attackL.setJustificationType(Justification::centred);
        releaseL.setJustificationType(Justification::centred);
        outputL.setJustificationType(Justification::centred);

        title.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        thresholdL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        ratioL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        attackL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        releaseL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        outputL.setColour(juce::Label::backgroundColourId, juce::Colours::black);

        title.setColour(juce::Label::textColourId, juce::Colours::white);
        thresholdL.setColour(juce::Label::textColourId, juce::Colours::white);
        ratioL.setColour(juce::Label::textColourId, juce::Colours::white);
        attackL.setColour(juce::Label::textColourId, juce::Colours::white);
        releaseL.setColour(juce::Label::textColourId, juce::Colours::white);
        outputL.setColour(juce::Label::textColourId, juce::Colours::white);

        addAndMakeVisible(&threshold);
        addAndMakeVisible(&ratio);
        addAndMakeVisible(&attack);
        addAndMakeVisible(&release);
        addAndMakeVisible(&output);

        addAndMakeVisible(&cancel);
        addAndMakeVisible(&remove);

        addAndMakeVisible(&thresholdL);
        addAndMakeVisible(&ratioL);
        addAndMakeVisible(&attackL);
        addAndMakeVisible(&releaseL);
        addAndMakeVisible(&outputL);

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

    Slider threshold;
    Slider ratio;
    Slider attack;
    Slider release;
    Slider output;

    Label title;
    Label thresholdL;
    Label ratioL;
    Label attackL;
    Label releaseL;
    Label outputL;

    te::AudioTrack* track;
    int idx;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorWindow)
};