#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

class RenderingWindow : public DocumentWindow {
public:
    RenderingWindow(String name, te::Edit* newEdit) : DocumentWindow(name, Colours::darkgrey, DocumentWindow::allButtons)
    {
        edit = newEdit;
        centreWithSize(400, 250);
        setVisible(true);
        setResizable(false, false);
        init();
    }
    void closeButtonPressed() override {
        delete this;

    }
    void resized() override {
        title.setBounds(getWidth() / 2 - 75, 40, 150, 20);

        start.setBounds(35, 95, 60, 20);
        startE.setBounds(start.getRight() + 5, start.getY(), 100, 20);

        end.setBounds(start.getX(), start.getY() + 40, start.getWidth(), start.getHeight());
        endE.setBounds(end.getRight() + 5, end.getY(), startE.getWidth(), startE.getHeight());

        confirm.setBounds(getWidth() / 4, getHeight() - 50, 80, 30);
        cancel.setBounds(getWidth() * 3 / 4 - 80, getHeight() - 50, 80, 30);

        choice.setBounds(250, 80, 100, 20);

        wav.setBounds(250, 110, 60, 20);
        mp3.setBounds(wav.getX(), wav.getY() + 30, wav.getWidth(), wav.getHeight());

    }
    

private:
    void init() {


        confirm.setButtonText("confirm");
        confirm.onClick = [this] { confirmClicked(); };

        cancel.setButtonText("cancel");
        cancel.onClick = [this] { delete this; };

        startE.setText("0");
        endE.setText(String(edit->getLength()));

        startE.setInputRestrictions(10, "0123456789.");
        endE.setInputRestrictions(10, "0123456789.");

        title.setJustificationType(Justification::centred);
        title.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        title.setColour(juce::Label::textColourId, juce::Colours::white);
        title.setText("Rendering audio", dontSendNotification);

        start.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        start.setColour(juce::Label::textColourId, juce::Colours::white);
        start.setText("Start : ", dontSendNotification);

        end.setText("End : ", dontSendNotification);
        end.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        end.setColour(juce::Label::textColourId, juce::Colours::white);

        choice.setText("Select format..", dontSendNotification);
        choice.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
        choice.setColour(juce::Label::textColourId, juce::Colours::white);
        
        wav.setRadioGroupId(1000);
        mp3.setRadioGroupId(1000);

        wav.setButtonText("  wav");
        mp3.setButtonText("  mp3");

        wav.setColour(ToggleButton::tickColourId, juce::Colours::black);
        mp3.setColour(ToggleButton::tickColourId, juce::Colours::black);

        wav.setToggleState(true, dontSendNotification);

        wav.onClick = [this] { format = "*.wav"; };
        mp3.onClick = [this] { format = "*.mp3"; };

        addAndMakeVisible(&wav);
        addAndMakeVisible(&mp3);

        addAndMakeVisible(&title);
        addAndMakeVisible(&start);
        addAndMakeVisible(&end);
        addAndMakeVisible(&choice);

        addAndMakeVisible(&confirm);
        addAndMakeVisible(&cancel);

        addAndMakeVisible(&startE);
        addAndMakeVisible(&endE);

    }

    void confirmClicked() {
        
        if (edit->getLength() > 0) {
            if (startE.getText().getDoubleValue() < endE.getText().getDoubleValue()) {
                FileChooser chooser("select a save location...", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), format);

                if (chooser.browseForFileToSave(true)) {
                    File renderFile{ chooser.getResult() };
                    te::EditTimeRange range{ startE.getText().getDoubleValue(), endE.getText().getDoubleValue() };

                    BigInteger tracksToDo{ 0 };

                    for (auto i = 0; i < te::getAllTracks(*edit).size(); i++)
                        tracksToDo.setBit(i);

                    te::Renderer::renderToFile("Render", renderFile, *edit, range, tracksToDo, true, {}, false);
                }
                delete this;
            }
            else {
                AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Can't render audio!",
                    "End time must be later than start time.",
                    "OK");
            }
        }
        else {
            AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Can't render audio!",
                "There is no audio loaded in project",
                "OK");
        }        
    }

    void cancelClicked() {
        delete this;
    }

    TextButton confirm;
    TextButton cancel;

    TextEditor startE;
    TextEditor endE;

    Label start;
    Label end;
    Label title;
    Label choice;

    ToggleButton wav;
    ToggleButton mp3;

    String format = "*.wav";

    te::Edit* edit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RenderingWindow)
};