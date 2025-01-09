#pragma once

#include "JuceHeader.h"
#include "MainComponent.h"

class LevelMeterComp : public Component, public Timer
{
public:
	LevelMeterComp(te::LevelMeasurer& lm) : levelMeasurer(lm)
	{
		setOpaque(true);
		levelMeasurer.addClient(levelClient);
		startTimerHz(100);
	}

	~LevelMeterComp()
	{
		levelMeasurer.removeClient(levelClient);
		stopTimer();
	}

	void paint(Graphics& g) override
	{
		//g.fillAll(Colour(0xff000000));// fill the background black

		const double meterHeight{ double(getHeight()) };
		const double meterWidth{ double(getWidth()) };
		const double offSet{ fabs(RANGEMINdB) };
		//const double scaleFactor{ meterHeight / (RANGEMAXdB + offSet) };
		/*
		// now we calculate and draw our 0dB line
		g.setColour(Colours::lightgrey);  // set line color						   
		g.fillRect(0.0f, float(meterHeight - (offSet * scaleFactor)), float(meterWidth), 1.0f);

		// draw meter Gain bar
		g.setColour(Colours::green);
		auto displayBarHeight = ((currentLeveldB + offSet) * scaleFactor);
		g.fillRect(0.0f, float(meterHeight - displayBarHeight), float(meterWidth), float(displayBarHeight));
		*/
		auto bounds = getLocalBounds().toFloat();

		g.setColour(Colours::black);
		g.fillRect(bounds);

		const double scaleFactor{ meterWidth / (RANGEMAXdB + offSet) };
		auto displayBarHeight = ((currentLeveldB + offSet) * scaleFactor);

		g.setGradientFill(gradient);
		g.fillRect(bounds.removeFromLeft(displayBarHeight));

	}

	void resized() override {
		auto bounds = getLocalBounds().toFloat();
		gradient = juce::ColourGradient{
			juce::Colours::green,
			bounds.getTopLeft(),
			juce::Colours::red,
			bounds.getTopRight(),
			false
		};
		gradient.addColour(0.5, juce::Colours::yellow);
	}

private:
	void timerCallback()
	{
		prevLeveldB = currentLeveldB;

		currentLeveldB = levelClient.getAndClearAudioLevel(0).dB;

		// Now we give the level bar fading charcteristics.
		// And, the below coversions, decibelsToGain and gainToDecibels,
		// take care of 0dB, which will never fade!...but a gain of 1.0 (0dB) will.

		const auto prevLevel{ Decibels::decibelsToGain(prevLeveldB) };

		if (prevLeveldB > currentLeveldB)
			currentLeveldB = Decibels::gainToDecibels(prevLevel * 0.94);

		// the test below may save some unnecessary paints
		if (currentLeveldB != prevLeveldB)
			repaint();
	}

	// set the range of the meter in dB 
	const double RANGEMAXdB{ 3.0 };//+3dB
	const double RANGEMINdB{ -30.0 };//-30dB

	double currentLeveldB{ 0.0 };
	double prevLeveldB{ 0.0 };

	te::LevelMeasurer& levelMeasurer;
	te::LevelMeasurer::Client levelClient;
	ColourGradient gradient{};
};