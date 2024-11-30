/*
  ==============================================================================

    TransparentVisualizer.h
    Created: 25 Nov 2024 3:09:31pm
    Author:  tjbac

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace juce;

//==============================================================================
/*
*/
#pragma once

#include <JuceHeader.h>
using namespace juce;

class TransparentVisualizer : public juce::AudioVisualiserComponent
{
public:
    TransparentVisualizer()
        : AudioVisualiserComponent(2),
        backgroundColour(Colours::red),
        waveformColour(Colours::darkgrey)
    {
        setBufferSize(128); // Example buffer size
        for (int i = 0; i < 2; ++i)
            channels.add(new ChannelInfo(*this, 128));
        this->setOpaque(false);
    }

    void paint(juce::Graphics& g) override
    {
        //g.fillAll(juce::Colour(0.f, 0.f, 0.f, 0.f));

        auto r = getLocalBounds().toFloat();
        auto channelHeight = r.getHeight() / static_cast<float>(channels.size());

        g.setColour(waveformColour);

        for (auto* c : channels)
        {
            paintChannel(g, r.removeFromTop(channelHeight),
                c->levels.begin(), c->levels.size(), c->nextSample);
        }
    }

    void paintChannel(Graphics& g, Rectangle<float> area,
        const Range<float>* levels, int numLevels, int nextSample) override
    {
        Path p;
        getChannelAsPath(p, levels, numLevels, nextSample);

        g.fillPath(p, AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(), 0.0f, 1.0f, area.getX(), area.getBottom(), (float)numLevels, -1.0f, area.getRight(), area.getY()));
        //AffineTransform::fromTargetPoints(0.0f, -1.0f, area.getX(), area.getY(), 0.0f, 1.0f, area.getX(), area.getBottom(), (float)numLevels, -1.0f, area.getRight(), area.getY())
    }

    void setBufferSize(int newSize)
    {
        for (auto* c : channels)
            c->setBufferSize(newSize);
    }

    void clear()
    {
        for (auto* c : channels)
            c->clear();
    }

    void pushSamples(const float** inputSamples, int numSamples)
    {
        for (int channel = 0; channel < channels.size(); ++channel)
        {
            channels[channel]->pushSamples(inputSamples[channel], numSamples);
        }
    }

    void pushBuffer(const float* const* d, int numChannels, int num)
    {
        numChannels = jmin(numChannels, channels.size());

        for (int i = 0; i < numChannels; ++i)
            channels.getUnchecked(i)->pushSamples(d[i], num);
    }

    void pushBuffer(const AudioBuffer<float>& buffer)
    {
        pushBuffer(buffer.getArrayOfReadPointers(),
            buffer.getNumChannels(),
            buffer.getNumSamples());
    }

    void pushBuffer(const AudioSourceChannelInfo& buffer)
    {
        auto numChannels = jmin(buffer.buffer->getNumChannels(), channels.size());

        for (int i = 0; i < numChannels; ++i)
            channels.getUnchecked(i)->pushSamples(buffer.buffer->getReadPointer(i, buffer.startSample),
                buffer.numSamples);
    }

    void getChannelAsPath(Path& path, const Range<float>* levels,
        int numLevels, int nextSample)
    {
        path.preallocateSpace(4 * numLevels + 8);

        for (int i = 0; i < numLevels; ++i)
        {
            auto level = -(levels[(nextSample + i) % numLevels].getEnd());

            if (i == 0)
                path.startNewSubPath(0.0f, level);
            else
                path.lineTo((float)i, level);
        }

        for (int i = numLevels; --i >= 0;)
            path.lineTo((float)i, -(levels[(nextSample + i) % numLevels].getStart()));

        path.closeSubPath();
    }

private:
    struct ChannelInfo
    {
        ChannelInfo(TransparentVisualizer& o, int bufferSize)
            : owner(o)
        {
            setBufferSize(bufferSize);
            clear();
        }

        void clear() noexcept
        {
            levels.fill({});
            value = {};
            subSample = 0;
        }

        void pushSamples(const float* inputSamples, int num) noexcept
        {
            for (int i = 0; i < num; ++i)
                pushSample(inputSamples[i]);
        }

        void pushSample(float newSample) noexcept
        {
            if (--subSample <= 0)
            {
                if (++nextSample == levels.size())
                    nextSample = 0;

                levels.getReference(nextSample) = value;
                subSample = owner.getSamplesPerBlock();
                value = Range<float>(newSample, newSample);
            }
            else
            {
                value = value.getUnionWith(newSample);
            }
        }

        void setBufferSize(int newSize)
        {
            levels.removeRange(newSize, levels.size());
            levels.insertMultiple(-1, {}, newSize - levels.size());

            if (nextSample >= newSize)
                nextSample = 0;
        }

        TransparentVisualizer& owner;
        Array<Range<float>> levels;
        Range<float> value;
        std::atomic<int> nextSample{ 0 }, subSample{ 0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelInfo)
    };

    OwnedArray<ChannelInfo> channels;
    Colour backgroundColour, waveformColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransparentVisualizer)
};
