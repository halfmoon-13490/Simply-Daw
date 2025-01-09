#pragma once

#include <JuceHeader.h>

class PlayTracking : public Component
{
public:
    void paint(Graphics& g) {
        g.setColour(Colours::black);
        g.fillAll();
        g.setColour(Colours::white);
        for (int i = 0; i < 7; i++) {
            g.drawLine(getWidth() / 6 * i, 0, getWidth() / 6 * i, getHeight());
        }
    }
};