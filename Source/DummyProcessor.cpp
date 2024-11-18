/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "DummyProcessor.h"


//==============================================================================
DummyProcessorAudioProcessor::DummyProcessorAudioProcessor()
{
}

DummyProcessorAudioProcessor::~DummyProcessorAudioProcessor()
{
}

//==============================================================================
const juce::String DummyProcessorAudioProcessor::getName() const
{
    return juce::String("DummyProcessor");
}

bool DummyProcessorAudioProcessor::acceptsMidi() const
{
    return false;
}

bool DummyProcessorAudioProcessor::producesMidi() const
{
    return false;
}

bool DummyProcessorAudioProcessor::isMidiEffect() const
{
    return false;
}

double DummyProcessorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DummyProcessorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DummyProcessorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DummyProcessorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DummyProcessorAudioProcessor::getProgramName (int index)
{
    return {};
}

void DummyProcessorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DummyProcessorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void DummyProcessorAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DummyProcessorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DummyProcessorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
}

//==============================================================================
bool DummyProcessorAudioProcessor::hasEditor() const
{
    return false; 
}

juce::AudioProcessorEditor* DummyProcessorAudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void DummyProcessorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DummyProcessorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..

//juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
//{
//    return new DummyProcessorAudioProcessor();
//}
