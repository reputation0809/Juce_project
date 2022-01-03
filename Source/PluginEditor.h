/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Compressor_pAudioProcessorEditor  : public juce::AudioProcessorEditor,
public juce::TextEditor::Listener
{
public:
    Compressor_pAudioProcessorEditor (Compressor_pAudioProcessor&,juce::AudioProcessorValueTreeState&);
    ~Compressor_pAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void addSlider(juce::String name,juce::String labelText,juce::Slider& slider,juce::Label& label,std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment);
    /*void addCombobox(juce::String name,juce::String labelText,juce::ComboBox& combobox,juce::Label& label,std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>& attachment);*/
    
    void valuechanged(juce::TextEditor* text);
    
    void createGUI();
    
    juce::TextEditor start,end;
    
    Compressor_pAudioProcessor& audioProcessor;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::AudioProcessorValueTreeState& params;
    juce::Slider threshold,slope,knee,attack,release,make_up;
    juce::Label threshold_label,slope_label,knee_label,attack_label,release_label,make_up_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threshold_a,slope_a,knee_a,attack_a,release_a,make_up_a;
    juce::ComboBox mode;
    juce::Label mode_label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> mode_a;
    
    juce::Label label1,label3;
    
    
    //std::unique_ptr<juce::AudioProcessorValueTreeState::Att> start_m_a,start_s_a,end_m_a,end_s_a;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Compressor_pAudioProcessorEditor)
};

