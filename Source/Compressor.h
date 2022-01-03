/*
  ==============================================================================

    Compressor.h
    Created: 6 Nov 2021 4:11:34pm
    Author:  Rep

  ==============================================================================
*/
#include "CircularBuffer.h"
#pragma once

class Compressor{
public:
    Compressor();
    float compressorSample(float data,float threshold,float ratio,float attack,float release,float knee,float makeup);
    float expanderSample(float data,float threshold,float ratio,float attack,float release,float knee,float makeup);
    float limiterSample(float data,float threshold,float ratio,float attack,float release,float knee,float makeup);
private:
    CircularBuffer buffer;
    float tav,rms,gain;
};
