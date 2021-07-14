#include "strategy/util.h"

#include <algorithm>

#include "common/util.h"
#include "strategy/set-operations.h"

namespace Crew {

std::vector<Card> getPlayable(
	const Game& game,
	unsigned player
) {
	if (
		game.currentTrick.getSuit() == -1 ||
		std::find_if(
			game.hands[player].begin(),
			game.hands[player].end(),
			[&](const Card& card) {
				return card.suit == game.currentTrick.getSuit();
			}
		) == game.hands[player].end()
	) {
		return game.hands[player];
	}

	std::vector<Card> result;
	result.reserve(game.hands[player].size());

	for (const auto& card : game.hands[player]) {
		if (card.suit == game.currentTrick.getSuit())
			result.push_back(card);
	}

	return result;
}


bigtype totalValidDistributions(
	const std::vector<Card>& Cp,
	const std::vector<Card>& Cq,
	const std::vector<Card>& Cr,
	int Hp, int Hq
) {
	//std::cout << "Cp,Cq,Cr = " << Cp.size() << ',' << Cq.size() << ',' << Cr.size() << std::endl;
	std::vector<Card> CpAndCq      = inBoth(Cp, Cq);
	std::vector<Card> CpAndCr      = inBoth(Cp, Cr);
	std::vector<Card> CqAndCr      = inBoth(Cq, Cr);
	std::vector<Card> CpAndCqAndCr = inBoth(CpAndCq, Cr);
	//std::cout << "CpAndCq = " << CpAndCq.size() << std::endl;
	//std::cout << "CpAndCr = " << CpAndCr.size() << std::endl;
	//std::cout << "CqAndCr = " << CqAndCr.size() << std::endl;
	//std::cout << "CpAndCqAndCr = " << CpAndCqAndCr.size() << std::endl;

	bigtype result = 0;
	for (unsigned i = 0; i <= notIn(CpAndCq, Cr).size(); ++i) {
		for(unsigned j = 0; j <= CpAndCqAndCr.size(); ++j) {
			for (unsigned k = 0; k <= notIn(CqAndCr, Cp).size(); ++k) {
				bigtype term = choose<bigtype>(notIn(CpAndCq, Cr).size(), i);
				term *= choose<bigtype>(CpAndCqAndCr.size(), j);
				term *= choose<bigtype>(notIn(CpAndCr, Cq).size(), Hp-i-j-notIn(Cp, inEither(Cq,Cr)).size());
				term *= choose<bigtype>(notIn(CqAndCr, Cp).size(), k);
				term *= choose<bigtype>(CpAndCqAndCr.size()-j, Hq-notIn(CpAndCq,Cr).size()+i-k);
				result += term;
			}
		}
	}
	return result;
}

double probHasCard(
	std::vector<Card> Cp,
	std::vector<Card> Cq,
	std::vector<Card> Cr,
	int Hp, int Hq,
	const Card& card
) {
	if (std::find(Cp.begin(), Cp.end(), card) == Cp.end())
		return 0;

	auto totalValidDistr = totalValidDistributions(Cp, Cq, Cr, Hp, Hq);

	removeElement(Cq, card);
	removeElement(Cr, card);

	//std::cout << "Cp: " << Cp << "  Cq: " << Cq << "  Cr: " << Cr << std::endl;
	//std::cout << "Hp: " << Hp << "  Hq: " << Hq << std::endl;

	auto newValidDistr = totalValidDistributions(Cp, Cq, Cr, Hp, Hq);

	//std::cout << newValidDistr << "/" << totalValidDistr << " = " << (newValidDistr/totalValidDistr).convert_to<double>()  << std::endl;

	return (newValidDistr/totalValidDistr);
}

}
