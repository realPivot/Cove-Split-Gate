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
    AudioProcessorValueTreeState& vts;
    Gui::Meter lowMeterL, lowMeterR, highMeterL, highMeterR;

    juce::Rectangle<int> debugRect;

    juce::Slider crossoverSlider;
    juce::SliderParameterAttachment crossoverAttach;

    juce::ToggleButton lowBypassButton;
    juce::ButtonParameterAttachment lowBypassAttach;

    juce::ToggleButton highBypassButton;
    juce::ButtonParameterAttachment highBypassAttach;

    juce::Slider lowThresholdSlider;
    juce::SliderParameterAttachment lowThresholdAttach;

    juce::Slider lowRatioSlider;
    juce::SliderParameterAttachment lowRatioAttach;

    juce::Slider lowAttackSlider;
    juce::SliderParameterAttachment lowAttachAttach;

    juce::Slider lowReleaseSlider;
    juce::SliderParameterAttachment lowReleaseAttach;

    juce::Slider lowHoldSlider;
    juce::SliderParameterAttachment lowHoldAttach;

    juce::Slider highThresholdSlider;
    juce::SliderParameterAttachment highThresholdAttach;

    juce::Slider highRatioSlider;
    juce::SliderParameterAttachment highRatioAttach;

    juce::Slider highAttackSlider;
    juce::SliderParameterAttachment highAttachAttach;

    juce::Slider highReleaseSlider;
    juce::SliderParameterAttachment highReleaseAttach;

    juce::Slider highHoldSlider;
    juce::SliderParameterAttachment highHoldAttach;

    

    int timerIterator = 0; // iterator for timer callback.
    float resetPeakTimeInSeconds = 1.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoveSplitGateAudioProcessorEditor)
};
