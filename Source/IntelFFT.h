/*
  ==============================================================================

    IntelFFT.h
    Created: 17 Dec 2016 4:30:37am
    Author:  Anthony

  ==============================================================================
*/

#ifndef INTELFFT_H_INCLUDED
#define INTELFFT_H_INCLUDED

#include <mkl.h>
#include "../JuceLibraryCode/JuceHeader.h"

class IntelFFT
{
public:
	IntelFFT(int size, bool doublePrecision);
	~IntelFFT();

	inline void performRealForwardTransform(float* data) noexcept;
	inline void performRealForwardTransform(double* data);

	void performRealBackwardTransform(float* data) noexcept;
	void performRealBackwardTransform(double* data) noexcept;

	void performFrequencyForwardTransform(float* data) noexcept;
	void performFrequencyForwardTransform(double* data) noexcept;

	const MKL_Complex8* getPerformedData() const { return output; }
private:
	DFTI_DESCRIPTOR_HANDLE descriptor;
	long opStatus;
	int numSamples;

	juce::File logFile;
	juce::Time logTime;

	juce::HeapBlock<MKL_Complex8> input;
	juce::HeapBlock<MKL_Complex8> output;

	inline void performComplexForwardTransform() noexcept;
	inline void performComplexBackwardTransform() noexcept;

	inline void intelFFTCorrectCheck();
};



#endif  // INTELFFT_H_INCLUDED
