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
#include <cmath>

//==============================================================================
/*
*/
class SettingsMenu : public juce::Component
{
public:
    enum SliderType
    {
        Gain,
        Speed
    };

    SettingsMenu(juce::Value waveformGain, juce::Value stereo, juce::Value waveformSpeed) :
        waveformGainValue(waveformGain), 
        waveformSpeedValue(waveformSpeed),
        waveformStereoValue(stereo)
    {
        initSlider(waveformGainSlider, waveformGainValue, SliderType::Gain);
        initSlider(waveformSpeedSlider, waveformSpeedValue, SliderType::Speed);
        initButton(waveformStereoButton, waveformStereoValue);

        waveformStereoButton.onClick = [this]() {
            updateButton(waveformStereoButton);
        };
    }

    ~SettingsMenu() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        //g.drawText("Settings Menu", getLocalBounds(), juce::Justification::centredTop, true);
    }


    void initSlider(juce::Slider& slider, juce::Value& value, SliderType type) {
        addAndMakeVisible(slider);

        //Slider Settings
        slider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
        slider.setTextBoxStyle(Slider::TextBoxRight, true, 50, 20);
        if (type == SliderType::Gain) {
            slider.setNormalisableRange(logRange(Decibels::decibelsToGain(-25.0), Decibels::decibelsToGain(50.0)));
            slider.setDoubleClickReturnValue(true, Decibels::decibelsToGain(25.0));
        }
        else if (type == SliderType::Speed) {
            slider.setNormalisableRange(logRange(0.1, 4.0));
            slider.setDoubleClickReturnValue(true, 1.0);
        }
        
        

        // Bind the slider to the Value
        slider.getValueObject().referTo(value);

        // Initialize slider position from the Value
        slider.setValue(value.getValue());

        // Update slider text based on slider value
        slider.textFromValueFunction = [type](double sliderVal)
            {   if (type == SliderType::Gain) {
            return juce::String(Decibels::gainToDecibels(sliderVal), 2) + " dB";
        }
            else if (type == SliderType::Speed) {
            return juce::String(sliderVal * 100, 2, false) + " %";
        }
            };

        if (type == SliderType::Speed) {
            slider.onValueChange = [this, &slider]() {
                //e.updateVisualizerSettings(slider.getValue());
                };
        }

        // Update on creation
        slider.updateText();
    }

    void initButton(juce::Button& button, juce::Value value) {
        addAndMakeVisible(button);

        button.getToggleStateValue().referTo(value);
        button.setToggleable(true);
        button.setClickingTogglesState(true);

        updateButton(button);
        button.setToggleState(value.getValue(), dontSendNotification);
        
    }

    void initLabel(juce::Label& label) {

    }

    void updateButton(juce::Button& button) {
        if (&button == &waveformStereoButton) {
            auto buttonState = button.getToggleState();
            if (buttonState == true) {
                button.setButtonText("Stereo");
            }
            else {
                button.setButtonText("Mono");
            }
        }
    }

    void resized() override
    {
        auto bounds = getBounds().reduced(2);
        auto sectionAmount = 6;
        auto sectionHeight = bounds.getHeight() / sectionAmount;
        std::vector<juce::Rectangle<int>> sections;
        sections.clear();

        for (int i = 0; i < sectionAmount; i++) {
            sections.push_back(bounds.removeFromTop(sectionHeight));
        }

        waveformGainSlider.setBounds(sections[1]);
        waveformSpeedSlider.setBounds(sections[3]);
        waveformStereoButton.setBounds(sections[5]);
    }

private:

    juce::Slider waveformGainSlider;
    juce::Value waveformGainValue;

    juce::TextButton waveformStereoButton;
    juce::Value waveformStereoValue;

    juce::Slider waveformSpeedSlider;
    juce::Value waveformSpeedValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsMenu)
};
