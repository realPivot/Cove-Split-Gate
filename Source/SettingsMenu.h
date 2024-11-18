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

//==============================================================================
/*
*/
class SettingsMenu : public juce::PopupMenu::CustomComponent
{
public:
    SettingsMenu(juce::AudioProcessorValueTreeState& vts) :
        waveformGainAttach(*vts.getParameter("waveformGain"), waveformGainSlider)
    {
        addAndMakeVisible(waveformGainSlider);
        waveformGainSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
        waveformGainSlider.setTextBoxStyle(Slider::TextBoxRight, true, 25, 10);

    }

    ~SettingsMenu() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

        g.setColour (juce::Colours::grey);
        g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

        g.setColour (juce::Colours::white);
        g.setFont (juce::FontOptions (14.0f));
        g.drawText ("SettingsMenu", getLocalBounds(),
                    juce::Justification::centred, true);   // draw some placeholder text
    }

    void resized() override
    {
        waveformGainSlider.setBounds(0,0, getWidth(), getHeight() / 3);
    }

private:

    juce::Slider waveformGainSlider;
    juce::SliderParameterAttachment waveformGainAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsMenu)
};
