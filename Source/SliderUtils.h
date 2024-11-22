/*
  ==============================================================================

    SliderUtils.h
    Created: 22 Nov 2024 9:05:57am
    Author:  tjbac

  ==============================================================================
*/
// SliderUtils.h
#pragma once
#include <JuceHeader.h>

template <typename ValueT>
juce::NormalisableRange<ValueT> logRange(ValueT min, ValueT max)
{
    ValueT rng{ std::log(max / min) };
    return { min, max,
        [=](ValueT min, ValueT, ValueT v) { return std::exp(v * rng) * min; },
        [=](ValueT min, ValueT, ValueT v) { return std::log(v / min) / rng; }
    };
}