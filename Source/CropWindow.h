#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"
#include "TrackThumbnail.h"

namespace te = tracktion_engine;

class CropWindow : public DocumentWindow {
public:
    CropWindow(String name, te::Edit* newEdit, TrackThumbnail* newThumbnail, int newTrackNo) : DocumentWindow(name, Colours::grey, DocumentWindow::allButtons)
    {
        edit = newEdit;
        thumbnail = newThumbnail;
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
        start.setBounds(20, 80, 45, 25);
        startE.setBounds(start.getRight() + 5, start.getY(), 70, 25);
        end.setBounds(startE.getX() + 80, start.getY(), start.getWidth(), start.getHeight());
        endE.setBounds(end.getRight() + 5, end.getY(), startE.getWidth(), startE.getHeight());
        title.setBounds(getWidth() / 2 - 80, 40, 160, 20);
        confirm.setBounds(40, 130, 75, 40);
        cancel.setBounds(200, 130, 75, 40);
    }
    double getStart() {
        return startTime;
    }

    double getEnd() {
        return endTime;
    }
    
private:
    void init() {
        confirm.setButtonText("confirm");
        confirm.onClick = [this] { confirmClicked(); };
        cancel.setButtonText("cancel");
        cancel.onClick = [this] { delete this; };
        
        startE.setInputRestrictions(10, "0123456789.");
        endE.setInputRestrictions(10, "0123456789.");
        start.setColour(juce::Label::backgroundColourId, juce::Colours::grey);
        start.setColour(juce::Label::textColourId, juce::Colours::black);
        start.setText("Start : ", dontSendNotification);
        end.setText("End : ", dontSendNotification);
        end.setColour(juce::Label::backgroundColourId, juce::Colours::grey);
        end.setColour(juce::Label::textColourId, juce::Colours::black);
        title.setText("cut out Track", dontSendNotification);
        title.setJustificationType(Justification::centred);
        title.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        title.setColour(juce::Label::textColourId, juce::Colours::white);

        startTime = te::getAudioTracks(*edit)[trackNo]->getClips()[0]->getPosition().getStart();
        endTime = te::getAudioTracks(*edit)[trackNo]->getClips()[0]->getPosition().getEnd();

        startE.setText(String(startTime));
        endE.setText(String(endTime));

        addAndMakeVisible(&title);
        addAndMakeVisible(&confirm);
        addAndMakeVisible(&cancel);
        addAndMakeVisible(&startE);
        addAndMakeVisible(&endE);
        addAndMakeVisible(&start);
        addAndMakeVisible(&end);
        
    }

    void confirmClicked() {
        if (startE.getText().getDoubleValue() > startTime && endE.getText().getDoubleValue() < endTime) {
            if (startE.getText().getDoubleValue() < endE.getText().getDoubleValue()) {
                auto track = te::getAudioTracks(*edit)[trackNo];

                track->splitAt(startE.getText().getDoubleValue());
                track->splitAt(endE.getText().getDoubleValue());

                track->getClips()[0]->removeFromParentTrack();
                track->getClips()[1]->removeFromParentTrack();

                thumbnail->crop(track->getClips()[0]->getPosition().getOffset(), track->getClips()[0]->getPosition().getOffset() + track->getClips()[0]->getPosition().getLength());

                auto drawPosition = (startE.getText().getDoubleValue() / 300.f) * 1200.f + 400;

                thumbnail->setBounds(drawPosition, 100 + (120 * trackNo), track->getClips()[0]->getPosition().getLength() / 300 * 1200, 120);
                
                delete this;
            }
            else {
                AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Can't crop track!",
                    "End time must be later than start time.",
                    "OK");
            }
        }
        else {
            AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon, "Can't crop track!",
                "The cut range should be narrower than the original audio length",
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

    te::Edit* edit;
    TrackThumbnail* thumbnail;

    int trackNo;

    double startTime = 0;
    double endTime = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CropWindow)
};