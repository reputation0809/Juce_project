/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

//==============================================================================
Compressor_pAudioProcessor::Compressor_pAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
state(*this,nullptr,juce::Identifier("parames"),{
    std::make_unique<juce::AudioParameterFloat>(
                                                "threshold",
                                                "Threshold",
                                                juce::NormalisableRange<float>(-60.0f,20.0f,0.01f),10.0f),
    std::make_unique<juce::AudioParameterFloat>(
                                                "ratio",
                                                "Ratio",
                                                juce::NormalisableRange<float>(1.0f,20.0f,0.01f),2.0f),
    std::make_unique<juce::AudioParameterFloat>(
                                                "knee",
                                                "KneeWidth",
                                                juce::NormalisableRange<float>(0.0f,24.0f,0.01f),0.0f),
    std::make_unique<juce::AudioParameterFloat>(
                                                "attack",
                                                "Attack",
                                                juce::NormalisableRange<float>(0.01f,500.0f,0.01f),100.0f),
    std::make_unique<juce::AudioParameterFloat>(
                                                "release",
                                                "Release",
                                                juce::NormalisableRange<float>(0.01f,2000.0f,0.01f),500.0f),
    std::make_unique<juce::AudioParameterFloat>(
                                                "make_up",
                                                "Make up",
                                                juce::NormalisableRange<float>(-10.0f,24.0f,0.01f),0.0f)
    
}),mode_state(*this, nullptr, "PARAMETERS", initializeGUI())
#endif
{
}

Compressor_pAudioProcessor::~Compressor_pAudioProcessor()
{
    
}

juce::AudioProcessorValueTreeState::ParameterLayout Compressor_pAudioProcessor::initializeGUI(){
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>("modeID", "modeName", juce::StringArray("Compressor","Expander","Limiter"), 0));
    
    return {params.begin(),params.end()};
}

//==============================================================================
const juce::String Compressor_pAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Compressor_pAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Compressor_pAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Compressor_pAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Compressor_pAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Compressor_pAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Compressor_pAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Compressor_pAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Compressor_pAudioProcessor::getProgramName (int index)
{
    return {};
}

void Compressor_pAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Compressor_pAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    for(int channel=0;channel<getNumOutputChannels();channel++){
        allCompressors.add(Compressor());
    }
    threshParam=state.getRawParameterValue("threshold");
    slopeParam=state.getRawParameterValue("ratio");
    kneeParam=state.getRawParameterValue("knee");
    attackParam=state.getRawParameterValue("attack");
    releaseParam=state.getRawParameterValue("release");
    makeupParam=state.getRawParameterValue("make_up");
    
}

void Compressor_pAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Compressor_pAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Compressor_pAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    std::string b_s=s.toStdString();
    std::string b_e=e.toStdString();
    float start=std::stof(b_s);
    float end=std::stof(b_e);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    float at=1-std::pow(juce::MathConstants<float>::euler, ((1 / getSampleRate()) * -2.2f) / (*attackParam / 1000.0f));
    float rt=1-std::pow(juce::MathConstants<float>::euler, ((1 / getSampleRate()) * -2.2f) / (*releaseParam / 1000.0f));
    
    if((int)*mode_state.getRawParameterValue("modeID")==1){

        for(int i=0;i<buffer.getNumSamples();i++){
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                auto* data = buffer.getWritePointer (channel);

                // ..do something to the data...
                Compressor* comp=&allCompressors.getReference(channel);
                data[i]=comp->compressorSample(data[i], *threshParam, *slopeParam, at, rt, *releaseParam,*makeupParam);
            }
        }
    }
    
    else if ((int)*mode_state.getRawParameterValue("modeID")==2){

        for(int i=0;i<buffer.getNumSamples();i++){
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                auto* data = buffer.getWritePointer (channel);

                // ..do something to the data...
                Compressor* comp=&allCompressors.getReference(channel);
                data[i]=comp->expanderSample(data[i], *threshParam, *slopeParam, at, rt, *releaseParam,*makeupParam);
            }
        }
    }
    
    else{
        
        for(int i=0;i<buffer.getNumSamples();i++){
            for (int channel = 0; channel < totalNumInputChannels; ++channel)
            {
                auto* data = buffer.getWritePointer (channel);

                // ..do something to the data...
                Compressor* comp=&allCompressors.getReference(channel);
                data[i]=comp->limiterSample(data[i], *threshParam, *slopeParam, at, rt, *releaseParam,*makeupParam);
                
            }
        }
    }
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
}

//==============================================================================
bool Compressor_pAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Compressor_pAudioProcessor::createEditor()
{
    return new Compressor_pAudioProcessorEditor (*this,state);
}

//==============================================================================
void Compressor_pAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto stateTree=state.copyState();
    std::unique_ptr<juce::XmlElement> xml(stateTree.createXml());
    copyXmlToBinary(*xml, destData);
    
    auto modeTree=mode_state.copyState();
    std::unique_ptr<juce::XmlElement> mode_xml(modeTree.createXml());
    copyXmlToBinary(*mode_xml, destData);
}

void Compressor_pAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if(xmlState.get()!=nullptr&&xmlState->hasTagName(state.state.getType())){
        state.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Compressor_pAudioProcessor();
}

