/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "math.h"
#include "SliderUtils.h"


float calcLogSkew(float min, float max) // skew factor algorithm from jucestepbystep.wordpress.com/logarithmic-sliders/
{
    float mid = sqrtf(min * max);
    float output = (log10(.5) / log10((mid - min) / (max - min)));
    return output;
}

AudioProcessorValueTreeState::ParameterLayout createParameterLayout() { // Default parameter layout
    auto attackMsMin = .3f;
    auto attackMsMax = 200.f;
    auto releaseMsMin = 1.f;
    auto releaseMsMax = 500.f;
    auto ratioLow = 1.0f;
    auto ratioHigh = 30.0f;
    auto holdLow = 0.5f;
    auto holdHigh = 50.f;

    std::vector < std::unique_ptr<RangedAudioParameter>> params;

    juce::AudioParameterFloatAttributes frequencyAttribute = AudioParameterFloatAttributes().withStringFromValueFunction([](auto x, auto) {
        return juce::String(double(x), 2, false);
        });

    auto timeAttribute = AudioParameterFloatAttributes().withStringFromValueFunction([](auto x, auto) { 
        if (x < 10) {
            return juce::String(x, 2) + juce::String(" ms");
        }
        else if (x < 100 && x >= 10) {
            return juce::String(x, 1) + juce::String(" ms");
        }
        else {
            return juce::String((double)juce::roundToInt(x), 0, false) + juce::String(" ms");
        }
        });

    auto thresholdAttribute = AudioParameterFloatAttributes().withStringFromValueFunction([](auto x, auto) { 
        auto thresholdString = Decibels::toString(Decibels::gainToDecibels(x), 2, -80.f, false);
        return thresholdString + " dB"; });

    auto bypassAttribute = AudioParameterBoolAttributes().withStringFromValueFunction([](auto x, auto) { return x ? "On" : "Off"; });

    auto ratioAttribute = AudioParameterFloatAttributes().withStringFromValueFunction([](auto x, auto) {
        return juce::String(double(x), 2, false);
        });

    auto muteAttribute = AudioParameterBoolAttributes().withStringFromValueFunction([](auto x, auto) { return x ? "Mute" : "Disabled"; });


    /*
    params.push_back(std::make_unique<AudioParameterFloat>("crossover", "Crossover Frequency", juce::NormalisableRange<float>(20.f, 20000.f, .01f, calcLogSkew(50.f, 20000.f)), 1000.f, frequencyAttribute));
    params.push_back(std::make_unique<AudioParameterBool>("lowBypass", "Low Enable", 0, bypassAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowRatio", "Low Ratio", juce::NormalisableRange<float>(ratioLow, ratioHigh, 0.1f, calcLogSkew(ratioLow, ratioHigh / 4)), 4.0f, " / 1"));
    params.push_back(std::make_unique<AudioParameterFloat>("lowAttack", "Low Attack", juce::NormalisableRange<float>(attackMsMin, attackMsMax, .001f, calcLogSkew(attackMsMin, attackMsMax / 2)), 5.f, timeAttribute)); // divide attackMsMax by 2 to make skew less aggressive
    params.push_back(std::make_unique<AudioParameterFloat>("lowRelease", "Low Release", juce::NormalisableRange<float>(releaseMsMin, releaseMsMax, .001f, calcLogSkew(releaseMsMin, releaseMsMax / 2)), 100.f, timeAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowHold", "Low Hold", juce::NormalisableRange<float>(holdLow, holdHigh, .001f, calcLogSkew(holdLow + 1.f, holdHigh / 2)), 1.f, timeAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowThreshold", "Low Threshold", juce::NormalisableRange<float>(Decibels::decibelsToGain(-80.f), Decibels::decibelsToGain(12.0f), .000001f, calcLogSkew(0.1f, 112.1f)), 1.f, thresholdAttribute));
    params.push_back(std::make_unique<AudioParameterBool>("lowMute", "Low Mute", 0, muteAttribute));
    */

    params.push_back(std::make_unique<AudioParameterFloat>("crossover", "Crossover Frequency", logRange(20.f, 20000.f), 633.f, frequencyAttribute));
    params.push_back(std::make_unique<AudioParameterBool>("lowBypass", "Low Enable", 0, bypassAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowRatio", "Low Ratio", logRange(ratioLow, ratioHigh), 4.0f, ratioAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowAttack", "Low Attack", logRange(attackMsMin, attackMsMax), 5.f, timeAttribute)); // divide attackMsMax by 2 to make skew less aggressive
    params.push_back(std::make_unique<AudioParameterFloat>("lowRelease", "Low Release", logRange(releaseMsMin, releaseMsMax), 100.f, timeAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowHold", "Low Hold", logRange(holdLow, holdHigh), 1.f, timeAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("lowThreshold", "Low Threshold", logRange(Decibels::decibelsToGain(-80.f), Decibels::decibelsToGain(12.0f)), 1.f, thresholdAttribute));
    params.push_back(std::make_unique<AudioParameterBool>("lowMute", "Low Mute", 0, muteAttribute));

    params.push_back(std::make_unique<AudioParameterBool>("highBypass", "High Enable", 0, bypassAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("highRatio", "High Ratio", logRange(ratioLow, ratioHigh), 4.0f, ratioAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("highAttack", "High Attack", logRange(attackMsMin, attackMsMax), 5.f, timeAttribute)); // divide attackMsMax by 2 to make skew less aggressive
    params.push_back(std::make_unique<AudioParameterFloat>("highRelease", "High Release", logRange(releaseMsMin, releaseMsMax), 100.f, timeAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("highHold", "High Hold", logRange(holdLow, holdHigh), 1.f, timeAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("highThreshold", "High Threshold", logRange(Decibels::decibelsToGain(-80.f), Decibels::decibelsToGain(12.0f)), 1.f, thresholdAttribute));
    params.push_back(std::make_unique<AudioParameterBool>("highMute", "High Mute", 0, muteAttribute));
    return { params.begin(), params.end() };
}

AudioProcessorValueTreeState::ParameterLayout createHiddenParameterLayout() { // Layout for non-automatable parameters
    std::vector < std::unique_ptr<RangedAudioParameter>> params;
    auto bypassAttribute = AudioParameterBoolAttributes().withStringFromValueFunction([](auto x, auto) { return x ? "On" : "Off"; });

    params.push_back(std::make_unique<AudioParameterFloat>("waveformGain", "WaveformGain", Decibels::decibelsToGain(-25.0), Decibels::decibelsToGain(50.0), Decibels::decibelsToGain(25.0)));
    params.push_back(std::make_unique<AudioParameterBool>("waveformStereo", "WaveformStereo", false, bypassAttribute));
    params.push_back(std::make_unique<AudioParameterFloat>("waveformSpeed", "WaveformSpeed", logRange(0.1f, 4.0f), 1.0f));

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
    , dummyProcessor(), vts(*this, nullptr, "PARAMS", createParameterLayout()), 
    hiddenVts(dummyProcessor, nullptr, "HIDDENPARAMS", createHiddenParameterLayout())
    
{
    // set global parameters with value from vts
    crossoverParam = vts.getRawParameterValue("crossover");
    lowRatio = vts.getRawParameterValue("lowRatio");
    lowBypass = vts.getRawParameterValue("lowBypass");
    lowAttack = vts.getRawParameterValue("lowAttack");
    lowRelease = vts.getRawParameterValue("lowRelease");
    lowHold = vts.getRawParameterValue("lowHold");
    lowThreshold = vts.getRawParameterValue("lowThreshold");
    lowMute = vts.getRawParameterValue("lowMute");
    highBypass = vts.getRawParameterValue("highBypass");
    highRatio = vts.getRawParameterValue("highRatio");
    highAttack = vts.getRawParameterValue("highAttack");
    highRelease = vts.getRawParameterValue("highRelease");
    highHold = vts.getRawParameterValue("highHold");
    highThreshold = vts.getRawParameterValue("highThreshold");
    highMute = vts.getRawParameterValue("highMute");

    waveformGain = hiddenVts.getRawParameterValue("waveformGain");

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

    lowGateBuffer.setSize(spec.numChannels, samplesPerBlock);
    highGateBuffer.setSize(spec.numChannels, samplesPerBlock);
    lowGateBuffer.clear();
    highGateBuffer.clear();
    
    lowBandRMSLeft.setCurrentAndTargetValue(-100.f);
    lowBandRMSRight.setCurrentAndTargetValue(-100.f);
    highBandRMSLeft.setCurrentAndTargetValue(-100.f);
    highBandRMSRight.setCurrentAndTargetValue(-100.f);

    
}

void CoveSplitGateAudioProcessor::releaseResources()
{
    lowGateBuffer.clear();
    highGateBuffer.clear();
    for (int i = 0; i < filterBuffers.size(); i++) {
        filterBuffers[i].clear();
    }

}

AudioProcessorValueTreeState& CoveSplitGateAudioProcessor::getVts(bool shouldReturnHiddenVTS) {
    if (!shouldReturnHiddenVTS)
        return vts;

    if (shouldReturnHiddenVTS)
        return hiddenVts;
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

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

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

    // Contexts
    auto fb0Context = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Context = juce::dsp::ProcessContextReplacing<float>(fb1Block);

    // Set Gate Params
    lowGate.setParameters(vts, false, "lowThreshold", "lowRatio", "lowAttack", "lowRelease", "lowHold", "crossover", true);
    highGate.setParameters(vts, false, "highThreshold", "highRatio", "highAttack", "highRelease", "highHold", "crossover", false);

    // Process Filters
    lp.process(fb0Context);
    hp.process(fb1Context);

    fb0Context.getInputBlock().copyTo(lowGateBuffer);
    fb1Context.getInputBlock().copyTo(highGateBuffer);

    //Metering
    {
        auto sampleScaler = 0.1f; // amount of samples to skip for the rms value calculation. 0.5 == 50% of the sampleRate (or .5 seconds)
        auto leniancyScalePercent = 1.3f; // value above current value to ignore updating current value. Ex: 1.25 = 125% of current value. Any value above 125%
        // will be updated instantly, any value below will be smoothed to.

        lowBandRMSLeft.skip(spec.sampleRate * sampleScaler);
        lowBandRMSRight.skip(spec.sampleRate * sampleScaler);
        highBandRMSLeft.skip(spec.sampleRate * sampleScaler);
        highBandRMSRight.skip(spec.sampleRate * sampleScaler);
        // Low Band
        { 
            // Smooth input low L meter
            {
                const auto value = Decibels::gainToDecibels(lowGateBuffer.getRMSLevel(0, 0, numSamples));
                if (value <= (lowBandRMSLeft.getCurrentValue() * leniancyScalePercent))
                    lowBandRMSLeft.setTargetValue(value);
                else
                    lowBandRMSLeft.setCurrentAndTargetValue(value);
            }
            // Smooth input low R meter
            {
                const auto value = Decibels::gainToDecibels(lowGateBuffer.getRMSLevel(1, 0, numSamples));
                if (value <= (lowBandRMSRight.getCurrentValue() * leniancyScalePercent))
                    lowBandRMSRight.setTargetValue(value);
                else
                    lowBandRMSRight.setCurrentAndTargetValue(value);
            }
        }
        // High Band
        { 
            // Smooth input high L meter
            {
                const auto value = Decibels::gainToDecibels(highGateBuffer.getRMSLevel(0, 0, numSamples));
                if (value < (highBandRMSLeft.getCurrentValue() * leniancyScalePercent))
                    highBandRMSLeft.setTargetValue(value);
                else
                    highBandRMSLeft.setCurrentAndTargetValue(value);
            }
            // Smooth input high R meter
            {
                const auto value = Decibels::gainToDecibels(highGateBuffer.getRMSLevel(1, 0, numSamples));
                if (value < (highBandRMSRight.getCurrentValue() * leniancyScalePercent))
                    highBandRMSRight.setTargetValue(value);
                else
                    highBandRMSRight.setCurrentAndTargetValue(value);
            }
        }
    }

    // Waveform
    juce::AudioBuffer<float> tempLowGateBuffer(lowGateBuffer);
    juce::AudioBuffer<float> tempHighGateBuffer(highGateBuffer);

    if (waveformGain != nullptr) {
        tempLowGateBuffer.applyGain(waveformGain->load());
        tempHighGateBuffer.applyGain(waveformGain->load());
    }

    pushBufferToVisualizer(tempLowGateBuffer, Band::low, PreOrPost::preGate);
    pushBufferToVisualizer(tempHighGateBuffer, Band::high, PreOrPost::preGate);

    // Process Expanders
    if (*lowBypass > 0.5f) { lowGate.process(lowGateBuffer); }
    if (*highBypass > 0.5f) { highGate.process(highGateBuffer); }

    fb0Context.getOutputBlock().copyFrom(lowGateBuffer);
    fb1Context.getOutputBlock().copyFrom(highGateBuffer);

    //tempLowGateBuffer.clear();

    /*
    for (int i = 0; i < tempLowGateBuffer.getNumChannels(); i++) {
        tempLowGateBuffer.copyFrom(i, 0, lowGateBuffer, i, 0, lowGateBuffer.getNumSamples());
        tempHighGateBuffer.copyFrom(i, 0, highGateBuffer, i, 0, highGateBuffer.getNumSamples());
    }

    if (waveformGain != nullptr) {
        tempLowGateBuffer.applyGain(waveformGain->load());
        tempHighGateBuffer.applyGain(waveformGain->load());
    }

    pushBufferToVisualizer(tempLowGateBuffer, Band::low, PreOrPost::postGate);
    pushBufferToVisualizer(tempHighGateBuffer, Band::high, PreOrPost::postGate);
    */
    buffer.clear();

    // sum filters back together
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
        {
            for (auto i = 0; i < nc; i++)
            {
                inputBuffer.addFrom(i, 0, source, i, 0, ns);
            }
        };

    
    if (*lowMute < 0.5f) {
        addFilterBand(buffer, filterBuffers[0]); // add low band to output buffer if mute is not enabled
        if (waveformGain != nullptr) filterBuffers[0].applyGain(waveformGain->load());
        pushBufferToVisualizer(filterBuffers[0], Band::low, PreOrPost::postGate);
    }
    else
    {
        filterBuffers[0].applyGain(Decibels::decibelsToGain(-300.0f));
        pushBufferToVisualizer(filterBuffers[0], Band::low, PreOrPost::postGate);
    }
    if (*highMute < 0.5f) {
        addFilterBand(buffer, filterBuffers[1]); // add high band to output buffer if mute is not enabled
        if (waveformGain != nullptr) filterBuffers[1].applyGain(waveformGain->load());
        pushBufferToVisualizer(filterBuffers[1], Band::high, PreOrPost::postGate);
    }
    else {
        filterBuffers[1].applyGain(Decibels::decibelsToGain(-300.0f));
        pushBufferToVisualizer(filterBuffers[1], Band::high, PreOrPost::postGate);
    }
    
}

void CoveSplitGateAudioProcessor::pushBufferToVisualizer(const juce::AudioBuffer<float>& buffer, Band band, PreOrPost when) {
    if (auto* editor = dynamic_cast<CoveSplitGateAudioProcessorEditor*>(this->getActiveEditor())) // if editor exists
    {
        if (when == PreOrPost::preGate) {
            switch (band) { // check which band we are pushing buffer to
            case Band::low:
                editor->updateVisualizer(buffer, CoveSplitGateAudioProcessorEditor::GateBand::LowBand, CoveSplitGateAudioProcessorEditor::preGate); // push relevant buffer
                break;
            case Band::high:
                editor->updateVisualizer(buffer, CoveSplitGateAudioProcessorEditor::GateBand::HighBand, CoveSplitGateAudioProcessorEditor::preGate); // push relevant buffer
                break;
            }
        }
        else if (when == PreOrPost::postGate)
        {
            switch (band) { // check which band we are pushing buffer to
            case Band::low:
                editor->updateVisualizer(buffer, CoveSplitGateAudioProcessorEditor::GateBand::LowBand, CoveSplitGateAudioProcessorEditor::postGate); // push relevant buffer
                break;
            case Band::high:
                editor->updateVisualizer(buffer, CoveSplitGateAudioProcessorEditor::GateBand::HighBand, CoveSplitGateAudioProcessorEditor::postGate); // push relevant buffer
                break;
            }
        }

    }
}

float CoveSplitGateAudioProcessor::getRmsValue(Channel channel, Band band) const
{
    switch (band) {
    case CoveSplitGateAudioProcessor::Band::low:
        if (channel == Channel::left)
            return lowBandRMSLeft.getCurrentValue();
        if (channel == Channel::right)
            return lowBandRMSRight.getCurrentValue();
        break;
    case CoveSplitGateAudioProcessor::Band::high:
        if (channel == Channel::left)
            return highBandRMSLeft.getCurrentValue();
        if (channel == Channel::right)
            return highBandRMSRight.getCurrentValue();
        break;
    }
}
    
    

//==============================================================================
bool CoveSplitGateAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CoveSplitGateAudioProcessor::createEditor()
{
    return new CoveSplitGateAudioProcessorEditor (*this);
    //return new GenericAudioProcessorEditor(*this); // return generic editor
}

//==============================================================================
void CoveSplitGateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto xml{ std::make_unique<juce::XmlElement>("SPLITGATE")};

    std::unique_ptr<juce::XmlElement> mainStateXML(vts.copyState().createXml());
    xml->addChildElement(mainStateXML.release()); // added to <STATE> tag

    std::unique_ptr<juce::XmlElement> hiddenStateXML(hiddenVts.copyState().createXml()); // added to <UA_STATE> tag
    xml->addChildElement(hiddenStateXML.release());

    copyXmlToBinary(*xml, destData);
}


void CoveSplitGateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml{ getXmlFromBinary(data, sizeInBytes) };

    if (xml.get() != nullptr && xml->hasTagName("SPLITGATE")) {
        if (auto* mainStateXML = xml->getChildByName(vts.state.getType())) {
            vts.replaceState(juce::ValueTree::fromXml(*mainStateXML));
        }
        if (auto* hiddenStateXML = xml->getChildByName(hiddenVts.state.getType())) {
            hiddenVts.replaceState(juce::ValueTree::fromXml(*hiddenStateXML));
        }
    }

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CoveSplitGateAudioProcessor();
}
