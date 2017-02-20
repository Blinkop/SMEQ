/*
  ==============================================================================

    Spectrum.h
    Created: 4 Feb 2017 4:45:16am
    Author:  Anthony

  ==============================================================================
*/

#ifndef SPECTRUM_H_INCLUDED
#define SPECTRUM_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Spectrum    : public Component
{
public:
	Spectrum() {}
	~Spectrum() {}

	void paint(Graphics& g) override {}
	void resized() override {}

	virtual void renderNextFrame(float* fftData) = 0;
	virtual void prepareToRender(double sampleRate, int dataSize) = 0;
	String getMode() const { return mode; }
protected:
	double sampleRate;
	int fftSize;
	int stopIndex;
	float bandDiff;

	String mode;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Spectrum)
};


#endif  // SPECTRUM_H_INCLUDED
