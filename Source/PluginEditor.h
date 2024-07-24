/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Meter.h"
#include "CoveLookAndFeel.h"

//==============================================================================
/**
*/
class CoveSplitGateAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    enum GateBand
    {
        LowBand,
        HighBand
    };

    CoveSplitGateAudioProcessorEditor (CoveSplitGateAudioProcessor&);
    ~CoveSplitGateAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

    void setGateState(GateBand band, bool state);

private:
    CoveSplitGateAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& vts;
    CoveLookAndFeel coveLNF;
    ComponentBoundsConstrainer constrainer;

    Gui::Meter lowMeterL, lowMeterR, highMeterL, highMeterR;

    juce::Rectangle<int> debugRect, debugRect_2;

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

    juce::Label thresholdLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label ratioLabel;
    juce::Label holdLabel;
    juce::Label crossoverLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoveSplitGateAudioProcessorEditor)
};
