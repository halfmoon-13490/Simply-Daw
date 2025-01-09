#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

#define PIXEL 0.25
#define TRACKCONTROL_SIZE 400
#define RGB 255

namespace te = tracktion_engine;

class TrackThumbnail : public juce::Component,
    public ChangeListener
{
public:
    TrackThumbnail()
    {
        formatManager.registerBasicFormats();
        thumbnail.addChangeListener(this);
    }

    ~TrackThumbnail() override
    {
        thumbnail.removeChangeListener(this);
    }

    juce::AudioThumbnail& getAudioThumbnail() { return thumbnail; }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(defaultColor);
        g.setColour(juce::Colours::white);

        if (thumbnail.getTotalLength() > 0.0)
        {
            if (end == 0.0) {
                end = thumbnail.getTotalLength();
            }

            thumbnail.drawChannels(g, getLocalBounds(), start, end, 1.0f);
        }
        else
        {
            if (recording == 1) {
                g.setFont(14.0f);
                g.drawFittedText("(Recording audio...)", getLocalBounds(), juce::Justification::centred, 2);
            }
            else {
                g.setFont(14.0f);
                g.drawFittedText("(No audio loaded...)", getLocalBounds(), juce::Justification::centred, 2);
            }
        }

        juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
        g.setColour(juce::Colours::black);
        g.drawRect(area, 1.0f);


        if (isCropping == 1) {
            juce::Rectangle<int> area1(startPoint, 0, endPoint, getHeight());
            
            g.setColour(Colours::red);
            g.drawRect(area1, 1.0f);
        }
    }

    void setFile(juce::File file) {
        thumbnail.setSource(new juce::FileInputSource(file));
    }

    void removeSource() {
        thumbnail.setSource(nullptr);
    }

    double getLength() {
        return thumbnail.getTotalLength();
    }

    void crop(double newStart, double newEnd) {
        start = newStart;
        end = newEnd;
        repaint();
    }

    void unCrop() {
        start = 0;
        end = thumbnail.getTotalLength();
    }

    double getStart() { return start; }
    double getEnd() { return end; }

    void mouseDown(const MouseEvent& event) override {
        if (isCropping == 1) {
            startPoint = event.getMouseDownX();
            endPoint = startPoint;
        }
        

    }

    void mouseDrag(const MouseEvent& event) override {
        if (isCropping == 1) {
            if (event.getDistanceFromDragStartX() < getWidth() - startPoint) {
                endPoint = event.getDistanceFromDragStartX();                
            }
            repaint();
        }
        
    }

    void mouseUp(const MouseEvent& event) override {
        if (isCropping == 1) {
            if (startPoint > 0 && endPoint > startPoint) {
                
                dragAndCrop();
            }
            setUncrop();
        }
        
    }

    void setRecord(int n) {
        recording = n;
    }

    void setCrop(te::AudioTrack* newTrack) {
        isCropping = 1;
        track = newTrack;

        startPoint = 0;
        endPoint = getWidth();
        repaint();
    }

    void setUncrop() {
        isCropping = 0;
        track = nullptr;

        startPoint = 0;
        endPoint = 0;
        repaint();
    }

    int getCrop() {
        return isCropping;
    }

    void dragAndCrop() {
        int newStartPoint = getX() + startPoint - TRACKCONTROL_SIZE;

        track->splitAt(newStartPoint * PIXEL);

        track->splitAt((newStartPoint + endPoint) * PIXEL);
        
        track->getClips()[0]->removeFromParentTrack();
        track->getClips()[1]->removeFromParentTrack();

        crop(track->getClips()[0]->getPosition().getOffset(), track->getClips()[0]->getPosition().getOffset() + track->getClips()[0]->getPosition().getLength());

        setBounds(getX() + startPoint, getY(), endPoint, getHeight());
        
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();

        if (auto* cs = dynamic_cast<ColourSelector*> (source)) {
            defaultColor = cs->getCurrentColour();
            repaint();
        }
            
    }

 private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };


    double start = 0.0;
    double end = 0.0;

    int recording = 0;

    int isCropping = 0;

    Colour defaultColor = Colours::darkslategrey;

    te::AudioTrack* track = nullptr;

    int startPoint = 0;
    int endPoint = 0;
};

class TimeLine : public Component
{
public:
    void paint(Graphics& g) {
        g.setColour(Colours::black);
        g.fillAll();
        g.setColour(Colours::white);
        g.drawLine(0, 0, getWidth(), 0);
        g.setColour(Colours::lightgrey);
        for (int i = 0; i < 6; i++) {
           // g.drawLine(getWidth() / 5 * i, 0, getWidth() / 5 * i, getHeight());
        }

        for (int i = 0; i < 31; i++) {
            if (i % 6 == 0) {
                g.drawLine(getWidth() / 30 * i, 0, getWidth() / 30 * i, getHeight());
                /*a.setText(String(i), dontSendNotification);
                addAndMakeVisible(&a);
                a.setColour(juce::Label::backgroundColourId, juce::Colours::orange);
                a.setColour(juce::Label::textColourId, juce::Colours::black);
                a.setBounds(31, 2, 20, 15);*/

            }
            else {
                g.drawLine(getWidth() / 30 * i, 0, getWidth() / 30 * i, getHeight() * 2 / 3);
            }
            
        }
    }
private :
    Label a;
    
};