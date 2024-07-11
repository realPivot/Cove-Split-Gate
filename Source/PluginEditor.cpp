/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

typedef CoveSplitGateAudioProcessor::Channel _Channel;
typedef CoveSplitGateAudioProcessor::Band _Band;

//==============================================================================
CoveSplitGateAudioProcessorEditor::CoveSplitGateAudioProcessorEditor (CoveSplitGateAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400 , 550);

    addAndMakeVisible(lowMeterL);
    addAndMakeVisible(lowMeterR);
    addAndMakeVisible(highMeterL);
    addAndMakeVisible(highMeterR);

    lowMeterL.setStyle(Gui::Meter::Horizontal);
    lowMeterL.setFillDirection(Gui::Meter::Left);
    lowMeterR.setStyle(Gui::Meter::Horizontal);
    lowMeterR.setFillDirection(Gui::Meter::Left);
    highMeterL.setStyle(Gui::Meter::Horizontal);
    highMeterL.setFillDirection(Gui::Meter::Left);
    highMeterR.setStyle(Gui::Meter::Horizontal);
    highMeterR.setFillDirection(Gui::Meter::Left);
    

    startTimerHz(144); // how fast should rms value be updated for meters. Meter framerate updated in meter component.
}

CoveSplitGateAudioProcessorEditor::~CoveSplitGateAudioProcessorEditor()
{
}

//==============================================================================
void CoveSplitGateAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}


void CoveSplitGateAudioProcessorEditor::timerCallback() {
    lowMeterL.setLevel(audioProcessor.getRmsValue(_Channel::left, _Band::low));
    lowMeterR.setLevel(audioProcessor.getRmsValue(_Channel::right, _Band::low));
    highMeterL.setLevel(audioProcessor.getRmsValue(_Channel::left, _Band::high));
    highMeterR.setLevel(audioProcessor.getRmsValue(_Channel::right, _Band::high));
}

void CoveSplitGateAudioProcessorEditor::resized()
{
    auto area = getBounds();
    auto header = area.removeFromTop(area.getHeight() / 10);
    auto leftHeader = header.removeFromLeft(header.getWidth() / 2);
    auto rightHeader = header;
    
    lowMeterL.setBounds(leftHeader.removeFromTop(leftHeader.getHeight() / 2).reduced(2));
    lowMeterR.setBounds(leftHeader.reduced(2));
    highMeterL.setBounds(rightHeader.removeFromTop(rightHeader.getHeight() / 2).reduced(2));
    highMeterR.setBounds(rightHeader.reduced(2));
}
