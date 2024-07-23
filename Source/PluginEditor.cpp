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
CoveSplitGateAudioProcessorEditor::CoveSplitGateAudioProcessorEditor(CoveSplitGateAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), vts(p.getVts()),
    crossoverAttach(*vts.getParameter("crossover"), crossoverSlider),
    lowBypassAttach(*vts.getParameter("lowBypass"), lowBypassButton),
    lowThresholdAttach(*vts.getParameter("lowThreshold"), lowThresholdSlider),
    lowRatioAttach(*vts.getParameter("lowRatio"), lowRatioSlider),
    lowAttachAttach(*vts.getParameter("lowAttack"), lowAttackSlider),
    lowReleaseAttach(*vts.getParameter("lowRelease"), lowReleaseSlider),
    lowHoldAttach(*vts.getParameter("lowHold"), lowHoldSlider),
    highBypassAttach(*vts.getParameter("highBypass"), highBypassButton),
    highThresholdAttach(*vts.getParameter("highThreshold"), highThresholdSlider),
    highRatioAttach(*vts.getParameter("highRatio"), highRatioSlider),
    highAttachAttach(*vts.getParameter("highAttack"), highAttackSlider),
    highReleaseAttach(*vts.getParameter("highRelease"), highReleaseSlider),
    highHoldAttach(*vts.getParameter("highHold"), highHoldSlider)
{
    setLookAndFeel(&coveLNF);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 550);

    // Meters
    addAndMakeVisible(lowMeterL);
    lowMeterL.setThresholdEnabled(true);
    addAndMakeVisible(lowMeterR);
    lowMeterR.setThresholdEnabled(true);
    addAndMakeVisible(highMeterL);
    highMeterL.setThresholdEnabled(true);
    addAndMakeVisible(highMeterR);
    highMeterR.setThresholdEnabled(true);

    lowMeterL.setStyle(Gui::Meter::Horizontal);
    lowMeterL.setFillDirection(Gui::Meter::Left);
    lowMeterR.setStyle(Gui::Meter::Horizontal);
    lowMeterR.setFillDirection(Gui::Meter::Left);
    highMeterL.setStyle(Gui::Meter::Horizontal);
    highMeterL.setFillDirection(Gui::Meter::Left);
    highMeterR.setStyle(Gui::Meter::Horizontal);
    highMeterR.setFillDirection(Gui::Meter::Left);

    startTimerHz(144); // how fast should rms value be updated for meters. Meter framerate updated in meter component.

    // Buttons
    addAndMakeVisible(lowBypassButton);
    lowBypassButton.onStateChange = [this]() {
        auto state = lowBypassButton.getToggleState();
        switch (state) {
        case true:
            lowMeterL.setThresholdEnabled(false);
            lowMeterR.setThresholdEnabled(false);
            lowThresholdSlider.setEnabled(false);
            lowRatioSlider.setEnabled(false);
            lowAttackSlider.setEnabled(false);
            lowReleaseSlider.setEnabled(false);
            lowHoldSlider.setEnabled(false);
            break;
        case false:
            lowMeterL.setThresholdEnabled(true);
            lowMeterR.setThresholdEnabled(true);
            lowThresholdSlider.setEnabled(true);
            lowRatioSlider.setEnabled(true);
            lowAttackSlider.setEnabled(true);
            lowReleaseSlider.setEnabled(true);
            lowHoldSlider.setEnabled(true);
            break;
        }
        };
        
    addAndMakeVisible(highBypassButton);
    highBypassButton.onStateChange = [this]() {
        auto state = highBypassButton.getToggleState();
        switch (state) {
        case true:
            highMeterL.setThresholdEnabled(false);
            highMeterR.setThresholdEnabled(false);
            highThresholdSlider.setEnabled(false);
            highRatioSlider.setEnabled(false);
            highAttackSlider.setEnabled(false);
            highReleaseSlider.setEnabled(false);
            highHoldSlider.setEnabled(false);
            break;
        case false:
            highMeterL.setThresholdEnabled(true);
            highMeterR.setThresholdEnabled(true);
            highThresholdSlider.setEnabled(true);
            highRatioSlider.setEnabled(true);
            highAttackSlider.setEnabled(true);
            highReleaseSlider.setEnabled(true);
            highHoldSlider.setEnabled(true);
            break;
        }
        };

    // Sliders
    addAndMakeVisible(crossoverSlider);
    crossoverSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    crossoverSlider.setTextBoxStyle(Slider::TextBoxAbove, false, 85, 20);

    addAndMakeVisible(lowThresholdSlider);
    lowThresholdSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowThresholdSlider.setTextBoxStyle(Slider::TextBoxRight, false, 70, 20);


    addAndMakeVisible(lowRatioSlider);
    lowRatioSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowRatioSlider.setTextBoxStyle(Slider::TextBoxRight, false, 70, 20);

    addAndMakeVisible(lowAttackSlider);
    lowAttackSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowAttackSlider.setTextBoxStyle(Slider::TextBoxRight, false, 70, 20);

    addAndMakeVisible(lowReleaseSlider);
    lowReleaseSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowReleaseSlider.setTextBoxStyle(Slider::TextBoxRight, false, 70, 20);

    addAndMakeVisible(lowHoldSlider);
    lowHoldSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowHoldSlider.setTextBoxStyle(Slider::TextBoxRight, false, 70, 20);

    addAndMakeVisible(highThresholdSlider);
    highThresholdSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highThresholdSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 70, 20);

    addAndMakeVisible(highRatioSlider);
    highRatioSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highRatioSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 70, 20);

    addAndMakeVisible(highAttackSlider);
    highAttackSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highAttackSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 70, 20);

    addAndMakeVisible(highReleaseSlider);
    highReleaseSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highReleaseSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 70, 20);

    addAndMakeVisible(highHoldSlider);
    highHoldSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highHoldSlider.setTextBoxStyle(Slider::TextBoxLeft, false, 70, 20);

    

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
    
    // Debug
    //g.setColour(juce::Colours::red);
    //g.drawRect(debugRect);
}


void CoveSplitGateAudioProcessorEditor::timerCallback() {
    lowMeterL.setLevel(audioProcessor.getRmsValue(_Channel::left, _Band::low));
    lowMeterR.setLevel(audioProcessor.getRmsValue(_Channel::right, _Band::low));
    highMeterL.setLevel(audioProcessor.getRmsValue(_Channel::left, _Band::high));
    highMeterR.setLevel(audioProcessor.getRmsValue(_Channel::right, _Band::high));

    auto lowSliderValue = lowThresholdSlider.getValue();
    lowMeterL.setThresholdValue(lowSliderValue);
    lowMeterR.setThresholdValue(lowSliderValue);

    auto highSliderValue = highThresholdSlider.getValue();
    highMeterL.setThresholdValue(highSliderValue);
    highMeterR.setThresholdValue(highSliderValue);
}

void CoveSplitGateAudioProcessorEditor::resized()
{
    auto area = getBounds();
    auto verticalSegment = area.getHeight() / 8;
    auto top = area.removeFromTop(verticalSegment);
    auto header = area.removeFromTop(verticalSegment);
    auto leftHeader = header.removeFromLeft(header.getWidth() / 2);
    auto rightHeader = header;

    auto topQuarter = top.getWidth() / 4;
    auto topLeft = top.removeFromLeft(topQuarter);
    auto topRight = top.removeFromRight(topQuarter);

    auto leftSide = area.removeFromLeft(area.getWidth() / 2);
    auto rightSide = area;



    // Top
    lowBypassButton.setBounds(topLeft.reduced(15).translated(50,10));
    highBypassButton.setBounds(topRight.reduced(15).transformed(juce::AffineTransform::translation(0, 10)));
    crossoverSlider.setBounds(top.reduced(2));

    // Meter
    lowMeterL.setBounds(leftHeader.removeFromTop(leftHeader.getHeight() / 2).reduced(2));
    lowMeterR.setBounds(leftHeader.reduced(2));
    highMeterL.setBounds(rightHeader.removeFromTop(rightHeader.getHeight() / 2).reduced(2));
    highMeterR.setBounds(rightHeader.reduced(2));

    // Left
    const int amountOfLeftComponents = 6;
    auto heightLeftComponents = leftSide.getHeight() / amountOfLeftComponents;
    std::array<juce::Rectangle<int>, amountOfLeftComponents> leftComponents;

    for (int i = 0; i < amountOfLeftComponents; i++) {
        leftComponents[i] = leftSide.removeFromTop(heightLeftComponents);
    }

    lowThresholdSlider.setBounds(leftComponents[0]);
    lowRatioSlider.setBounds(leftComponents[1]);
    lowAttackSlider.setBounds(leftComponents[2]);
    lowReleaseSlider.setBounds(leftComponents[3]);
    lowHoldSlider.setBounds(leftComponents[4]);


    // Right

    const int amountOfRightComponents = 6;
    auto heightRightComponents = rightSide.getHeight() / amountOfRightComponents;
    std::array<juce::Rectangle<int>, amountOfRightComponents> rightComponents;

    for (int i = 0; i < amountOfRightComponents; i++) {
        rightComponents[i] = rightSide.removeFromTop(heightRightComponents);
    }

    highThresholdSlider.setBounds(rightComponents[0]);
    highRatioSlider.setBounds(rightComponents[1]);
    highAttackSlider.setBounds(rightComponents[2]);
    highReleaseSlider.setBounds(rightComponents[3]);
    highHoldSlider.setBounds(rightComponents[4]);
    
}
