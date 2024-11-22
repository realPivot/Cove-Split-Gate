/*
  ==============================================================================

    SettingsMenu.h
    Created: 18 Nov 2024 1:53:10pm
    Author:  tjbac

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <../Source/PluginProcessor.h>
#include <../Source/SliderUtils.h>

//==============================================================================
/*
*/
class SettingsMenu : public juce::PopupMenu::CustomComponent
{
public:
    SettingsMenu(juce::Value waveformGain) : waveformGainValue(waveformGain)
    {
        initSlider(waveformGainSlider, waveformGainValue);
    }

    ~SettingsMenu() override
    {
    }

    void getIdealSize(int& idealWidth, int& idealHeight) override {
        idealWidth = 200;
        idealHeight = 60;
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        //g.drawText("Settings Menu", getLocalBounds(), juce::Justification::centredTop, true);
    }

    void resized() override
    {
        waveformGainSlider.setBounds(10,30, getWidth() - 20, 20);
    }

    void initSlider(juce::Slider& slider, juce::Value& value) {
        addAndMakeVisible(slider);

        //Slider Settings
        slider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
        slider.setTextBoxStyle(Slider::TextBoxRight, true, 50, 20);
        slider.setNormalisableRange(logRange(Decibels::decibelsToGain(-30.0), Decibels::decibelsToGain(30.0)));

        // Bind the slider to the Value
        slider.getValueObject().referTo(value);

        // Initialize slider position from the Value
        slider.setValue(value.getValue());

        // Update slider text based on slider value
        slider.textFromValueFunction = [](double sliderVal)
            {
                return juce::String(Decibels::gainToDecibels(sliderVal), 5) + " dB";
            };

        // Update on creation
        slider.updateText();
    }

private:

    juce::Slider waveformGainSlider;
    juce::Value waveformGainValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsMenu)
};
