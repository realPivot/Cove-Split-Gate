#pragma once

#include <JuceHeader.h>

namespace Gui
{
	class Meter : public juce::Component, public juce::Timer
	{
	public:
		// Styles of meters
		enum MeterStyle
		{
			Vertical, /**< A meter drawn vertically */
			Horizontal /**< A horizontal meter */
		};

		enum FillDirection
		{
			Left,
			Right
		};

		Meter() {
			startTimerHz(refreshRateInHz);
			isThresholdEnabled = false;
		} // Blank Constructor

		Meter(MeterStyle style, FillDirection direction) { // Constructor where style is defined
			meterStyle = style;
			fillDirection = direction;
			isThresholdEnabled = false;
			startTimerHz(refreshRateInHz);
		}

		~Meter() { // Destructor
		}

		void paint(juce::Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat(); // get bounds of meter
			auto cornerSize = 5.0f; // size of corner

			// Background
			g.setColour(juce::Colour(209, 222, 222).withBrightness(0.2f));
			g.fillRoundedRectangle(bounds, 5.0f);

			// Foreground
			g.setColour(juce::Colour(209,222,222)); // Color of filled bar

			const auto scaledX = juce::jmap(level, -80.0f, 0.0f, 0.0f, static_cast<float>(getWidth())); // position for filled levelMeter in x orientation
			const auto scaledY = juce::jmap(level, -80.0f, 0.0f, 0.0f, static_cast<float>(getHeight())); // position for filled levelMeter in y orientation
			const auto scaledXPeak = juce::jmap(maxLevel, -80.0f, 0.0f, 0.0f, static_cast<float>(getWidth())); // position for peak in x orientation
			const auto scaledYPeak = juce::jmap(maxLevel, -80.0f, 0.0f, 0.0f, static_cast<float>(getHeight())); // position for filled levelMeter in y orientation
			const auto jmapThresholdX = juce::jmap(thresholdValue, 0.0f, static_cast<float>(getWidth())); // position for threshold value in x orientation
			const auto jmapThresholdY = juce::jmap(thresholdValue, -80.f, 0.0f, 0.0f, static_cast<float>(getHeight())); // position for threshold value in y orientation
			thresholdX = jmapThresholdX;



			switch (meterStyle)
			{
			case MeterStyle::Horizontal:
				
				if (fillDirection == FillDirection::Left)
				{
					auto bounds2 = bounds; // copy area of bounds into second object to get peak
					auto thresholdBounds = bounds;
					auto filledBounds = bounds.removeFromLeft(scaledX); // remove filled meter area from bounds object
					auto peakBounds = bounds2.removeFromLeft(scaledXPeak); // remove peak area from copied bounds object

					// Background
					thresholdBounds = thresholdBounds.removeFromLeft(jmapThresholdX);
					if (isThresholdEnabled) {
						g.setColour(juce::Colours::red.withAlpha(.2f));
						g.fillRoundedRectangle(thresholdBounds, cornerSize);
					}

					g.setColour(juce::Colour(209, 222, 222)); // Color of filled bar
					g.fillRoundedRectangle(filledBounds, cornerSize);
					g.setColour(juce::Colour(29, 32, 31)); // Color of peak bar
					g.fillRect(peakBounds.getRight(), peakBounds.getY(), 2.f, bounds.getHeight());
					
					
				}
				else if (fillDirection == FillDirection::Right) {
					auto bounds2 = bounds; // copy area of bounds into second object to get peak
					auto filledBounds = bounds.removeFromRight(scaledX); // remove filled meter area from bounds object
					auto peakBounds = bounds2.removeFromRight(scaledXPeak);// remove peak area from copied bounds object

					g.fillRoundedRectangle(filledBounds, cornerSize);
					g.setColour(juce::Colour(29, 32, 31)); // Color of peak bar
					g.fillRect(peakBounds.getX(), peakBounds.getY(), 2.f, bounds.getHeight());

				}
				break;

			case MeterStyle::Vertical:
				auto bounds2 = bounds; // copy area of bounds into second object to get peak
				auto filledBounds = bounds.removeFromBottom(scaledY); // remove filled meter area from bounds object
				auto peakBounds = bounds2.removeFromBottom(scaledYPeak); // remove peak area from copied bounds object

				g.fillRoundedRectangle(filledBounds, cornerSize);
				g.setColour(juce::Colour(29, 32, 31)); // Color of peak bar
				g.fillRect(peakBounds.getX(), peakBounds.getY(), bounds.getWidth(), 2.f);
				break;
			}
			
		}

		void setLevel(const float value) { 
			// smoothing
			if (value >= level) {
				level = value;
			} else {
				level -= 0.25f;
			}
			level = std::clamp(level, -100.f, 0.f);
			
			// peak bar
			if (maxLevel <= value) {
				maxLevel = value;
				timerIterator = 0; // reset timerIterator
			}
		}

		void resetPeak() {
			maxLevel = level;
		}

		void timerCallback() override
		{
			timerIterator++;

			if (timerIterator >= refreshRateInHz / 1.25f) {
				resetPeak();
				timerIterator = 0;
			}

			repaint();
		}

		void setStyle(MeterStyle style) { meterStyle = style; }

		void setFillDirection(FillDirection direction) { fillDirection = direction; }

		void setRefreshRateInHz(int refreshRate /*clamped 1hz - 60hz. Defaults to 30hz*/) { refreshRateInHz = std::clamp(refreshRate, 1, 60); }

		void setThresholdEnabled(bool set) {
			isThresholdEnabled = set;
		}

		void setThresholdValue(float value) {
			thresholdValue = mapFromLog(value, Decibels::decibelsToGain(-80.f), Decibels::decibelsToGain(12.f));
			DBG("Threshold Value: " + juce::String(thresholdValue));
			DBG("jmapThresholdX: " + juce::String(thresholdX));
			//thresholdValue = value;
		}

		float mapFromLog(float valueInLogRange, float logRangeMin, float logRangeMax) // not a true map of log, but meant to smooth metering
		{
			jassert(logRangeMin > 0);
			jassert(logRangeMax > 0);

			auto logMin = std::log10f(logRangeMin);
			auto logMax = std::log10f(logRangeMax);

			return (std::log10f(valueInLogRange) - logMin) / (logMax - logMin);
		}

	private:
		bool isThresholdEnabled = false;
		float level = -80.0f;
		float maxLevel = -80.f;
		float thresholdValue = 0.0f;
		float thresholdX = 0.0f;
		int meterStyle = 0;
		int fillDirection = 0;
		int refreshRateInHz = 30;
		int timerIterator = 0;
	};
}