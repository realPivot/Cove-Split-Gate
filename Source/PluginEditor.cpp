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
    //setLookAndFeel(&coveLNF);
    LookAndFeel::setDefaultLookAndFeel(&coveLNF);
    coveLNF.setColour(ResizableWindow::backgroundColourId, _Isabelline);
    
    coveLNF.setColour(TextEditor::ColourIds::backgroundColourId, _PaleDogwood);
    coveLNF.setColour(Slider::ColourIds::trackColourId, _PaleDogwood);
    coveLNF.setColour(Slider::ColourIds::textBoxBackgroundColourId, _PaleDogwood);
    coveLNF.setColour(Slider::ColourIds::textBoxOutlineColourId, _PaleDogwood);
    
    coveLNF.setColour(Slider::backgroundColourId, _RoseQuartz);
    
        coveLNF.setColour(Slider::thumbColourId, _UltraViolet);
    coveLNF.setColour(ToggleButton::tickColourId, _UltraViolet);

    coveLNF.setColour(Label::textColourId, _SpaceCadet);
    coveLNF.setColour(TextEditor::textColourId, _SpaceCadet);
    coveLNF.setColour(Slider::ColourIds::textBoxTextColourId, _SpaceCadet);
    coveLNF.setColour(Slider::ColourIds::textBoxOutlineColourId, _SpaceCadet);
    coveLNF.setColour(ToggleButton::tickDisabledColourId, _SpaceCadet);
    

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
    auto lowBypassValue = (*vts.getRawParameterValue("lowBypass") >= 0.5f) ? true : false;
    lowBypassButton.setToggleState(lowBypassValue, juce::dontSendNotification);
    lowBypassValue ? setGateState(LowBand, false) : setGateState(LowBand, true);
    lowBypassButton.onStateChange = [this]() {
        auto state = lowBypassButton.getToggleState();
        switch (state) {
        case true:
            setGateState(GateBand::LowBand, false);
            break;
        case false:
            setGateState(GateBand::LowBand, true);
            break;
        }
        };
    
        
    addAndMakeVisible(highBypassButton);
    auto highBypassValue = (*vts.getRawParameterValue("highBypass") >= 0.5f) ? true : false;
    highBypassButton.setToggleState(highBypassValue, juce::dontSendNotification);
    highBypassValue ? setGateState(HighBand, false) : setGateState(HighBand, true);
    highBypassButton.onClick = [this]() {
        auto state = highBypassButton.getToggleState();
        switch (state) {
        case true:
            setGateState(GateBand::HighBand, false);
            break;
        case false:
            setGateState(GateBand::HighBand, true);
            break;
        }
        };
    
    // Sliders
    auto smallSliderTextBoxWidth = 70;
    auto bigSliderTextBoxWidth = 105;
    auto textBoxHeight = 20;

    addAndMakeVisible(crossoverSlider);
    crossoverSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    crossoverSlider.setTextBoxStyle(Slider::TextBoxAbove, false, bigSliderTextBoxWidth, textBoxHeight);
    crossoverSlider.setNumDecimalPlacesToDisplay(0);
    crossoverSlider.setTextValueSuffix(" hz");

    addAndMakeVisible(lowThresholdSlider);
    lowThresholdSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowThresholdSlider.setTextBoxStyle(Slider::TextBoxRight, false, smallSliderTextBoxWidth, textBoxHeight);


    addAndMakeVisible(lowRatioSlider);
    lowRatioSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowRatioSlider.setTextBoxStyle(Slider::TextBoxRight, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(lowAttackSlider);
    lowAttackSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowAttackSlider.setTextBoxStyle(Slider::TextBoxRight, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(lowReleaseSlider);
    lowReleaseSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowReleaseSlider.setTextBoxStyle(Slider::TextBoxRight, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(lowHoldSlider);
    lowHoldSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    lowHoldSlider.setTextBoxStyle(Slider::TextBoxRight, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(highThresholdSlider);
    highThresholdSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highThresholdSlider.setTextBoxStyle(Slider::TextBoxLeft, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(highRatioSlider);
    highRatioSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highRatioSlider.setTextBoxStyle(Slider::TextBoxLeft, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(highAttackSlider);
    highAttackSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highAttackSlider.setTextBoxStyle(Slider::TextBoxLeft, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(highReleaseSlider);
    highReleaseSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highReleaseSlider.setTextBoxStyle(Slider::TextBoxLeft, false, smallSliderTextBoxWidth, textBoxHeight);

    addAndMakeVisible(highHoldSlider);
    highHoldSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    highHoldSlider.setTextBoxStyle(Slider::TextBoxLeft, false, smallSliderTextBoxWidth, textBoxHeight);

    // Labels
    addAndMakeVisible(crossoverLabel);
    crossoverLabel.setText("Crossover", dontSendNotification);
    crossoverLabel.setJustificationType(Justification::centredBottom);
    crossoverLabel.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(thresholdLabel);
    thresholdLabel.setText("Threshold", dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centredTop);
    thresholdLabel.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(ratioLabel);
    ratioLabel.setText("Ratio", dontSendNotification);
    ratioLabel.setJustificationType(juce::Justification::centredTop);
    ratioLabel.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centredTop);
    attackLabel.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centredTop);
    releaseLabel.setInterceptsMouseClicks(false, false);
    
    addAndMakeVisible(holdLabel);
    holdLabel.setText("Hold", dontSendNotification);
    holdLabel.setJustificationType(juce::Justification::centredTop);
    holdLabel.setInterceptsMouseClicks(false, false);

}

CoveSplitGateAudioProcessorEditor::~CoveSplitGateAudioProcessorEditor()
{
    LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void CoveSplitGateAudioProcessorEditor::setGateState(GateBand band, bool state /*Value to set for the band*/) {
    switch (band)
    {
    case GateBand::LowBand:
        lowMeterL.setThresholdEnabled(!state);
        lowMeterR.setThresholdEnabled(!state);
        lowThresholdSlider.setEnabled(!state);
        lowRatioSlider.setEnabled(!state);
        lowAttackSlider.setEnabled(!state);
        lowReleaseSlider.setEnabled(!state);
        lowHoldSlider.setEnabled(!state);
        break;

    case GateBand::HighBand:
        highMeterL.setThresholdEnabled(!state);
        highMeterR.setThresholdEnabled(!state);
        highThresholdSlider.setEnabled(!state);
        highRatioSlider.setEnabled(!state);
        highAttackSlider.setEnabled(!state);
        highReleaseSlider.setEnabled(!state);
        highHoldSlider.setEnabled(!state);
        break;
    }

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

    // Debug
    /*
    g.setColour(juce::Colours::red);
    g.drawLine(getWidth() / 4.f, getHeight(), getWidth() / 4.f, 0, 1.f); // Draw 25% Vertical Line
    g.drawLine(getWidth() / 2.f, getHeight(), getWidth() / 2.f, 0, 1.f); // Draw Center Vertical Line
    g.drawLine((getWidth() / 4.f) * 3.f, getHeight(), (getWidth() / 4.f) * 3.f, 0, 1.f); // Draw 75% Vertical Line
    g.drawLine(getWidth(), getHeight() / 2.f, 0, getHeight() / 2.f, 1.f); // Draw Center Horizontal Line

    g.setColour(juce::Colours::pink);
    g.drawLine(debugRect.getWidth() / 2.f, getHeight(), debugRect.getWidth() / 2.f, 0, 1.f); // Draw Vertical Line halfway in left component
    g.drawLine(debugRect_2.getX() + (debugRect_2.getWidth() / 2), getHeight(), debugRect_2.getX() + (debugRect_2.getWidth() / 2), 0, 1.f); // Draw Vertical line halfway in right component
    */
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
    lowBypassButton.setBounds(topLeft.reduced(15).translated(32,10));
    highBypassButton.setBounds(topRight.reduced(15).transformed(juce::AffineTransform::translation(13, 10)));
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

    // Debug
    debugRect = leftComponents[0];
    debugRect.removeFromRight(lowThresholdSlider.getTextBoxWidth() + (coveLNF.getSliderThumbRadius(lowThresholdSlider) / 5));

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

    // Debug
    debugRect_2 = rightComponents[0];
    debugRect_2.removeFromLeft(coveLNF.getSliderLayout(highThresholdSlider).textBoxBounds.getWidth());

    // Labels
    auto textTop = getBounds().removeFromTop(verticalSegment);
    auto bottom = getBounds().removeFromBottom(verticalSegment * 6);
    auto heightTextComponents = bottom.getHeight() / (jmax(amountOfLeftComponents, amountOfRightComponents) * 2);
    std::array <juce::Rectangle<int>, jmax(amountOfLeftComponents, amountOfRightComponents) * 2> textComponents;
    for (int i = 0; i < textComponents.size(); i++) {
        textComponents[i] = bottom.removeFromTop(heightTextComponents);
    }
    crossoverLabel.setBounds(textTop);
    thresholdLabel.setBounds(textComponents[0]);
    ratioLabel.setBounds(textComponents[2]);
    attackLabel.setBounds(textComponents[4]);
    releaseLabel.setBounds(textComponents[6]);
    holdLabel.setBounds(textComponents[8]);

    
    
}
