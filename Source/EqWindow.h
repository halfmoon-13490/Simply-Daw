#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

namespace te = tracktion_engine;

class EqWindow : public DocumentWindow,
    public Slider::Listener
{
public:
    EqWindow(String name) : DocumentWindow(name, Colours::grey, DocumentWindow::allButtons)
    {
        centreWithSize(400, 250);
        setVisible(true);
        setResizable(false, false);
        init();
    }

    void closeButtonPressed() override {
        delete this;

    }

    void resized() override {
        low.setBounds(110, 60, 250, 50);
        low_mid.setBounds(110, 90, 250, 50);
        high_mid.setBounds(110, 120, 250, 50);
        high.setBounds(110, 150, 250, 50);

        title.setBounds(getWidth() / 2 - 100, 40, 200, 20);

        lowL.setBounds(20, 75, 80, 20);
        low_midL.setBounds(20, 105, 80, 20);
        high_midL.setBounds(20, 135, 80, 20);
        highL.setBounds(20, 165, 80, 20);
              
        cancel.setBounds(getWidth() / 2 - 40, getHeight() - 50, 80, 30);
    }
   

    void setEdit(te::Edit* newEdit) {
        edit = newEdit;
    }

    void setPreEq(float newLow, float newLowMid, float newHighMid, float newHigh) {
        low.setValue(newLow);
        low_mid.setValue(newLowMid);
        high_mid.setValue(newHighMid);
        high.setValue(newHigh);
    }

    void sliderValueChanged(Slider* slider) override {
        
        if (slider == &low) {
            edit->getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(1)->setParameter(slider->getValue(), sendNotification);
        }
        else if (slider == &low_mid) {
            edit->getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(10)->setParameter(slider->getValue(), sendNotification);
        }
        else if (slider == &high_mid) {
            edit->getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(7)->setParameter(slider->getValue(), sendNotification);
        }
        else if (slider == &high) {
            edit->getMasterTrack()->getAllPlugins()[0]->getAutomatableParameter(4)->setParameter(slider->getValue(), sendNotification);
        }
        
        
    }
    
private:
    void init() {
        
        cancel.setButtonText("exit");
        cancel.onClick = [this] { delete this; };

        low.setRange(-20.f, 20.f, 0.1f);
        low.setValue(0);
        low.addListener(this);
        low.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        low_mid.setRange(-20.f, 20.f, 0.1f);
        low_mid.setValue(0);
        low_mid.addListener(this);
        low_mid.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        high_mid.setRange(-20.f, 20.f, 0.1f);
        high_mid.setValue(0);
        high_mid.addListener(this);
        high_mid.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        high.setRange(-20.f, 20.f, 0.1f);
        high.setValue(0);
        high.addListener(this);
        high.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);

        title.setText("4-band EQ of all Track", dontSendNotification);
        lowL.setText("low ", dontSendNotification);
        low_midL.setText("low mid", dontSendNotification);
        high_midL.setText("high mid", dontSendNotification);
        highL.setText("high", dontSendNotification);

        title.setJustificationType(Justification::centred);
        lowL.setJustificationType(Justification::centred);
        low_midL.setJustificationType(Justification::centred);
        high_midL.setJustificationType(Justification::centred);
        highL.setJustificationType(Justification::centred);

        title.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        lowL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        low_midL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        high_midL.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        highL.setColour(juce::Label::backgroundColourId, juce::Colours::black);

        title.setColour(juce::Label::textColourId, juce::Colours::white);
        lowL.setColour(juce::Label::textColourId, juce::Colours::white);
        low_midL.setColour(juce::Label::textColourId, juce::Colours::white);
        high_midL.setColour(juce::Label::textColourId, juce::Colours::white);
        highL.setColour(juce::Label::textColourId, juce::Colours::white);
              
        addAndMakeVisible(&low);
        addAndMakeVisible(&low_mid);
        addAndMakeVisible(&high_mid);
        addAndMakeVisible(&high);
        addAndMakeVisible(&cancel);
        addAndMakeVisible(&lowL);
        addAndMakeVisible(&low_midL);
        addAndMakeVisible(&high_midL);
        addAndMakeVisible(&highL);
        addAndMakeVisible(&title);
    }


    void cancelClicked() {
        delete this;
    }
    
    TextButton cancel;
    
    Slider low;
    Slider low_mid;
    Slider high_mid;
    Slider high;

    Label title;
    Label lowL;
    Label low_midL;
    Label high_midL;
    Label highL;

    te::Edit* edit;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqWindow)
};