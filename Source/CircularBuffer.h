/*
  ==============================================================================

    CircularBuffer.h
    Created: 6 Nov 2021 4:11:34pm
    Author:  Rep

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class CircularBuffer {
public:
    CircularBuffer();
    CircularBuffer(int bufferSize, int delayLength);
    float getData();
    void setData(float data);
    void nextSample();
private:
    juce::AudioSampleBuffer buffer;
    int writeIndex;
    int readIndex;
    int delayLength;
};
