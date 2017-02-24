/*
  ==============================================================================

    FreqDBSpectrum.h
    Created: 15 Feb 2017 10:31:23pm
    Author:  Anthony

  ==============================================================================
*/

#ifndef FREQDBSPECTRUM_H_INCLUDED
#define FREQDBSPECTRUM_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"
#include "Spectrum.h"

//==============================================================================
/*
*/
class FreqDBSpectrum : public Spectrum
{
public:
	FreqDBSpectrum()
		: oXScale(std::log10(20), std::log10(20000))
	{
		mode = "dB(Hz)";
	}

	~FreqDBSpectrum()
	{
	}

	void paint(Graphics& g) override
	{
		Font mcF("Times New Roman", 10.0f, Font::bold | Font::italic);
		g.setFont(mcF);
		g.fillAll(Colours::darkgoldenrod);
		g.setColour(Colours::black);

		for (int i = 0; i < fftSize - 1; i++)
			g.drawLine(peaks[i].getX(), peaks[i].getY(), peaks[i + 1].getX(), peaks[i + 1].getY(), 0.5f);

		g.drawArrow(dBLine, 1.0f, 5, 10);
		g.drawArrow(freqLine, 1.0f, 5, 10);

		for (int i = 0; i < 3; i++)
		{
			g.drawHorizontalLine(oYPoints[i].getY(), oYPoints[i].getX(), oYPoints[i].getX() + 3);
			g.drawText(String(0.5 * i), oYPoints[i].getX() + 5, oYPoints[i].getY(), 40, 10, Justification::centred);
		}
		for (int i = 0; i < 10; i++)
		{
			g.drawVerticalLine(oXPoints[i].getX(), oXPoints[i].getY() - 3, oXPoints[i].getY());
			g.drawText(String(freqScales[i]), oXPoints[i].getX() - 10, oXPoints[i].getY() - 20, 40, 10, Justification::centred);
		}
	}

	void resized() override
	{
		Rectangle<int> localBounds = getLocalBounds();

		freqLine.setStart(0, localBounds.getHeight());
		freqLine.setEnd(localBounds.getWidth(), localBounds.getHeight());
		dBLine.setStart(freqLine.getStart());
		dBLine.setEnd(0, 0);

		oYPoints[0].setXY(0, localBounds.getHeight() - 10);
		oYPoints[1].setX(oYPoints[0].getX());
		oYPoints[1].setY(dBLine.getLength() / 2);
		oYPoints[2].setXY(oYPoints[0].getX(), 20);

		for (int i = 0; i < 10; i++)
			oXPoints[i].setXY(jmap(std::log10(freqScales[i]), oXScale.getStart(),
				oXScale.getEnd(), static_cast<double>(20), static_cast<double>(freqLine.getLength())), freqLine.getStartY());

		for (int i = 0; i < fftSize; i++)
			peaks[i].setX(jmap(static_cast<double>(std::log10(bandDiff * i + 20)), oXScale.getStart(),
				oXScale.getEnd(), static_cast<double>(0), static_cast<double>(localBounds.getWidth())));
	}

	void renderNextFrame(float* fftData, const float maxLevel)
	{
		Range<float> minMax = FloatVectorOperations::findMinAndMax(fftData, fftSize);

		for (int i = 0; i < fftSize; i++)
		{
			const int fftIndex = jmap(static_cast<int>(bandDiff * (i + 1)), 0, static_cast<int>(sampleRate / 2), 0, fftSize - 1);
			const float level = jmap(fftData[fftIndex], minMax.getStart(),
				minMax.getEnd(), 0.0f, maxLevel);
			const int yScale = jmap(level, 0.0f, 1.0f, 0.0f, static_cast<float>(getLocalBounds().getHeight() - 30));

			peaks[i].setY(getLocalBounds().getHeight() - 10 - yScale);
		}

		repaint();
	}

	void prepareToRender(double sampleRate, int dataSize)
	{
		this->sampleRate = sampleRate;
		this->fftSize = dataSize / 2;

		frequencyDomain.setStart(20);
		frequencyDomain.setEnd(sampleRate / 2);

		stopIndex = jmap(20000, 0, static_cast<int>(sampleRate / 2), 0, fftSize - 1);
		bandDiff = 20000 / fftSize;

		for (int i = 0; i < fftSize; i++)
			peaks[i].setX(jmap(static_cast<double>(std::log10(bandDiff * i + 20)), oXScale.getStart(),
				oXScale.getEnd(), static_cast<double>(0), static_cast<double>(getLocalBounds().getWidth())));
	}

private:
	Point<float> peaks[512];

	Line<float> dBLine;
	Line<float> freqLine;

	Point<float> oXPoints[10];
	Point<float> oYPoints[3];

	int freqScales[10] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };

	Range<float> frequencyDomain;
	const Range<double> oXScale;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreqDBSpectrum)
};


#endif  // FREQDBSPECTRUM_H_INCLUDED
