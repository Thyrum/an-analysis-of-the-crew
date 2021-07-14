#pragma once

#include "common/game.h"
#include "common/card.h"
#include "common/probability/math.h"
#include "common/trick.h"
#include "common/status.h"
#include <vector>

namespace Crew {

std::vector<Card> getPlayable(
	const Game& game,
	unsigned player
);

using bigtype = double;

bigtype totalValidDistributions(
	const std::vector<Card>& Cp,
	const std::vector<Card>& Cq,
	const std::vector<Card>& Cr,
	int Hp, int Hq
);

double probHasCard(
	std::vector<Card> Cp,
	std::vector<Card> Cq,
	std::vector<Card> Cr,
	int Hp, int Hq,
	const Card& card
);

}
