#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

namespace te = tracktion_engine;

class RenameWindow : public DocumentWindow {
public:
    RenameWindow(String name, te::AudioTrack* newTrack, Label* newLabel, int newTrackNo) : DocumentWindow(name, Colours::grey, DocumentWindow::allButtons)
    {
        track = newTrack;
        label = newLabel;
        trackNo = newTrackNo;

        centreWithSize(300, 200);
        setVisible(true);
        setResizable(false, false);
        init();
    }
    void closeButtonPressed() override {
        delete this;

    }
    void resized() override {

        confirm.setBounds(40, 130, 75, 40);
        cancel.setBounds(200, 130, 75, 40);
        title.setBounds(getWidth() / 2 - 100, 40, 200, 20);
        newName.setBounds(getWidth() / 2 - 85, 90, 170, 20);
    }


private:
    void init() {
        confirm.setButtonText("confirm");
        confirm.onClick = [this] { confirmClicked(); };

        cancel.setButtonText("cancel");
        cancel.onClick = [this] { delete this; };

        title.setText("edit name of Track", dontSendNotification);
        title.setJustificationType(Justification::centred);
        title.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        title.setColour(juce::Label::textColourId, juce::Colours::white);

        newName.setText(label->getText());

        addAndMakeVisible(&newName);
        addAndMakeVisible(&title);
        
        addAndMakeVisible(&confirm);
        addAndMakeVisible(&cancel);


    }

    void confirmClicked() {
        if (newName.getText() != "") {
            label->setText(newName.getText(), dontSendNotification);
            track->setName(newName.getText());
            delete this;
        }
        else {
            AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Can't rename track!",
                "new name should not be blank",
                "OK");
        }
    }

    void cancelClicked() {
        delete this;
    }



    TextButton confirm;
    TextButton cancel;

    TextEditor newName;

    Label title;

    te::AudioTrack* track;
    Label* label;

    int trackNo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RenameWindow)
};