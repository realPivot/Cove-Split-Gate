/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SettingsMenu.h"

typedef CoveSplitGateAudioProcessor::Channel _Channel;
typedef CoveSplitGateAudioProcessor::Band _Band;

//==============================================================================
CoveSplitGateAudioProcessorEditor::CoveSplitGateAudioProcessorEditor(CoveSplitGateAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), vts(p.getVts(false)), hiddenVts(p.getVts(true)),
    crossoverAttach(*vts.getParameter("crossover"), crossoverSlider),
    lowBypassAttach(*vts.getParameter("lowBypass"), lowBypassButton),
    lowThresholdAttach(*vts.getParameter("lowThreshold"), lowThresholdSlider),
    lowRatioAttach(*vts.getParameter("lowRatio"), lowRatioSlider),
    lowAttachAttach(*vts.getParameter("lowAttack"), lowAttackSlider),
    lowReleaseAttach(*vts.getParameter("lowRelease"), lowReleaseSlider),
    lowHoldAttach(*vts.getParameter("lowHold"), lowHoldSlider),
    lowMuteAttach(*vts.getParameter("lowMute"), lowMuteButton),
    highBypassAttach(*vts.getParameter("highBypass"), highBypassButton),
    highThresholdAttach(*vts.getParameter("highThreshold"), highThresholdSlider),
    highRatioAttach(*vts.getParameter("highRatio"), highRatioSlider),
    highAttachAttach(*vts.getParameter("highAttack"), highAttackSlider),
    highReleaseAttach(*vts.getParameter("highRelease"), highReleaseSlider),
    highHoldAttach(*vts.getParameter("highHold"), highHoldSlider),
    highMuteAttach(*vts.getParameter("highMute"), highMuteButton),
    coveLogoButton("Cove Logo", juce::DrawableButton::ButtonStyle::ImageFitted),
    splitLogoButton("Split Gate Logo", juce::DrawableButton::ButtonStyle::ImageFitted),
    advancedMenuButton("Advanced Settings", juce::DrawableButton::ButtonStyle::ImageFitted),
    audioVisualizerLow(2),
    audioVisualizerHigh(2)
{

    //setLookAndFeel(&coveLNF);
    LookAndFeel::setDefaultLookAndFeel(&coveLNF);
    setColoursForLNF(coveLNF);
    

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 550);

    // Meters
    //setMeterStyle(lowMeterL, Gui::Meter::Horizontal, Gui::Meter::Left);
    //setMeterStyle(lowMeterR, Gui::Meter::Horizontal, Gui::Meter::Left);
    //setMeterStyle(highMeterL, Gui::Meter::Horizontal, Gui::Meter::Left);
    //setMeterStyle(highMeterR, Gui::Meter::Horizontal, Gui::Meter::Left);
    audioVisualizerLow.setBufferSize(512);
    audioVisualizerLow.setSamplesPerBlock(p.getBlockSize());
    audioVisualizerLow.setNumChannels(2);
    addAndMakeVisible(audioVisualizerLow);
    
    audioVisualizerHigh.setBufferSize(512);
    audioVisualizerHigh.setSamplesPerBlock(p.getBlockSize());
    addAndMakeVisible(audioVisualizerHigh);

    startTimerHz(144); // how fast should rms value be updated for meters. Meter framerate updated in meter component.

    //Menu
    juce::Value waveformGainValue = hiddenVts.getParameterAsValue("waveformGain");

    menu.addSectionHeader("Waveform Settings");
    menu.addCustomItem(1, std::make_unique<SettingsMenu>(waveformGainValue));

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
    
    addAndMakeVisible(coveLogoButton);
    cove_logo->replaceColour(Colours::black, activeColours[2]);
    coveLogoButton.setImages(cove_logo.get());
    coveLogoButton.onClick = [this]() {
        juce::URL link("https://www.patreon.com/CoveDSP");
        link.launchInDefaultBrowser();
        };

    /* This doesn't seem to be working. I'm not going to worry about it for now.
    coveLogoButton.onStateChange = [this]() {
        auto state = coveLogoButton.getState();
        auto color = _RoseQuartz;
        switch (state)
        {
        case Button::ButtonState::buttonDown:
            cove_logo->replaceColour(_RoseQuartz, _RoseQuartz.darker());
            cove_logo->replaceColour(_RoseQuartz.brighter(), _RoseQuartz.darker());
            coveLogoButton.repaint();
            break;

        case Button::ButtonState::buttonOver:
            cove_logo->replaceColour(_RoseQuartz.darker(), _RoseQuartz.brighter());
            cove_logo->replaceColour(_RoseQuartz, _RoseQuartz.brighter());
            coveLogoButton.repaint();
            break;

        case Button::ButtonState::buttonNormal:
            cove_logo->replaceColour(_RoseQuartz.darker(), _RoseQuartz);
            cove_logo->replaceColour(_RoseQuartz.brighter(), _RoseQuartz);
            coveLogoButton.repaint();
            break;
        }
        };
    */

    addAndMakeVisible(splitLogoButton);
    splitgate_logo->replaceColour(Colours::black, activeColours[3]);
    splitLogoButton.setImages(splitgate_logo.get());
    splitLogoButton.onClick = [this]() {
        juce::URL link("https://github.com/realPivot/Cove-Split-Gate");
        link.launchInDefaultBrowser();
        };
    

    addAndMakeVisible(advancedMenuButton);
    gearIcon->replaceColour(Colours::black, activeColours[3]);
    advancedMenuButton.setImages(gearIcon.get());
    advancedMenuButton.onClick = [this]() {
        menu.showMenuAsync(juce::PopupMenu::Options().withTargetScreenArea(getBoundsInParent()), 
            [](int result) {
                if (result == 1)
                    DBG("SettingsMenu selected");
            });
        };

    addAndMakeVisible(lowMuteButton);
    lowMuteButton.setToggleable(true);
    lowMuteButton.setClickingTogglesState(true);
    lowMuteButton.setButtonText("Mute");

    addAndMakeVisible(highMuteButton);
    highMuteButton.setToggleable(true);
    highMuteButton.setClickingTogglesState(true);
    highMuteButton.setButtonText("Mute");

    // Sliders
    auto smallSliderTextBoxWidth = 70;
    auto bigSliderTextBoxWidth = 105;
    auto textBoxHeight = 20;
    auto sliderStyle = Slider::SliderStyle::LinearHorizontal;

    setSliderStyle(crossoverSlider, sliderStyle, Slider::TextBoxAbove, bigSliderTextBoxWidth, textBoxHeight, " hz", 0);

    setSliderStyle(lowThresholdSlider, sliderStyle, Slider::TextBoxRight, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(lowRatioSlider, sliderStyle, Slider::TextBoxRight, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(lowAttackSlider, sliderStyle, Slider::TextBoxRight, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(lowReleaseSlider, sliderStyle, Slider::TextBoxRight, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(lowHoldSlider, sliderStyle, Slider::TextBoxRight, smallSliderTextBoxWidth, textBoxHeight);

    setSliderStyle(highThresholdSlider, sliderStyle, Slider::TextBoxLeft, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(highRatioSlider, sliderStyle, Slider::TextBoxLeft, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(highAttackSlider, sliderStyle, Slider::TextBoxLeft, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(highReleaseSlider, sliderStyle, Slider::TextBoxLeft, smallSliderTextBoxWidth, textBoxHeight);
    setSliderStyle(highHoldSlider, sliderStyle, Slider::TextBoxLeft, smallSliderTextBoxWidth, textBoxHeight);

    lowThresholdSlider.valueFromTextFunction = [](const String &text) {
        auto input = text.retainCharacters("-0123456789.+");
        double value = input.getDoubleValue();
        return Decibels::decibelsToGain<double>(value, -80.f);
        };

    highThresholdSlider.valueFromTextFunction = [](const String& text) {
        auto input = text.retainCharacters("-0123456789.+");
        double value = input.getDoubleValue();
        return Decibels::decibelsToGain<double>(value, -80.f);
        };

    // Labels
    setLabelStyle(crossoverLabel, Justification::centredBottom);
    setLabelStyle(thresholdLabel, Justification::centredTop);
    setLabelStyle(ratioLabel, Justification::centredTop);
    setLabelStyle(attackLabel, Justification::centredTop);
    setLabelStyle(releaseLabel, Justification::centredTop);
    setLabelStyle(holdLabel, Justification::centredTop);
    setLabelStyle(versionLabel, Justification::topRight);

}

CoveSplitGateAudioProcessorEditor::~CoveSplitGateAudioProcessorEditor()
{
    LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void CoveSplitGateAudioProcessorEditor::updateVisualizer(const juce::AudioBuffer<float>& buffer, GateBand band)
{ // update the visualizer with buffer sent from audio processor
    
    switch (band) {
    case GateBand::LowBand:
        audioVisualizerLow.pushBuffer(buffer);
        break;
    case GateBand::HighBand:
        audioVisualizerHigh.pushBuffer(buffer);
        break;
    }
}

void CoveSplitGateAudioProcessorEditor::setMeterStyle(Gui::Meter& meter, Gui::Meter::MeterStyle style, Gui::Meter::FillDirection direction) 
{
    addAndMakeVisible(meter);
    meter.setThresholdEnabled(true);
    meter.setStyle(style);
    meter.setFillDirection(direction);
}

void CoveSplitGateAudioProcessorEditor::setSliderStyle(juce::Slider& slider, juce::Slider::SliderStyle style, juce::Slider::TextEntryBoxPosition textBoxPosition, int textBoxWidth, int textBoxHeight, juce::String suffix, int numDecimalPlaces) 
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(textBoxPosition, false, textBoxWidth, textBoxHeight);
    slider.setNumDecimalPlacesToDisplay(numDecimalPlaces);
    if (suffix.isNotEmpty()) {
        slider.setTextValueSuffix(suffix);
    }
}

void CoveSplitGateAudioProcessorEditor::setLabelStyle(juce::Label& label, juce::Justification justification) 
{
    addAndMakeVisible(label);
    label.setJustificationType(justification);
    label.setInterceptsMouseClicks(false, false);
}

void CoveSplitGateAudioProcessorEditor::setColoursForLNF(CoveLookAndFeel& lnf) {
    std::array<Gui::Meter*, 4> meters = { &lowMeterL, &lowMeterR, &highMeterL, &highMeterR };

    auto darkerAmount = .1f;

    // Light to Dark 
    juce::Colour colour_01 = activeColours[0].darker(darkerAmount * .5f);
    juce::Colour colour_02 = activeColours[1].darker(darkerAmount * 1.1);
    juce::Colour colour_03 = activeColours[2].darker(darkerAmount * 1.3);
    juce::Colour colour_04 = activeColours[3].darker(darkerAmount * 1.5f);
    juce::Colour colour_05 = activeColours[4].darker(darkerAmount * 2.f);

    // Lightest
    lnf.setColour(ResizableWindow::backgroundColourId, colour_01);

    // Medium-Light
    lnf.setColour(TextEditor::ColourIds::backgroundColourId, colour_02);
    lnf.setColour(Slider::ColourIds::trackColourId, colour_02);
    lnf.setColour(Slider::ColourIds::textBoxBackgroundColourId, colour_02);
    lnf.setColour(Slider::ColourIds::textBoxOutlineColourId, colour_02);

    // Medium
    lnf.setColour(Slider::backgroundColourId, colour_03);

    // Medium-Dark
    lnf.setColour(Slider::thumbColourId, colour_04);
    lnf.setColour(ToggleButton::tickColourId, colour_04);

    // Darkest
    lnf.setColour(Label::textColourId, colour_05);
    lnf.setColour(TextEditor::textColourId, colour_05);
    lnf.setColour(Slider::ColourIds::textBoxTextColourId, colour_05);
    lnf.setColour(Slider::ColourIds::textBoxOutlineColourId, colour_05);
    lnf.setColour(ToggleButton::tickDisabledColourId, colour_05);

    for (int i = 0; i < 4; i++) {
        meters[i]->setColour(Gui::Meter::Background, colour_04);
        meters[i]->setColour(Gui::Meter::Threshold, colour_04.interpolatedWith(colour_02, 0.5f));
        meters[i]->setColour(Gui::Meter::Level, colour_02);
        meters[i]->setColour(Gui::Meter::Peak, colour_03);
    }

    lnf.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
    lnf.setColour(TextButton::ColourIds::buttonColourId, juce::Colours::transparentWhite);
    lnf.setColour(TextButton::textColourOnId, colour_05);
    lnf.setColour(TextButton::textColourOffId, colour_03);
    lnf.setColour(TextButton::buttonOnColourId, colour_02);
    lnf.setColour(ComboBox::outlineColourId, juce::Colours::transparentWhite);

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

    auto area = getBounds();
    auto verticalSegment = area.getHeight() / 8;
    auto logoBand = area.removeFromTop(verticalSegment);
    auto top = area.removeFromTop(verticalSegment);
    auto header = area.removeFromTop(verticalSegment);
    auto leftHeader = header.removeFromLeft(header.getWidth() / 2);
    auto rightHeader = header;

    auto topQuarter = top.getWidth() / 4;
    auto topLeft = top.removeFromLeft(topQuarter);
    auto topRight = top.removeFromRight(topQuarter);

    auto leftSide = area.removeFromLeft(area.getWidth() / 2);
    auto rightSide = area;
    
    auto bgColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    auto bgGradient = juce::ColourGradient(bgColour, 0, logoBand.getBottom(), bgColour.brighter(), 0, logoBand.getY(),false);
    //g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker(0.1f));
    
    g.setGradientFill(bgGradient);
    g.fillRect(logoBand);

    lowpassSVG.get()->setDrawableTransform(AffineTransform::translation(topLeft.getCentre().translated(0, -25)));
    lowpassSVG->draw(g, 1.0f);

    highpassSVG.get()->setDrawableTransform(AffineTransform::translation(highBypassButton.getX(), topRight.getCentreY() - 25));
    highpassSVG->draw(g, 1.f);

    crossoverSVG.get()->setDrawableTransform(AffineTransform::translation(top.getCentre().translated( -(crossoverSVG.get()->getWidth() / 2), crossoverSVG.get()->getHeight() + 5)));
    crossoverSVG->draw(g, 1.f);

    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
    // Debug
    //g.setColour(juce::Colours::red);
    //g.drawRect(debugRect);

    // Debug
    if (debug)
    {
        auto& random = juce::Random::getSystemRandom();
        g.setColour(juce::Colours::red);
        g.drawLine(getWidth() / 4.f, getHeight(), getWidth() / 4.f, 0, 1.f); // Draw 25% Vertical Line
        g.drawLine(getWidth() / 2.f, getHeight(), getWidth() / 2.f, 0, 1.f); // Draw Center Vertical Line
        g.drawLine((getWidth() / 4.f) * 3.f, getHeight(), (getWidth() / 4.f) * 3.f, 0, 1.f); // Draw 75% Vertical Line
        g.drawLine(getWidth(), getHeight() / 2.f, 0, getHeight() / 2.f, 1.f); // Draw Center Horizontal Line

        g.setColour(juce::Colours::pink);
        g.drawLine(debugRect.getWidth() / 2.f, getHeight(), debugRect.getWidth() / 2.f, 0, 1.f); // Draw Vertical Line halfway in left component
        g.drawLine(debugRect_2.getX() + (debugRect_2.getWidth() / 2), getHeight(), debugRect_2.getX() + (debugRect_2.getWidth() / 2), 0, 1.f); // Draw Vertical line halfway in right component

        
        for (int i = 0; i < rectangles.size(); i++)
        {
            g.setColour(juce::Colour(random.nextInt(256), random.nextInt(256), random.nextInt(256)));
            g.drawRect(rectangles[i]);
        }
    }

    
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
    rectangles.clear();
    auto area = getBounds();
    auto verticalSegment = area.getHeight() / 8;

    auto logoBand = area.removeFromTop(verticalSegment);
    auto top = area.removeFromTop(verticalSegment);
    auto header = area.removeFromTop(verticalSegment);
    auto leftHeader = header.removeFromLeft(header.getWidth() / 2);
    auto rightHeader = header;

    auto topQuarter = top.getWidth() / 4;
    auto topLeft = top.removeFromLeft(topQuarter);
    auto topRight = top.removeFromRight(topQuarter);

    auto leftSide = area.removeFromLeft(area.getWidth() / 2);
    auto rightSide = area;

    rectangles.push_back(leftHeader);
    rectangles.push_back(rightHeader);
    rectangles.push_back(topLeft);
    rectangles.push_back(topRight);
    rectangles.push_back(leftSide);
    rectangles.push_back(rightSide);


    // Top
    lowBypassButton.setBounds(topLeft.reduced(15).translated(32,10));

    lowMuteButton.setBounds(topLeft.getX(), lowBypassButton.getY(), 50, 25);
    lowMuteButton.setCentrePosition(lowMuteButton.getBounds().getCentreX(), lowBypassButton.getBounds().getCentreY());

    highBypassButton.setBounds(topRight.reduced(15).transformed(juce::AffineTransform::translation(13, 10)));

    highMuteButton.setBounds(topRight.getRight() - 50, highBypassButton.getY(), 50, 25);
    highMuteButton.setCentrePosition(highMuteButton.getBounds().getCentreX(), highBypassButton.getBounds().getCentreY());

    crossoverSlider.setBounds(top.reduced(2));

    // Meter
    /*
    lowMeterL.setBounds(leftHeader.removeFromTop(leftHeader.getHeight() / 2).reduced(2));
    lowMeterR.setBounds(leftHeader.reduced(2));
    highMeterL.setBounds(rightHeader.removeFromTop(rightHeader.getHeight() / 2).reduced(2));
    highMeterR.setBounds(rightHeader.reduced(2));
    */
    // Waveform
    audioVisualizerLow.setBounds(leftHeader.removeFromTop(leftHeader.getHeight()).reduced(2));
    audioVisualizerHigh.setBounds(rightHeader.removeFromTop(rightHeader.getHeight()).reduced(2));

    // Left
    const int amountOfLeftComponents = 5;
    auto heightLeftComponents = leftSide.getHeight() / amountOfLeftComponents;
    std::array<juce::Rectangle<int>, amountOfLeftComponents> leftComponents;

    for (int i = 0; i < amountOfLeftComponents; i++) {
        leftComponents[i] = leftSide.removeFromTop(heightLeftComponents);
        rectangles.push_back(leftComponents[i]);
    }
    auto startingPos = 0;

    lowThresholdSlider.setBounds(leftComponents[startingPos]);
    lowRatioSlider.setBounds(leftComponents[startingPos + 1]);
    lowAttackSlider.setBounds(leftComponents[startingPos + 2]);
    lowReleaseSlider.setBounds(leftComponents[startingPos + 3]);
    lowHoldSlider.setBounds(leftComponents[startingPos + 4]);


    const int amountOfRightComponents = 5;
    auto heightRightComponents = rightSide.getHeight() / amountOfRightComponents;
    std::array<juce::Rectangle<int>, amountOfRightComponents> rightComponents;

    for (int i = 0; i < amountOfRightComponents; i++) {
        rightComponents[i] = rightSide.removeFromTop(heightRightComponents);
        rectangles.push_back(rightComponents[i]);
    }
    
    highThresholdSlider.setBounds(rightComponents[startingPos]);
    highRatioSlider.setBounds(rightComponents[startingPos + 1]);
    highAttackSlider.setBounds(rightComponents[startingPos + 2]);
    highReleaseSlider.setBounds(rightComponents[startingPos + 3]);
    highHoldSlider.setBounds(rightComponents[startingPos + 4]);

    // Labels
    auto textTop = getBounds().removeFromTop(verticalSegment);
    auto bottomText = getBounds().removeFromBottom(verticalSegment * jmax(amountOfLeftComponents, amountOfRightComponents));
    auto heightTextComponents = bottomText.getHeight() / (jmax(amountOfLeftComponents, amountOfRightComponents) * 2);
    std::array <juce::Rectangle<int>, jmax(amountOfLeftComponents, amountOfRightComponents) * 2> textComponents;
    for (int i = 0; i < textComponents.size(); i++) {
        textComponents[i] = bottomText.removeFromTop(heightTextComponents);
        //rectangles.push_back(textComponents[i]);
    }

    thresholdLabel.setBounds(textComponents[startingPos * 2]);
    ratioLabel.setBounds(textComponents[(startingPos * 2) + 2]);
    attackLabel.setBounds(textComponents[(startingPos * 2) + 4]);
    releaseLabel.setBounds(textComponents[(startingPos * 2) + 6]);
    holdLabel.setBounds(textComponents[(startingPos * 2) + 8]);

    
    //juce::Rectangle<int> bottom = Rectangle<int>(juce::Point<int>(0, jmax(lowHoldSlider.getBottom(), highHoldSlider.getBottom())), juce::Point<int>(getRight(), getBottom()));

    auto logoBandWidthInThird = logoBand.getWidth() / 3;
    auto logoBandLeft = logoBand.removeFromLeft(logoBandWidthInThird);
    auto logoBandMiddle = logoBand.removeFromLeft(logoBandWidthInThird);
    auto logoBandRight = logoBand;

    rectangles.push_back(logoBandLeft);
    rectangles.push_back(logoBandMiddle);
    rectangles.push_back(logoBandRight);

    //lowMuteButton.setBounds(bottomLeft.getX(), bottomLeft.getY(), 50,25);
    //lowMuteButton.setCentrePosition(bottomLeft.getCentre());
    coveLogoButton.setBounds(logoBandRight.reduced(20));
    coveLogoButton.setTopRightPosition(getBounds().getRight(), 0);
    coveLogoButton.setCentrePosition(coveLogoButton.getBounds().getCentreX(), logoBandMiddle.getCentreY());

    versionLabel.setBounds(logoBandRight);

    splitLogoButton.setBounds(logoBandLeft);
    advancedMenuButton.setBounds(logoBandMiddle.reduced(logoBandMiddle.getHeight() / 3));
    
    //highMuteButton.setBounds(bottomRight.getX(), bottomRight.getY(), 50, 25);
    //highMuteButton.setCentrePosition(bottomRight.getCentre());
    

}
