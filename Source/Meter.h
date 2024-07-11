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

		Meter() { startTimerHz(refreshRateInHz); } // Blank Constructor

		Meter(MeterStyle style, FillDirection direction) { // Constructor where style is defined
			meterStyle = style;
			fillDirection = direction;
			startTimerHz(refreshRateInHz);
		}

		~Meter() { // Destructor
		}

		void paint(juce::Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat(); // get bounds of meter
			auto cornerSize = 5.0f; // size of corner

			// Background
			g.setColour(Colour(209, 222, 222).withBrightness(0.2f));
			g.fillRoundedRectangle(bounds, 5.0f);

			// Foreground
			g.setColour(Colour(209,222,222)); // Color of filled bar

			const auto scaledX = juce::jmap(level, -80.0f, 0.0f, 0.0f, static_cast<float>(getWidth())); // position for filled levelMeter in x orientation
			const auto scaledY = juce::jmap(level, -80.0f, 0.0f, 0.0f, static_cast<float>(getHeight())); // position for filled levelMeter in y orientation
			const auto scaledXPeak = juce::jmap(maxLevel, -80.0f, 0.0f, 0.0f, static_cast<float>(getWidth())); // position for peak in x orientation
			const auto scaledYPeak = juce::jmap(maxLevel, -80.0f, 0.0f, 0.0f, static_cast<float>(getHeight())); // position for filled levelMeter in y orientation

			switch (meterStyle)
			{
			case MeterStyle::Horizontal:
				
				if (fillDirection == FillDirection::Left)
				{
					auto bounds2 = bounds; // copy area of bounds into second object to get peak
					auto filledBounds = bounds.removeFromLeft(scaledX); // remove filled meter area from bounds object
					auto peakBounds = bounds2.removeFromLeft(scaledXPeak); // remove peak area from copied bounds object
					
					g.fillRoundedRectangle(filledBounds, cornerSize);
					g.setColour(Colour(29, 32, 31)); // Color of peak bar
					g.fillRect(peakBounds.getRight(), peakBounds.getY(), 2.f, bounds.getHeight());
					
				}
				else if (fillDirection == FillDirection::Right) {
					auto bounds2 = bounds; // copy area of bounds into second object to get peak
					auto filledBounds = bounds.removeFromRight(scaledX); // remove filled meter area from bounds object
					auto peakBounds = bounds2.removeFromRight(scaledXPeak);// remove peak area from copied bounds object

					g.fillRoundedRectangle(filledBounds, cornerSize);
					g.setColour(Colour(29, 32, 31)); // Color of peak bar
					g.fillRect(peakBounds.getX(), peakBounds.getY(), 2.f, bounds.getHeight());

				}
				break;

			case MeterStyle::Vertical:
				auto bounds2 = bounds; // copy area of bounds into second object to get peak
				auto filledBounds = bounds.removeFromBottom(scaledY); // remove filled meter area from bounds object
				auto peakBounds = bounds2.removeFromBottom(scaledYPeak); // remove peak area from copied bounds object

				g.fillRoundedRectangle(filledBounds, cornerSize);
				g.setColour(Colour(29, 32, 31)); // Color of peak bar
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
	private:

		float level = -80.0f;
		float maxLevel = -80.f;
		int meterStyle = 0;
		int fillDirection = 0;
		int refreshRateInHz = 30;
		int timerIterator = 0;
	};
}