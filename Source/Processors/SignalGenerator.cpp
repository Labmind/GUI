/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2012 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "SignalGenerator.h"
#include <stdio.h>
#include <math.h>

SignalGenerator::SignalGenerator()
	: GenericProcessor("Signal Generator"),

	  defaultFrequency(10.0),
	  defaultAmplitude (100.0f),
	  nOut(5)	
{


}


SignalGenerator::~SignalGenerator()
{

}


AudioProcessorEditor* SignalGenerator::createEditor( )
{
	editor = new SignalGeneratorEditor(this);
	return editor;
}

void SignalGenerator::updateSettings()
{

	//std::cout << "Signal generator updating parameters" << std::endl;

	while (waveformType.size() < getNumOutputs())
	{
		waveformType.add(SINE);
		frequency.add(defaultFrequency);
		amplitude.add(defaultAmplitude);
		phase.add(0);
		phasePerSample.add(double_Pi * 2.0 / (getSampleRate() / frequency.getLast()));
		currentPhase.add(0);
	}

	sampleRateRatio = getSampleRate() / 44100.0;

	std::cout << "Sample rate ratio: " << sampleRateRatio << std::endl;

}

void SignalGenerator::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Message received." << std::endl;

	if (currentChannel > -1) {
		if (parameterIndex == 0) {
			amplitude.set(currentChannel,newValue*100.0f);
		} else if (parameterIndex == 1) {
			frequency.set(currentChannel,newValue);
			phasePerSample.set(currentChannel, double_Pi * 2.0 / (getSampleRate() / frequency[currentChannel]));
		} else if (parameterIndex == 2) {
			phase.set(currentChannel, newValue/360.0f * (double_Pi * 2.0));
		} else if (parameterIndex == 3) {
			waveformType.set(currentChannel, (int) newValue);
		}

		//updateWaveform(currentChannel);
	}

}


bool SignalGenerator::enable () {

	std::cout << "Signal generator received enable signal." << std::endl;
	
	// for (int n = 0; n < waveformType.size(); n++)
	// {
	// 	updateWaveform(n);
		
	// }

	return true;
}

// void SignalGenerator::updateWaveform(int n)
// {

	// Array<float> cycleData;

	// int cycleLength = int(getSampleRate() / frequency[n]);
	// float phasePerSample = double_Pi * 2.0 / (getSampleRate() / frequency[n]);

	// cycleData.ensureStorageAllocated(cycleLength);

	// for (int i = 0; i < cycleLength; i++)
	// {
	// 	switch (waveformType[n])
	// 	{
	// 		case SINE:
	// 			cycleData.add(amplitude[n] * std::sin(i*phasePerSample + phase[n]));
	// 			break;
	// 		case SQUARE:
	// 			cycleData.add(amplitude[n] * copysign(1,std::sin(i*phasePerSample + phase[n])));
	// 			break;
	// 		case TRIANGLE:
	// 			cycleData.add(0);
	// 			break;
	// 		case SAW:
	// 			cycleData.add(0);
	// 			break;
	// 		case NOISE:
	// 			cycleData.add(0);
	// 			break;
	// 		default:
	// 			cycleData.set(i, 0);
	// 	}

	// }

	// waveforms.set(n, cycleData);

	// currentSample.set(n,0);

// }

bool SignalGenerator::disable() {
	
	std::cout << "Signal generator received disable signal." << std::endl;
	return true;
}

void SignalGenerator::process(AudioSampleBuffer &buffer, 
                            MidiBuffer &midiMessages,
                            int& nSamps)
{

	nSamps = int((float) buffer.getNumSamples() * sampleRateRatio);

    for (int i = 0; i < nSamps; ++i)
    {
        for (int j = buffer.getNumChannels(); --j >= 0;) {

        	float sample;

        	switch (waveformType[j])
        	{
        		case SINE:
        			sample = amplitude[j] * (float) std::sin (currentPhase[j] + phase[j]);
   					break;
				case SQUARE:
					sample = amplitude[j] * copysign(1,std::sin(currentPhase[j] + phase[j]));
					break;
				case TRIANGLE:
					sample = amplitude[j] * ((currentPhase[j] + phase[j]) / double_Pi - 1) *
							copysign(2,std::sin(currentPhase[j] + phase[j]));
					break;
				case SAW:
					sample = amplitude[j] * ((currentPhase[j] + phase[j]) / double_Pi - 1);
					break;
				case NOISE:
					sample = amplitude[j] * (float(rand()) / float(RAND_MAX)-0.5f);
					break;
				default:
					sample = 0;
        	}
        	
       		currentPhase.set(j,currentPhase[j] + phasePerSample[j]);

       		if (currentPhase[j] > double_Pi*2)
       			currentPhase.set(j,0);

       		// dereference pointer to one of the buffer's samples
            *buffer.getSampleData (j, i) = sample;
        }
    }


 	

	// for (int chan = 0; chan < buffer.getNumChannels(); chan++)
	// {
		
	// 	int dataSize = waveforms[chan].size();
	// 	int destSample = -dataSize;
	// 	int lastSample = dataSize;

	// 	while (lastSample < nSamps)
	// 	{

	// 		destSample += dataSize;

	// 		//std::cout << lastSample << " " << destSample << " " << currentSample[chan] << " " << dataSize << std::endl;

	// 		// buffer.copyFrom(chan, 
	// 		//  				destSample, 
	// 		//  				waveforms[chan].getRawDataPointer() + currentSample[chan], 
	// 		//  				dataSize - currentSample[chan]);

	// 		lastSample += dataSize;

	// 		currentSample.set(chan,0);

	// 		//std::cout << "DONE" << std::endl;
	// 	}

	// 	//std::cout << lastSample << " " << destSample << " " << currentSample[chan] << " " << dataSize << std::endl;

	// 	if (destSample < 0)
	// 		destSample = 0;

	// 	int samplesLeft = nSamps - destSample;

	// 	if (samplesLeft < dataSize - currentSample[chan])
	// 	{
	// 	 	// buffer.copyFrom(chan,
	// 	 	// 			destSample,
	// 	 	// 			waveforms[chan].getRawDataPointer() + currentSample[chan],
	// 	 	// 			samplesLeft);

	// 	 	currentSample.set(chan, currentSample[chan] + samplesLeft);

	// 	} else {

	// 		int samps = dataSize - currentSample[chan];

	// 		// buffer.copyFrom(chan,
	// 	 // 				destSample,
	// 	 // 				waveforms[chan].getRawDataPointer() + currentSample[chan],
	// 	 // 				samps);

	// 		destSample += samps;
	// 		samplesLeft -= samps;

	// 		// buffer.copyFrom(chan,
	// 	 // 				destSample,
	// 	 // 				waveforms[chan].getRawDataPointer(),
	// 	 // 				samplesLeft);

	// 		currentSample.set(chan, samplesLeft);
	// 	}

	// }

}
