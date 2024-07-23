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
    CoveLookAndFeel() {};

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
            g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

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
            g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

            if (!isTwoVal) // draw thumb for horizontal / threeVal slider
            {
                g.setColour(slider.findColour(Slider::thumbColourId));
                //g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint)); // old thumb
                g.fillRect(Rectangle<int>(static_cast<int> (thumbWidth) / 2, static_cast<int> (thumbWidth)).withCentre(isThreeVal ? thumbPoint.roundToInt() : maxPoint.roundToInt()));
                g.setColour(slider.findColour(Slider::backgroundColourId));
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

};