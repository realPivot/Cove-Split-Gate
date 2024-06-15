/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "math.h"


float calcLogSkew(float min, float max) // skew factor algorithm from jucestepbystep.wordpress.com/logarithmic-sliders/
{
    float mid = sqrtf(min * max);
    float output = (log10(.5) / log10((mid - min) / (max - min)));
    return output;
}

AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    auto attackMsMin = .1f;
    auto attackMsMax = 200.f;
    auto releaseMsMin = 1.f;
    auto releaseMsMax = 500.f;
    auto ratioLow = 1.0f;
    auto ratioHigh = 30.0f;
    auto holdLow = 1.f;
    auto holdHigh = 50.f;

    std::vector < std::unique_ptr<RangedAudioParameter>> params;

    auto frequencyAttribute = AudioParameterFloatAttributes().withLabel("hz");
    auto thresholdAttribute = AudioParameterFloatAttributes().withStringFromValueFunction([](auto x, auto) { return Decibels::toString(Decibels::gainToDecibels(x), 2, -100.f, false); }).withLabel("db");
    auto bypassAttribute = AudioParameterBoolAttributes().withStringFromValueFunction([](auto x, auto) { return x ? "On" : "Off"; });

    params.push_back(std::make_unique<AudioParameterFloat>("crossover", "Crossover Frequency", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, calcLogSkew(20.f, 20000.f)), 1000.f, frequencyAttribute));
    params.push_back(std::make_unique<AudioParameterBool>("lowBypass", "Low Bypass", 0, bypassAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowRatio", "Low Ratio", juce::NormalisableRange<float>(ratioLow, ratioHigh, 0.5f, calcLogSkew(1.0f, ratioHigh / 2)), 4.0f, " / 1"));
    params.push_back(std::make_unique<AudioParameterFloat>("lowAttack", "Low Attack", juce::NormalisableRange<float>(attackMsMin, attackMsMax, .1f, calcLogSkew(attackMsMin, attackMsMax / 2)), 5.f, "ms")); // divide attackMsMax by 2 to make skew less aggressive
    params.push_back(std::make_unique<AudioParameterFloat>("lowRelease", "Low Release", juce::NormalisableRange<float>(releaseMsMin, releaseMsMax, 1.f, calcLogSkew(releaseMsMin, releaseMsMax)), 100.f, "ms"));
    params.push_back(std::make_unique<AudioParameterFloat>("lowHold", "Low Hold", juce::NormalisableRange<float>(holdLow, holdHigh, 1.f)));
    params.push_back(std::make_unique<AudioParameterFloat>("lowThreshold", "Low Threshold", juce::NormalisableRange<float>(0.f, Decibels::decibelsToGain(12.0f), .000001f , calcLogSkew(0.1f, 112.1f)), 1.f, thresholdAttribute));

    params.push_back(std::make_unique<AudioParameterBool>("highBypass", "High Bypass", 0, bypassAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("highRatio", "High Ratio", juce::NormalisableRange<float>(ratioLow, ratioHigh, 0.5f, calcLogSkew(1.0f, ratioHigh / 2)), 4.0f, " / 1"));
    params.push_back(std::make_unique<AudioParameterFloat>("highAttack", "High Attack", juce::NormalisableRange<float>(attackMsMin, attackMsMax, .1f, calcLogSkew(attackMsMin, attackMsMax / 2)), 5.f, "ms")); // divide attackMsMax by 2 to make skew less aggressive
    params.push_back(std::make_unique<AudioParameterFloat>("highRelease", "High Release", juce::NormalisableRange<float>(releaseMsMin, releaseMsMax, 1.f, calcLogSkew(releaseMsMin, releaseMsMax)), 100.f, "ms"));
    params.push_back(std::make_unique<AudioParameterFloat>("highHold", "High Hold", juce::NormalisableRange<float>(holdLow, holdHigh, 1.f)));
    params.push_back(std::make_unique<AudioParameterFloat>("highThreshold", "High Threshold", juce::NormalisableRange<float>(0.f, Decibels::decibelsToGain(12.0f), .000001f, calcLogSkew(0.1f, 112.1f)), 1.f, thresholdAttribute));
    return { params.begin(), params.end() };
}

//==============================================================================
CoveSplitGateAudioProcessor::CoveSplitGateAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    , vts(*this, nullptr, "PARAMS", createParameterLayout())
{
    crossoverParam = vts.getRawParameterValue("crossover");
    lowRatio = vts.getRawParameterValue("lowRatio");
    lowBypass = vts.getRawParameterValue("lowBypass");
    lowAttack = vts.getRawParameterValue("lowAttack");
    lowRelease = vts.getRawParameterValue("lowRelease");
    lowHold = vts.getRawParameterValue("lowHold");
    lowThreshold = vts.getRawParameterValue("lowThreshold");
    highBypass = vts.getRawParameterValue("highBypass");
    highRatio = vts.getRawParameterValue("highRatio");
    highAttack = vts.getRawParameterValue("highAttack");
    highRelease = vts.getRawParameterValue("highRelease");
    highHold = vts.getRawParameterValue("highHold");
    highThreshold = vts.getRawParameterValue("highThreshold");

    lp.setType(dsp::LinkwitzRileyFilterType::lowpass);
    hp.setType(dsp::LinkwitzRileyFilterType::highpass);
}

CoveSplitGateAudioProcessor::~CoveSplitGateAudioProcessor()
{
}

//==============================================================================
const juce::String CoveSplitGateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CoveSplitGateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CoveSplitGateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CoveSplitGateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CoveSplitGateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CoveSplitGateAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CoveSplitGateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CoveSplitGateAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CoveSplitGateAudioProcessor::getProgramName (int index)
{
    return {};
}

void CoveSplitGateAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CoveSplitGateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    

    spec.sampleRate = sampleRate;
    spec.numChannels = getNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;

    lp.prepare(spec);
    hp.prepare(spec);

    lowGate.prepare(spec.sampleRate, spec.maximumBlockSize);
    highGate.prepare(spec.sampleRate, spec.maximumBlockSize);

    for (auto& buffer : filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
        buffer.clear();
    }
}

void CoveSplitGateAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CoveSplitGateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void CoveSplitGateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    lp.setCutoffFrequency(*crossoverParam);
    hp.setCutoffFrequency(*crossoverParam);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (auto& fb : filterBuffers) // Copy input buffer to all filter buffers
    { fb = buffer; }

    // Blocks
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);

    // Context
    auto fb0Context = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Context = juce::dsp::ProcessContextReplacing<float>(fb1Block);

    // Set Gate Params
    //auto ratio = 2;
    lowGate.setAttack(lowAttack);
    lowGate.setRelease(lowRelease);
    lowGate.setThreshold(lowThreshold);
    lowGate.setRatio(lowRatio);
    lowGate.setHold(lowHold);
    highGate.setAttack(highAttack);
    highGate.setRelease(highRelease);
    highGate.setThreshold(highThreshold);
    highGate.setRatio(highRatio);
    highGate.setHold(highHold);


    lp.process(fb0Context);
    hp.process(fb1Context);
    if (*lowBypass < 0.5f) { lowGate.process(fb0Context); }
    if (*highBypass < 0.5f) { highGate.process(fb1Context); }

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    buffer.clear();

    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
        {
            for (auto i = 0; i < nc; i++)
            {
                inputBuffer.addFrom(i, 0, source, i, 0, ns);
            }
        };

    addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);


}

//==============================================================================
bool CoveSplitGateAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CoveSplitGateAudioProcessor::createEditor()
{
    //return new CoveSplitGateAudioProcessorEditor (*this);
    return new GenericAudioProcessorEditor(*this); // return generic editor
}

//==============================================================================
void CoveSplitGateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = vts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CoveSplitGateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(vts.state.getType()))
            vts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CoveSplitGateAudioProcessor();
}
