/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <../Source/Gate.h>
using namespace juce;
using namespace dsp;

//==============================================================================
/**
*/
class CoveSplitGateAudioProcessor  : public juce::AudioProcessor
{
public:
    enum Band { // to define which band of audio is being referenced
        low,
        high
    };

    enum Channel { // to define which channel of audio is being referenced
        left,
        right
    };


    //==============================================================================
    CoveSplitGateAudioProcessor();
    ~CoveSplitGateAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getRmsValue(Channel channel, Band band) const;
    AudioProcessorValueTreeState& getVts();

private:
    ProcessSpec spec;
    AudioProcessorValueTreeState vts;

    std::atomic<float>* crossoverParam = nullptr;
    std::atomic<float>* lowRatio = nullptr;
    std::atomic<float>* lowBypass = nullptr;
    std::atomic<float>* lowAttack = nullptr;
    std::atomic<float>* lowHold = nullptr;
    std::atomic<float>* lowRelease = nullptr;
    
    std::atomic<float>* lowThreshold = nullptr;
    std::atomic<float>* highBypass = nullptr;
    std::atomic<float>* highAttack = nullptr;
    std::atomic<float>* highRelease = nullptr;
    std::atomic<float>* highRatio = nullptr;
    std::atomic<float>* highHold = nullptr;
    std::atomic<float>* highThreshold = nullptr;

    LinkwitzRileyFilter<float> lp, hp;
    //NoiseGate<float> lowGate, highGate;
    Gate lowGate, highGate;
    std::array<AudioBuffer<float>, 2> filterBuffers;
    juce::AudioBuffer<float> lowGateBuffer;
    juce::AudioBuffer<float> highGateBuffer;

    LinearSmoothedValue<float> lowBandRMSLeft, lowBandRMSRight, highBandRMSLeft, highBandRMSRight;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoveSplitGateAudioProcessor)
};
