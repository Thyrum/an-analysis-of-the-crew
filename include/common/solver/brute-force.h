#pragma once


#include "common/game.h"

namespace Crew {

unsigned long BruteForce(
	unsigned int players,
	unsigned int suits,
	unsigned int suitCards,
	unsigned int trumpCards,
	unsigned int objectives,
	unsigned long playouts,
	bool redistribute = false,
	bool verbose = true
);

}
