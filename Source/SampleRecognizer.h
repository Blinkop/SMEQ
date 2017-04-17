/*
  ==============================================================================

    SampleRecognizer.h
    Created: 24 Mar 2017 3:41:23pm
    Author:  Anthony

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include <cmath>

#ifndef SAMPLERECOGNIZER_H_INCLUDED
#define SAMPLERECOGNIZER_H_INCLUDED

class SampleRecognizer
{
public:
	static SampleRecognizer& getRecognizer()
	{
		static SampleRecognizer recognizer;
		return recognizer;
	}

	void performMainHarmonyDetection(float* data, int size, bool overThreshold) noexcept;
	void performTripleHarmonyDetection(float* data, int size, bool overThreshold) noexcept;
	void performMelEnergyDetection(float* data, int size, bool overThreshold) noexcept;

	void reloadIndexes(int sampleRate, int fftSize)
	{
		bassEndIndex = jmap(static_cast<int>(BASS_END), 0, sampleRate / 2, 0, fftSize / 2 - 1);
	}

	enum sampleType
	{
		NO_TYPE = 0,
		KICK = 1,
		SNARE = 2,
		HAT = 3
	};

	enum widnowSpecification
	{
		BASS_END = 1000,
		numWidnows = 2
	};

	enum
	{
		numFrames = 10,
		numHarmonies = 3
	};

	enum HarmonyCoefficients
	{
		firstHarmony = 4,
		secondHarmony = 3,
		thirdHarmony = 2
	};

	sampleType getType() const { return currentSampleType; }
	sampleType getLastType() const { return lastSampleType; }
private:
	SampleRecognizer() : log(juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
		.getFullPathName() + "\\Voice managing drum machine\\Recognizer log.txt")
	{
		resetInfo();
		_resetInfo();

		currentSampleType = sampleType::NO_TYPE;
		lastSampleType = currentSampleType;
		
		logString = "";
	}
	~SampleRecognizer()
	{
		log.appendText(logString);
	}

	void shuffleFrame(const float* data, int dataIndex)
	{
		if (data[dataIndex] > data[threeMaxHarmonies[0]])
		{
			threeMaxHarmonies[2] = threeMaxHarmonies[1];
			threeMaxHarmonies[1] = threeMaxHarmonies[0];
			threeMaxHarmonies[0] = dataIndex;
		}
		else if (data[dataIndex] == data[threeMaxHarmonies[0]] || data[dataIndex] > data[threeMaxHarmonies[1]])
		{
			threeMaxHarmonies[2] = threeMaxHarmonies[1];
			threeMaxHarmonies[1] = dataIndex;
		}
		else if (data[dataIndex] == data[threeMaxHarmonies[1]] || data[dataIndex] > data[threeMaxHarmonies[2]])
		{
			threeMaxHarmonies[2] = dataIndex;
		}
	}

	void resetInfo() noexcept
	{
		for (int i = 0; i < numFrames; i++)
			melTotalInfo[i] = -1;

		currentPosition = 0;
	}

	void _resetInfo() noexcept
	{
		for (int i = 0; i < numFrames; i++)
			for (int j = 0; j < numHarmonies; j++)
				_melTotalInfo[i][j] = -1;

		_currentPosition = 0;
	}

	void __resetInfo() noexcept
	{
		for (int i = 0; i < numFrames; i++)
			for (int j = 0; i < numWidnows; j++)
				__melTotalInfo[i][j] = -1;

		__currentPosition = 0;
	}

	inline void harmonyReload() noexcept
	{
		zeromem(threeMaxHarmonies, numHarmonies * sizeof(int));
	}

	inline int getFrameSumAt(int index) const
	{
		return	_melTotalInfo[index][0] * firstHarmony
				+ _melTotalInfo[index][1] * secondHarmony
				+ _melTotalInfo[index][2] * thirdHarmony;
	}

	

	SampleRecognizer(const SampleRecognizer&) = delete;
	const SampleRecognizer& operator=(const SampleRecognizer&) = delete;
	SampleRecognizer(SampleRecognizer&&) = delete;
	const SampleRecognizer& operator=(SampleRecognizer&&) = delete;

	int bassEndIndex;
	/*------------------------------------------------------------*/
	/*-------------------Main harmony recognition-----------------*/
	/*------------------------------------------------------------*/
	int melTotalInfo[numFrames];
	int currentPosition;
	/*------------------------------------------------------------*/
	/*-------------------3 harmony recognition--------------------*/
	/*------------------------------------------------------------*/
	int _melTotalInfo[numFrames][numHarmonies];
	int threeMaxHarmonies[numHarmonies];
	int _currentPosition;
	/*------------------------------------------------------------*/
	/*-------------------Mel window energy recognition------------*/
	/*------------------------------------------------------------*/
	int __melTotalInfo[numFrames][numWidnows];
	int __currentPosition;

	sampleType currentSampleType;
	sampleType lastSampleType;

	File log;
	String logString;
};



#endif  // SAMPLERECOGNIZER_H_INCLUDED
