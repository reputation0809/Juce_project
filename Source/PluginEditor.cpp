/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Compressor_pAudioProcessorEditor::Compressor_pAudioProcessorEditor (Compressor_pAudioProcessor& p,juce::AudioProcessorValueTreeState& state)
    : AudioProcessorEditor (&p), audioProcessor (p),params(state)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addSlider("threshold", "Threshold (dB)", threshold, threshold_label, threshold_a);
    addSlider("ratio", "Ratio", slope, slope_label, slope_a);
    addSlider("knee", "Knee (dB)", knee, knee_label, knee_a);
    addSlider("attack", "Attack (s)", attack, attack_label, attack_a);
    addSlider("release", "Release (s)", release, release_label, release_a);
    addSlider("make_up", "Make up (dB)", make_up, make_up_label, make_up_a);
   // addCombobox("mode", "Mode", mode, mode_label, mode_a);
    setSize (500, 400);
    createGUI();
}

Compressor_pAudioProcessorEditor::~Compressor_pAudioProcessorEditor()
{
}

//==============================================================================
void Compressor_pAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello Juce!", getLocalBounds(), juce::Justification::centred, 1);
}

void Compressor_pAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    threshold.setBounds(100, 50, 200, 50);
    slope.setBounds(100, 100, 200, 50);
    knee.setBounds(100, 150, 200, 50);
    attack.setBounds(100, 200, 200, 50);
    release.setBounds(100, 250, 200, 50);
    make_up.setBounds(100, 300, 200, 50);
}

void Compressor_pAudioProcessorEditor::addSlider(juce::String name,juce::String labelText,juce::Slider& slider,juce::Label& label,std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment){
    addAndMakeVisible(slider);
    attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(params,name,slider));
    label.setText(labelText, juce::dontSendNotification);
    label.attachToComponent(&slider, true);
    addAndMakeVisible(label);
}

void Compressor_pAudioProcessorEditor::createGUI(){
    mode.addItem("Compressor", 1);
    mode.addItem("Expander", 2);
    mode.addItem("Limiter", 3);
    mode.setBounds(100, 15, 100, 30);
    addAndMakeVisible(mode);
    
    mode_label.setText("Mode", juce::dontSendNotification);
    mode_label.attachToComponent(&mode, true);
    addAndMakeVisible(mode_label);
    
    mode_a=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.mode_state,"modeID",mode);
    
    start.setBounds(300, 15, 30, 30);
    start.setJustification(juce::Justification::centred);
    start.addListener(this);
    addAndMakeVisible(start);
    label1.setText("Set Boundary", juce::dontSendNotification);
    label1.attachToComponent(&start, true);
    addAndMakeVisible(label1);
    
    end.setBounds(345, 15, 30, 30);
    end.setJustification(juce::Justification::centred);
    addAndMakeVisible(end);
    end.addListener(this);
    label3.setText("~", juce::dontSendNotification);
    label3.attachToComponent(&end, true);
    addAndMakeVisible(label3);
    
    
}

void Compressor_pAudioProcessorEditor::valuechanged(juce::TextEditor* text){
    if(text==&start){
        audioProcessor.s=start.getText();
    }
    else{
        audioProcessor.e=end.getText();
    }
}
