/*
  ==============================================================================

    SampleRecognizer.cpp
    Created: 24 Mar 2017 3:41:23pm
    Author:  Anthony

  ==============================================================================
*/

#include "SampleRecognizer.h"

void SampleRecognizer::performMainHarmonyDetection(float * data, int size, bool overThreshold) noexcept
{
	if (currentPosition == numFrames)
		overThreshold = false;
	if (overThreshold)
	{
		float maxHarmonyValue = FloatVectorOperations::findMaximum(data, size / 2);
		int mainHarmonyIndex;
		for (int i = 0; i < size / 2; i++)
			if (data[i] == maxHarmonyValue)
			{
				mainHarmonyIndex = i;
				break;
			}

		if (mainHarmonyIndex >= bassEndIndex)
			melTotalInfo[currentPosition] = 1;
		else
			melTotalInfo[currentPosition] = 0;
		currentPosition++;
	}
	else
	{
		if (melTotalInfo[0] != -1)
		{
			if (melTotalInfo[0] == 0)
			{
				currentSampleType = sampleType::KICK;
				for (int i = 1; i < numFrames; i++)
					if (melTotalInfo[i] == 1)
					{
						currentSampleType = sampleType::SNARE;
						break;
					}
			}
			else
				currentSampleType = sampleType::HAT;
		}
		else
		{
			currentSampleType = sampleType::NO_TYPE;
		}

		if (currentSampleType != sampleType::NO_TYPE)
		{
			lastSampleType = currentSampleType;
			logString.append("detected: sampleType: " + String(lastSampleType) + "; with melInfo: ", 256);
			for (int i = 0; i < numFrames; i++)
				logString.append(String(melTotalInfo[i]) + " ", 256);
			logString.append("\n", 256);
		}

		resetInfo();
	}
}

void SampleRecognizer::performTripleHarmonyDetection(float * data, int size, bool overThreshold) noexcept
{
	if (_currentPosition == numFrames)
		overThreshold = false;

	if (overThreshold)
	{
		harmonyReload();
		for (int i = 0; i < size / 2; i++)
			shuffleFrame(data, i);

		for (int i = 0; i < numHarmonies; i++)
			if (threeMaxHarmonies[i] >= bassEndIndex)
				_melTotalInfo[_currentPosition][i] = 1;
			else
				_melTotalInfo[_currentPosition][i] = 0;

		_currentPosition++;
	}
	else
	{
		if (_melTotalInfo[0][0] != -1)
		{
			if (getFrameSumAt(0) < 5)
			{
				currentSampleType = sampleType::KICK;
				for (int i = 1; i < numFrames; i++)
				{
					if (getFrameSumAt(i) >= 5)
					{
						currentSampleType = sampleType::SNARE;
						break;
					}
				}
			}
			else
			{
				currentSampleType = sampleType::HAT;

				if (getFrameSumAt(1) < 5)
				{
					currentSampleType = sampleType::KICK;
					for (int i = 2; i < numFrames; i++)
					{
						if (getFrameSumAt(i) >= 5)
						{
							currentSampleType = sampleType::SNARE;
							break;
						}
					}
				}
			}
		}
		else
		{
			currentSampleType = sampleType::NO_TYPE;
		}
		if (currentSampleType != sampleType::NO_TYPE)
		{
			lastSampleType = currentSampleType;
			logString.append("detected: sampleType: " + String(lastSampleType) + "; with melInfo: ", 256);
			for (int i = 0; i < numFrames; i++)
			{
				for (int j = 0; j < numHarmonies; j++)
					logString.append(String(_melTotalInfo[i][j]) + " ", 256);
				logString.append(" | ", 256);
			}
			logString.append("\n", 256);
		}

		_resetInfo();
	}
}

void SampleRecognizer::performMelEnergyDetection(float * data, int size, bool overThreshold) noexcept
{
}

