/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Meter.h"

//==============================================================================
/**
*/
class CoveSplitGateAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    CoveSplitGateAudioProcessorEditor (CoveSplitGateAudioProcessor&);
    ~CoveSplitGateAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    CoveSplitGateAudioProcessor& audioProcessor;
    Gui::Meter lowMeterL, lowMeterR, highMeterL, highMeterR;



    int timerIterator = 0; // iterator for timer callback.
    float resetPeakTimeInSeconds = 1.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoveSplitGateAudioProcessorEditor)
};
