/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Source/Compressor.h"
#include <iostream>
#include <sstream>
#include <string>
//==============================================================================
/**
*/
class Compressor_pAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Compressor_pAudioProcessor();
    ~Compressor_pAudioProcessor() override;

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
    
    juce::AudioProcessorValueTreeState state;
    juce::Array<Compressor> allCompressors;
    
    std::atomic<float> * threshParam, *slopeParam, *kneeParam, *attackParam, *releaseParam, *makeupParam;
    juce::AudioProcessorValueTreeState mode_state;
    juce::AudioProcessorValueTreeState::ParameterLayout initializeGUI();
    
    juce::String s,e;
    
private:
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Compressor_pAudioProcessor)
};
