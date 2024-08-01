/*
  ==============================================================================

    CoveLookAndFeel.h
    Created: 22 May 2024 6:36:33pm
    Author:  tjbac

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once
class CoveLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CoveLookAndFeel() {

        setDefaultSansSerifTypeface(getTypefaceForFont(getNotoThinFont()));
    };
    // Sliders
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
        auto fill = slider.findColour(Slider::rotarySliderFillColourId);

        auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(6);

        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin(8.0f, radius * 0.5f);
        auto thumbW = radius * .08f;
        auto arcRadius = radius - lineW * 0.5f;

        Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);

        g.setColour(outline);
        g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

        if (slider.isEnabled())
        {
            Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);

            g.setColour(fill);
            g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
        }

        Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
            bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
        Line<float> thumbLine(bounds.getCentre(), thumbPoint);
        auto newStart = thumbLine.getPointAlongLineProportionally(.9f);
        auto newEnd = thumbLine.getPointAlongLineProportionally(1.1f);
        thumbLine.setStart(newStart);
        thumbLine.setEnd(newEnd);

        Path thumbPath;
        thumbPath.addLineSegment(thumbLine, thumbW);


        g.setColour(slider.findColour(Slider::thumbColourId));
        //g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
        g.strokePath(thumbPath, PathStrokeType(thumbW, PathStrokeType::curved, PathStrokeType::EndCapStyle::square));
    }

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const Slider::SliderStyle style, Slider& slider) override
    {
        if (slider.isBar())
        {
            g.setColour(slider.findColour(Slider::trackColourId));
            g.fillRect(slider.isHorizontal() ? Rectangle<float>(static_cast<float> (x), (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
                : Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));

            drawLinearSliderOutline(g, x, y, width, height, style, slider);
        }
        else
        {
            auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
            auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

            auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

            Point<int> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
                slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

            Point<int> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
                slider.isHorizontal() ? startPoint.y : (float)y);

            Path backgroundTrack;
            backgroundTrack.startNewSubPath(startPoint.toFloat());
            backgroundTrack.lineTo(endPoint.toFloat());
            g.setColour(slider.findColour(Slider::backgroundColourId));
            g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::square });

            Path valueTrack;
            Point<int> minPoint, maxPoint, thumbPoint;

            if (isTwoVal || isThreeVal)
            {
                minPoint = { slider.isHorizontal() ? juce::roundToInt(minSliderPos) : juce::roundToInt((float)width * 0.5f),
                             slider.isHorizontal() ? juce::roundToInt((float)height * 0.5f) : juce::roundToInt(minSliderPos) };

                if (isThreeVal)
                    thumbPoint = { slider.isHorizontal() ? juce::roundToInt(sliderPos) : juce::roundToInt((float)width * 0.5f),
                                   slider.isHorizontal() ? juce::roundToInt((float)height * 0.5f) : juce::roundToInt(sliderPos) };

                maxPoint = { slider.isHorizontal() ? juce::roundToInt(maxSliderPos) : juce::roundToInt((float)width * 0.5f),
                             slider.isHorizontal() ? juce::roundToInt((float)height * 0.5f) : juce::roundToInt(maxSliderPos) };
            }
            else
            {
                int kx = slider.isHorizontal() ? juce::roundToInt(sliderPos) : juce::roundToInt(((float)x + (float)width * 0.5f));
                int ky = slider.isHorizontal() ? juce::roundToInt(((float)y + (float)height * 0.5f)) : juce::roundToInt(sliderPos);

                minPoint = startPoint;
                maxPoint = { kx, ky };
            }

            auto thumbWidth = getSliderThumbRadius(slider);

            valueTrack.startNewSubPath(minPoint.toFloat());
            valueTrack.lineTo(isThreeVal ? thumbPoint.toFloat() : maxPoint.toFloat());
            g.setColour(slider.findColour(Slider::trackColourId));
            g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::square });

            if (!isTwoVal) // draw thumb for horizontal / threeVal slider
            {
                g.setColour(slider.findColour(Slider::thumbColourId));
                //g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint)); // old thumb
                g.fillRect(Rectangle<int>(static_cast<int> (thumbWidth) / 2, static_cast<int> (thumbWidth)).withCentre(isThreeVal ? thumbPoint.roundToInt() : maxPoint.roundToInt()));
                g.setColour(slider.findColour(Slider::trackColourId));
                g.fillRect(Rectangle<int>((static_cast<int> (thumbWidth) / 5), static_cast<int> (thumbWidth) / 2).withCentre(isThreeVal ? thumbPoint.roundToInt() : maxPoint.roundToInt()));
                //g.drawEllipse(Rectangle<float>(1.f, 1.f).withCentre(isThreeVal ? thumbPoint : maxPoint), 1.f);
            }

            if (isTwoVal || isThreeVal)
            {
                auto sr = jmin(trackWidth, (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
                auto pointerColour = slider.findColour(Slider::thumbColourId);

                if (slider.isHorizontal())
                {
                    drawPointer(g, minSliderPos - sr,
                        jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
                        trackWidth * 2.0f, pointerColour, 2);

                    drawPointer(g, maxSliderPos - trackWidth,
                        jmin((float)(y + height) - trackWidth * 2.0f, (float)y + (float)height * 0.5f),
                        trackWidth * 2.0f, pointerColour, 4);
                }
                else
                {
                    drawPointer(g, jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
                        minSliderPos - trackWidth,
                        trackWidth * 2.0f, pointerColour, 1);

                    drawPointer(g, jmin((float)(x + width) - trackWidth * 2.0f, (float)x + (float)width * 0.5f), maxSliderPos - sr,
                        trackWidth * 2.0f, pointerColour, 3);
                }
            }

            if (slider.isBar())
                drawLinearSliderOutline(g, x, y, width, height, style, slider);
        }
    }

    int getSliderThumbRadius(Slider& slider) override
    {
        return jmin(12, slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.5f)
            : static_cast<int> ((float)slider.getWidth() * 0.5f));
    }


    Slider::SliderLayout getSliderLayout(Slider& slider) override
    {
        // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

        int minXSpace = 0;
        int minYSpace = 0;

        auto textBoxPos = slider.getTextBoxPosition();

        if (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight)
            minXSpace = 30;
        else
            minYSpace = 15;

        auto localBounds = slider.getLocalBounds();

        auto textBoxWidth = jmax(0, jmin(slider.getTextBoxWidth(), localBounds.getWidth() - minXSpace));
        auto textBoxHeight = jmax(0, jmin(slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

        Slider::SliderLayout layout;

        // 2. set the textBox bounds

        if (textBoxPos != Slider::NoTextBox)
        {
            if (slider.isBar())
            {
                layout.textBoxBounds = localBounds;
            }
            else
            {
                layout.textBoxBounds.setWidth(textBoxWidth);
                layout.textBoxBounds.setHeight(textBoxHeight);

                if (textBoxPos == Slider::TextBoxLeft)           layout.textBoxBounds.setX(0);
                else if (textBoxPos == Slider::TextBoxRight)     layout.textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
                else /* above or below -> centre horizontally */ layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

                if (textBoxPos == Slider::TextBoxAbove)          layout.textBoxBounds.setY(0);
                else if (textBoxPos == Slider::TextBoxBelow)     layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight);
                else /* left or right -> centre vertically */    layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);
            }
        }

        // 3. set the slider bounds

        layout.sliderBounds = localBounds;

        if (slider.isBar())
        {
            layout.sliderBounds.reduce(1, 1);   // bar border
        }
        else
        {
            if (textBoxPos == Slider::TextBoxLeft)       layout.sliderBounds.removeFromLeft(textBoxWidth);
            else if (textBoxPos == Slider::TextBoxRight) layout.sliderBounds.removeFromRight(textBoxWidth);
            else if (textBoxPos == Slider::TextBoxAbove) layout.sliderBounds.removeFromTop(textBoxHeight);
            else if (textBoxPos == Slider::TextBoxBelow) layout.sliderBounds.removeFromBottom(textBoxHeight);

            const int thumbIndent = getSliderThumbRadius(slider);

            if (slider.isHorizontal())    layout.sliderBounds.reduce(thumbIndent, 0);
            else if (slider.isVertical()) layout.sliderBounds.reduce(0, thumbIndent);
        }

        return layout;
    }

    // Buttons
    void drawToggleButton(Graphics& g, ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto fontSize = jmin(15.0f, (float)button.getHeight() * 0.75f);
        auto tickWidth = fontSize * 1.1f;

        drawTickBox(g, button, 4.0f, ((float)button.getHeight() - tickWidth) * 0.5f,
            tickWidth, tickWidth,
            button.getToggleState(),
            button.isEnabled(),
            shouldDrawButtonAsHighlighted,
            shouldDrawButtonAsDown);

        g.setColour(button.findColour(ToggleButton::textColourId));
        g.setFont(fontSize);

        if (!button.isEnabled())
            g.setOpacity(0.5f);

        g.drawFittedText(button.getButtonText(),
            button.getLocalBounds().withTrimmedLeft(roundToInt(tickWidth) + 10)
            .withTrimmedRight(2),
            Justification::centredTop, 1);
    }

    void drawTickBox(Graphics& g, Component& component,
        float x, float y, float w, float h, const bool ticked,
        [[maybe_unused]] const bool isEnabled,
        [[maybe_unused]] const bool shouldDrawButtonAsHighlighted,
        [[maybe_unused]] const bool shouldDrawButtonAsDown) override
    {
        Rectangle<float> tickBounds(juce::roundFloatToInt(x), juce::roundFloatToInt(y), juce::roundFloatToInt(w), juce::roundFloatToInt(h));

        g.setColour(component.findColour(ToggleButton::tickDisabledColourId));
        g.drawRect(tickBounds, 1.0f);

        if (ticked)
        {
            g.setColour(component.findColour(ToggleButton::tickColourId));
            auto tick = getTickShape(0.75f);
            g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(4, 5).toFloat(), false));
            auto bgColor = component.findColour(ToggleButton::tickDisabledColourId);
            g.setColour(bgColor.withAlpha(.25f));
            g.fillRect(tickBounds.reduced(2.f));
        }
    }

    // Text or Text Editors
    void drawTextEditorOutline(Graphics& g, int width, int height, TextEditor& textEditor) override
    {
        if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) == nullptr)
        {
            if (textEditor.isEnabled())
            {
                if (textEditor.hasKeyboardFocus(true) && !textEditor.isReadOnly())
                {
                    g.setColour(textEditor.findColour(TextEditor::focusedOutlineColourId));
                    g.drawRect(0, 0, width, height, 1);
                }
                else
                {
                    g.setColour(textEditor.findColour(TextEditor::outlineColourId));
                    g.drawRect(0, 0, width, height);
                }
            }
        }
    }

    void drawBubble(Graphics& g, BubbleComponent& comp,
        const Point<float>& tip, const Rectangle<float>& body) override
    {
        Path p;

        p.addBubble(body.reduced(0.5f), body.getUnion(Rectangle<float>(tip.x, tip.y, 0.f, 0.f)),
            tip, 1.f, 0.f);

        g.setColour(comp.findColour(BubbleComponent::backgroundColourId));
        g.fillPath(p);

        g.setColour(comp.findColour(BubbleComponent::outlineColourId));
        g.strokePath(p, PathStrokeType(1.0f));
    }

    void drawButtonBackground(Graphics& g,
        Button& button,
        const Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 0.f;
        //auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        auto bounds = button.getLocalBounds().toFloat().reduced(1.f, 1.f);

        auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour(baseColour);

        auto flatOnLeft = button.isConnectedOnLeft();
        auto flatOnRight = button.isConnectedOnRight();
        auto flatOnTop = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            Path path;
            path.addRoundedRectangle(bounds.getX(), bounds.getY(),
                bounds.getWidth(), bounds.getHeight(),
                cornerSize, cornerSize,
                !(flatOnLeft || flatOnTop),
                !(flatOnRight || flatOnTop),
                !(flatOnLeft || flatOnBottom),
                !(flatOnRight || flatOnBottom));

            g.fillPath(path);

            g.setColour(button.findColour(ComboBox::outlineColourId));
            g.strokePath(path, PathStrokeType(1.0f));
        }
        else
        {
            g.fillRoundedRectangle(bounds, cornerSize);

            g.setColour(button.findColour(ComboBox::outlineColourId));
            g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        }
    }

    // Fonts
    static const Font& getNotoLightFont()
    {
        static Font notoLight(Font(Typeface::createSystemTypefaceFor(BinaryData::NotoSansLight_ttf,
            BinaryData::NotoSansLight_ttfSize)));
        return notoLight;
    }

    static const Font& getNotoMediumFont()
    {
        static Font notoMedium(Font(Typeface::createSystemTypefaceFor(BinaryData::NotoSansMedium_ttf,
            BinaryData::NotoSansMedium_ttfSize)));
        return notoMedium;
    }

    static const Font& getNotoRegularFont()
    {
        static Font notoRegular(Font(Typeface::createSystemTypefaceFor(BinaryData::NotoSansRegular_ttf,
            BinaryData::NotoSansRegular_ttfSize)));
        return notoRegular;
    }

    static const Font& getNotoSemiBoldFont()
    {
        static Font notoSemiBold(Font(Typeface::createSystemTypefaceFor(BinaryData::NotoSansSemiBold_ttf,
            BinaryData::NotoSansSemiBold_ttfSize)));
        return notoSemiBold;
    }

    static const Font& getNotoThinFont()
    {
        static Font notoThin(Font(Typeface::createSystemTypefaceFor(BinaryData::NotoSansThin_ttf,
            BinaryData::NotoSansThin_ttfSize)));
        return notoThin;
    }

    Typeface::Ptr getTypefaceForFont(const Font& f) override
    {
        static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::NotoSansRegular_ttf, BinaryData::NotoSansRegular_ttfSize);
        return myFont;
    }
};