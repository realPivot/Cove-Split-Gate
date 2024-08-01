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

    juce::TextButton lowMuteButton;
    juce::ButtonParameterAttachment lowMuteAttach;

    juce::TextButton highMuteButton;
    juce::ButtonParameterAttachment highMuteAttach;

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

    juce::Label thresholdLabel{"ThresholdLabel", "Threshold"};
    juce::Label attackLabel{"AttackLabel", "Attack"};
    juce::Label releaseLabel{"ReleaseLabel", "Release"};
    juce::Label ratioLabel{"RatioLabel", "Ratio"};
    juce::Label holdLabel{"HoldLabel", "Hold"};
    juce::Label crossoverLabel{"CrossoverLabel","Crossover" };
    juce::Label versionLabel{"VersionLabel", JucePlugin_VersionString};

    std::unique_ptr<Drawable> lowpassSVG = juce::Drawable::createFromImageData(BinaryData::LowPassCurve_svg, BinaryData::LowPassCurve_svgSize);
    std::unique_ptr<Drawable> highpassSVG = juce::Drawable::createFromImageData(BinaryData::HighPassCurve_svg, BinaryData::HighPassCurve_svgSize);
    std::unique_ptr<Drawable> crossoverSVG = juce::Drawable::createFromImageData(BinaryData::CrossoverCurve_svg, BinaryData::CrossoverCurve_svgSize);
    std::unique_ptr<Drawable> cove_logo = juce::Drawable::createFromImageData(BinaryData::COVE_TextOnly_svg, BinaryData::COVE_TextOnly_svgSize);
    std::unique_ptr<Drawable> splitgate_logo = juce::Drawable::createFromImageData(BinaryData::SplitGate_Logo_svg, BinaryData::SplitGate_Logo_svgSize);

    juce::DrawableButton coveLogoButton;
    juce::DrawableButton splitLogoButton;
    
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

    const std::array<juce::Colour, 5> activeColours{_Isabelline, _PaleDogwood, _RoseQuartz, _UltraViolet, _SpaceCadet};

    void setMeterStyle(Gui::Meter& meter, Gui::Meter::MeterStyle style, Gui::Meter::FillDirection direction);
    void setButtonStyle(juce::Button& button);
    void setSliderStyle(juce::Slider& slider, juce::Slider::SliderStyle style, juce::Slider::TextEntryBoxPosition textBoxPosition, int textBoxWidth = 0, int textBoxHeight = 0, juce::String suffix = "", int numDecimalPlaces = 1);
    void setLabelStyle(juce::Label& label, juce::Justification justification);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoveSplitGateAudioProcessorEditor)
};
