/*
  ==============================================================================

    IntelFFT.cpp
    Created: 17 Dec 2016 4:30:37am
    Author:  Anthony

  ==============================================================================
*/

#include "IntelFFT.h"

IntelFFT::IntelFFT(int size, bool doublePrecision)
			: opStatus(0),
			  logFile(juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getFullPathName() + "\\Recorded\\FFTLog.txt")
{
	doublePrecision
		? opStatus = DftiCreateDescriptor(&descriptor, DFTI_DOUBLE, DFTI_COMPLEX, 1, size)
		: opStatus = DftiCreateDescriptor(&descriptor, DFTI_SINGLE, DFTI_COMPLEX, 1, size);

	opStatus = DftiSetValue(descriptor, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
	opStatus = DftiSetValue(descriptor, DFTI_BACKWARD_SCALE, 1.0f / size);
	opStatus = DftiSetValue(descriptor, DFTI_FORWARD_SCALE, 1.0f);
	opStatus = DftiCommitDescriptor(descriptor);

	intelFFTCorrectCheck();
	numSamples = size;

	input.calloc(size);
	output.calloc(size);
}

IntelFFT::~IntelFFT()
{
	opStatus = DftiFreeDescriptor(&descriptor);
	intelFFTCorrectCheck();
}

void IntelFFT::performRealForwardTransform(float * data) noexcept
{
	for (int i = 0; i < numSamples; i++)
		input[i].real = data[i];
	performComplexForwardTransform();
}

void IntelFFT::performRealForwardTransform(double * data)
{
	
}

void IntelFFT::performRealBackwardTransform(float * data) noexcept
{
	MKL_Complex8* complexData = reinterpret_cast<MKL_Complex8*>(data);

	for (int i = 0; i < numSamples; ++i)
	{
		input[i].real = complexData[i].real;
		input[i].imag = complexData[i].imag;
	}

	performComplexBackwardTransform();
	for (int i = 0; i < numSamples; ++i)
		data[i] = output[i].real;
}

void IntelFFT::performRealBackwardTransform(double * data) noexcept
{
}

void IntelFFT::performFrequencyForwardTransform(float * data) noexcept
{
	performRealForwardTransform(data);
	for (int i = 0; i < numSamples; ++i)
		data[i] = juce::juce_hypot(output[i].real, output[i].imag);
}

void IntelFFT::performFrequencyForwardTransform(double * data) noexcept
{
}

void IntelFFT::performComplexForwardTransform() noexcept
{
	opStatus = DftiComputeForward(descriptor, input, output);
	//Correct check?
}

void IntelFFT::performComplexBackwardTransform() noexcept
{
	opStatus = DftiComputeBackward(descriptor, input, output);
	//Correct check?
}

void IntelFFT::intelFFTCorrectCheck()
{
	/*TODO: fix correct check*/
	logTime.getCurrentTime();
	if (opStatus)
	{
		jassertfalse;
		logFile.appendText(logTime.formatted("%c") + " Error: " + juce::String(DftiErrorMessage(opStatus)) + "\n");
	}
}
