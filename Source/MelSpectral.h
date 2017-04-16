/*
  ==============================================================================

    MelSpectral.h
    Created: 15 Apr 2017 4:26:21pm
    Author:  Anthony

  ==============================================================================
*/

#ifndef MELSPECTRAL_H_INCLUDED
#define MELSPECTRAL_H_INCLUDED

#include <cmath>

class MelMath
{
public:
	static double hertzToMel(double hz) { return 1127.01048 * std::log(1 + hz / 700); }
	static double melToHertz(double mel) { return 700 * (std::exp(mel / 1127.01048) - 1); }
private:
	MelMath() {}
	~MelMath() {}

	MelMath(const MelMath&) = delete;
	const MelMath& operator=(const MelMath&) = delete;
	MelMath(MelMath&&) = delete;
	const MelMath& operator=(MelMath&&) = delete;
};


#endif  // MELSPECTRAL_H_INCLUDED
