/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <cmath>
#include "IntelFFT.h"
#include "FreqDBSpectrum.h"
#include "TimeFreqSpectrum.h"

#ifdef JUCE_MSVC
//#pragma warning(disable: 4244)
#endif

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent,
							   private Timer,
							   private Button::Listener,
							   private Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent()
		: forwardFFT(fftSize, false),
		  currentDataPosition(0),
		  nextBlockReady(false),
		  paused(false),
		  switchModeButton("Switch mode", 0.25f, Colours::greenyellow),
		  pauseButton("Pause"),
		  speedSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxBelow),
		  threshold(0.001f)
    {
		setSize(800, 600);
		speedSlider.setRange(0, 1);
		switchModeButton.addListener(this);
		pauseButton.addListener(this);
		speedSlider.addListener(this);

		currentSpectrum = &spectrum;
		addAndMakeVisible(currentSpectrum);
		addAndMakeVisible(switchModeButton);
		addAndMakeVisible(pauseButton);
		addAndMakeVisible(speedSlider);
		speedSlider.setAlwaysOnTop(true);
		switchModeButton.setAlwaysOnTop(true);
		pauseButton.setAlwaysOnTop(true);
		speedSlider.setEnabled(false);

        setAudioChannels (2, 0);
		startTimerHz(60);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

	void switchMode()
	{
		Component* tmp = currentSpectrum;
		if (currentSpectrum == &spectrum)
		{
			currentSpectrum = &tfSpectrum;
			speedSlider.setEnabled(true);
		}
		else
		{
			currentSpectrum = &spectrum;
			speedSlider.setEnabled(false);
		}

		tmp->setVisible(false);
		removeChildComponent(tmp);
		currentSpectrum->prepareToRender(sampleRate, fftSize);
		addAndMakeVisible(currentSpectrum);
	}
	/*Listeners*/
	void buttonClicked(Button* button) override
	{
		if (button == &switchModeButton)
			switchMode();
		else if (button == &pauseButton)
			paused = pauseButton.getToggleState();
	}

	void sliderValueChanged(Slider* slider) override
	{
		if (slider == &speedSlider)
		{
			tfSpectrum.changeSpeed(speedSlider.getValue());
		}
	}

	void timerCallback() override
	{
		if (nextBlockReady && !paused)
		{
			updateSpectrogram();
			nextBlockReady = false;
		}
	}

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
		this->sampleRate = sampleRate;
		this->bufferLength = samplesPerBlockExpected;
		currentSpectrum->prepareToRender(sampleRate, fftSize);

		zeromem(fftData, bufferLength);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
		const float* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
		{
			const ScopedLock sl(writerLock);

			for (int i = 0; i < bufferToFill.numSamples; i++)
				pushNextfftBlock(channelData[i]);
		}

		bufferToFill.clearActiveBufferRegion();
    }

	void pushNextfftBlock(float sample) //OPTIMIZE THIS SHIT
	{
		if (currentDataPosition == fftSize)
		{
			if (!nextBlockReady)
			{
				zeromem(fftData, sizeof(fftData));
				memcpy(fftData, inputData, sizeof(inputData));
				nextBlockReady = true;
			}

			currentDataPosition = 0;
		}

		inputData[currentDataPosition++] = sample;
	}

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
		Font mcF("Times New Roman", 14.0f, Font::bold | Font::italic);
		g.setFont(mcF);
        g.fillAll (Colours::darkgoldenrod);

		g.drawText(currentSpectrum->getMode(), switchModeButton.getBounds().withY(0), Justification::centred);
    }

    void resized() override
    {
		Rectangle<int> localBounds = getLocalBounds();

		spectrum.setBounds(20, 20,
			localBounds.getWidth() - localBounds.getWidth() / 20, localBounds.getHeight() - localBounds.getHeight() / 15);
		tfSpectrum.setBounds(spectrum.getBounds());

		switchModeButton.setBounds(localBounds.getWidth() - localBounds.getWidth() / 10,
			20, localBounds.getWidth() / 20, localBounds.getHeight() / 40);
		pauseButton.setBounds(switchModeButton.getBounds().withX(switchModeButton.getX() - switchModeButton.getWidth() * 2));
		speedSlider.setBounds(pauseButton.getBounds().withX
			(pauseButton.getX() - pauseButton.getWidth() * 3).withWidth(pauseButton.getWidth() * 3));
    }

	void updateSpectrogram()
	{
		Range<float> minMax = FloatVectorOperations::findMinAndMax(fftData, fftSize);

		float maxLevel = 0;
		if (minMax.getEnd() >= threshold)
			maxLevel = minMax.getEnd();

		forwardFFT.performFrequencyForwardTransform(fftData);
		currentSpectrum->renderNextFrame(fftData, maxLevel);
	}


private:
    //==============================================================================
	enum
	{
		fftSize = 1 << 10
	};

	ArrowButton switchModeButton;
	ToggleButton pauseButton;

	FreqDBSpectrum spectrum;
	TimeFreqSpectrum tfSpectrum;
	Spectrum* currentSpectrum;
	Slider speedSlider;

	IntelFFT forwardFFT;

	float fftData[fftSize];
	float inputData[fftSize];
	int currentDataPosition;
	bool nextBlockReady;
	bool paused;

	double sampleRate;
	int bufferLength;
	const float threshold;

	CriticalSection writerLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
