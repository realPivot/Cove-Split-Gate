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

    void setColoursForLNF(CoveLookAndFeel& lnf);

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

    std::unique_ptr<Drawable> lowpassSVG = juce::Drawable::createFromImageData(BinaryData::LowPassCurve_svg, BinaryData::LowPassCurve_svgSize);
    std::unique_ptr<Drawable> highpassSVG = juce::Drawable::createFromImageData(BinaryData::HighPassCurve_svg, BinaryData::HighPassCurve_svgSize);
    std::unique_ptr<Drawable> crossoverSVG = juce::Drawable::createFromImageData(BinaryData::CrossoverCurve_svg, BinaryData::CrossoverCurve_svgSize);
    std::unique_ptr<Drawable> cove_logo = juce::Drawable::createFromImageData(BinaryData::COVELogo_Bold_svg, BinaryData::COVELogo_Bold_svgSize);

    juce::DrawableButton coveLogoButton;
    
    const juce::Colour _MossGreen = juce::Colour(116, 142, 84);
    const juce::Colour _LavenderBlush = juce::Colour(238, 229, 233);
    const juce::Colour _SandyBrown = juce::Colour(255, 155, 66);
    const juce::Colour _RichBlack = juce::Colour(4, 21, 31);
    const juce::Colour _CaputMortuum = juce::Colour(88, 31, 24);
    
    const juce::Colour _Charcoal = juce::Colour(30, 70, 83);
    const juce::Colour _PersianGreen = juce::Colour(42, 157, 143);
    const juce::Colour _Saffron = juce::Colour(233, 196, 106);
    const juce::Colour _SandyOrange = juce::Colour(244, 162, 97);
    const juce::Colour _BurntSienna = juce::Colour(231, 111, 81);

    const juce::Colour _Isabelline = juce::Colour(242, 233, 228);
    const juce::Colour _PaleDogwood = juce::Colour(201, 173, 167);
    const juce::Colour _RoseQuartz = juce::Colour(154, 140, 152);
    const juce::Colour _UltraViolet = juce::Colour(74, 78, 105);
    const juce::Colour _SpaceCadet = juce::Colour(34, 34, 59);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoveSplitGateAudioProcessorEditor)
};
