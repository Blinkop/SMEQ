/*
  ==============================================================================

    TimeFreqSpectrum.h
    Created: 15 Feb 2017 10:26:57pm
    Author:  Anthony

  ==============================================================================
*/

#ifndef TIMEFREQSPECTRUM_H_INCLUDED
#define TIMEFREQSPECTRUM_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Spectrum.h"
#include <cmath>

class TimeFreqSpectrum : public Spectrum
{
public:
	TimeFreqSpectrum()
		: spectogramImage(Image::PixelFormat::ARGB, 128, 1024, true),
		  oYScale(std::log10(20), std::log10(20000))
	{
		mode = "Hz(t)";
		setOpaque(true);
	}
	~TimeFreqSpectrum()
	{

	}

	void paint(Graphics& g) override
	{
		Font mcF("Times New Roman", 10.0f, Font::bold | Font::italic);
		g.setFont(mcF);
		g.fillAll(Colours::darkgoldenrod);
		g.drawImage(spectogramImage, getLocalBounds().toFloat());
		g.drawArrow(freqLine, 1.0f, 5, 10);
		g.drawArrow(timeLine, 1.0f, 5, 10);

		for (int i = 0; i < 10; i++)
		{
			g.drawHorizontalLine(oYPoints[i].getY(), oYPoints[i].getX(), oYPoints[i].getX() + 3);
			g.drawText(String(freqScales[i]), oYPoints[i].getX() + 2, oYPoints[i].getY() + 5, 40, 10, Justification::centred);
		}
	}

	void resized() override
	{
		Rectangle<int> localBounds = getLocalBounds();

		freqLine.setStart(0, localBounds.getHeight());
		timeLine.setStart(freqLine.getStart());
		freqLine.setEnd(0, 0);
		timeLine.setEnd(localBounds.getWidth(), localBounds.getHeight());

		for (int i = 0; i < 10; i++)
			oYPoints[i].setXY(0, localBounds.getHeight() - jmap(std::log10(freqScales[i]),
				oYScale.getStart(), oYScale.getEnd(), static_cast<double>(20), static_cast<double>(freqLine.getLength())));


	}

	void renderNextFrame(float* fftData)
	{
		Range<float> minMax = FloatVectorOperations::findMinAndMax(fftData, fftSize);

		const int rightEdge = spectogramImage.getWidth() - 1;
		const int imageHeight = spectogramImage.getHeight();

		spectogramImage.moveImageSection(0, 0, 1, 0, rightEdge, imageHeight);

		for (int i = 0; i < fftSize; i++)
		{
			const int fftIndex = jmap(static_cast<int>(bandDiff * (i + 1)), 0, static_cast<int>(sampleRate / 2), 0, fftSize - 1);
			const int level = jmap(fftData[fftIndex], minMax.getStart(),
				minMax.getEnd(), 0.0f, 255.0f);

			const int yPosition = imageHeight
				- jmap(static_cast<double>(std::log10(bandDiff * i + 1)),
					oYScale.getStart(), oYScale.getEnd(), static_cast<double>(0), static_cast<const double>(imageHeight));

			spectogramImage.setPixelAt(rightEdge, yPosition, Colour::fromRGBA(0, 0, 0, level));
		}

		repaint();
	}

	void prepareToRender(double sampleRate, int dataSize)
	{
		this->sampleRate = sampleRate;
		this->fftSize = dataSize / 2;

		bandDiff = 20000 / fftSize;

		spectogramImage.clear(spectogramImage.getBounds(), Colours::darkgoldenrod);
	}

private:
	Image spectogramImage;
	Line<float> freqLine;
	Line<float> timeLine;

	Point<float> oYPoints[10];
	const Range<double> oYScale;
	int freqScales[10] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
};


#endif  // TIMEFREQSPECTRUM_H_INCLUDED
