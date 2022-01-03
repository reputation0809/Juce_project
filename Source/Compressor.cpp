/*
  ==============================================================================

    Compressor.cpp
    Created: 6 Nov 2021 4:11:34pm
    Author:  Rep

  ==============================================================================
*/

#include "Compressor.h"

Compressor::Compressor(){
    buffer=CircularBuffer(150,20);
    tav=0.01;
    rms=0;
    gain=1;
}

float Compressor::compressorSample(float data, float threshold, float ratio, float attack,float release, float knee, float makeup){
    rms=(1-tav)*rms+tav*std::pow(data, 2.0f);
    float dbRMS=10*std::log10(rms);
    
    float slope=1-(1/ratio);
    float dbGain=std::min(0.0f,(slope*(threshold-dbRMS)));
    float newGain=std::pow(10,dbGain/20);
    
    float coeff;
    if(newGain<gain) coeff=attack;
    else coeff=release;
    gain=(1-coeff)*gain+coeff*newGain;
    
    float compressedSample=gain*buffer.getData();
    buffer.setData(data);
    buffer.nextSample();
    
    return compressedSample;
}

float Compressor::expanderSample(float data, float threshold, float ratio, float attack,float release, float knee, float makeup){
    rms=(1-tav)*rms+tav*std::pow(data, 2.0f);
    float dbRMS=10*std::log10(rms);
    
    float slope=1-(1/ratio);
    float dbGain=std::max(0.0f,(slope*(threshold-dbRMS)));
    float newGain=std::pow(10,dbGain/20);
    
    float coeff;
    if(newGain<gain) coeff=attack;
    else coeff=release;
    gain=(1-coeff)*gain+coeff*newGain;
    
    float expandedSample=gain*buffer.getData();
    buffer.setData(data);
    buffer.nextSample();
    
    return expandedSample;
}

float Compressor::limiterSample(float data,float threshold,float ratio,float attack,float release,float knee,float makeup){
    float coeff;
    float xpeak=0.0f;
    
    float amplitude = abs(data);
    if(amplitude > xpeak) coeff = attack;
    else coeff = release;
    
    xpeak = (1-coeff)*xpeak+coeff*amplitude;
    
    float filter = fmin(1.0f, threshold/xpeak);
    if(gain > filter) coeff = attack;
    else coeff = release;
    gain = (1-coeff)*gain+coeff*filter;
    
    float limitedSample =gain*buffer.getData();
    buffer.setData(data);
    buffer.nextSample();
    
    return limitedSample;
}
