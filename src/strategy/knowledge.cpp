#include "strategy/knowledge.h"
#include "strategy/set-operations.h"
#include "strategy/util.h"

#include <algorithm>

namespace Crew {

void Knowledge::reset(const Game& game, unsigned player_input) {
	supersets.clear();
	handSizes.clear();
	player_us = player_input;
	supersets = std::vector<std::vector<Card>>(game.P);
	handSizes = std::vector<int>(game.P);

	for (unsigned i = 0; i < game.P; ++i) {
		handSizes[i] = game.H(i);
		if (i == player_us) {
			supersets[i] = game.hands[i];
		}
		else {
			for (unsigned j = 0; j < game.P; ++j) {
				if (j == player_us)
					continue;
				supersets[j].insert(supersets[j].end(), game.hands[i].begin(), game.hands[i].end());
			}
		}
	}
}

void Knowledge::addTrick(const Trick& trick) {
	for (unsigned i = 0; i < trick.getCards().size(); ++i) {
		if (trick.getCard(i) != Card(-1, -1))
			playerPlayedCard(i, trick.getCard(i), trick.getSuit());
	}
}

void Knowledge::playerPlayedCard(int player, const Card& card, int requestedSuit) {
	// We already knew this player played this card
	if (
		std::find(
			supersets[player].begin(),
			supersets[player].end(),
			card
		) == supersets[player].end()
	)
		return;

	if (card.suit != requestedSuit)
		playerRemoveSuit(player, requestedSuit);

	for (unsigned p = 0; p < supersets.size(); ++p)
		playerRemoveCard(p, card);

	handSizes[player] -= 1;
}

bool Knowledge::playerCanHaveSuit(int player, int suit) const {
	return std::find_if(
		supersets.at(player).begin(),
		supersets[player].end(),
		[suit](const Card& card) {
			return card.suit == suit;
		}
	) != supersets[player].end();
}

bool Knowledge::playerCanHaveCard(int player, const Card& card) const {
	return std::find(
		supersets.at(player).begin(),
		supersets[player].end(),
		card
	) != supersets[player].end();
}

void Knowledge::playerRemoveSuit(int player, int suit) {
	auto it = std::remove_if(
		supersets.at(player).begin(),
		supersets[player].end(),
		[suit](const Card& card) {
			return card.suit == suit;
		}
	);
	auto r = std::distance(it, supersets[player].end());
	supersets[player].erase(it, supersets[player].end());
}

void Knowledge::playerRemoveCards(int player, const CardSet& cards) {
	auto it = std::remove_if(
		supersets.at(player).begin(),
		supersets[player].end(),
		[&cards](const Card& card) {
			return std::find(cards.begin(), cards.end(), card) != cards.end();
		}
	);
	auto r = std::distance(it, supersets[player].end());
	supersets[player].erase(it, supersets[player].end());
}

void Knowledge::playerRemoveCard(int player, const Card& card) {
	for (auto it = supersets.at(player).begin(); it != supersets[player].end(); ++it) {
		if (*it == card) {
			supersets[player].erase(it);
			break;
		}
	}
}

double Knowledge::probHasCard(
	unsigned player_them,
	const Card& card
) const {
	if (
		std::find(
			supersets[player_them].begin(),
			supersets[player_them].end(),
			card
		) == supersets[player_them].end()
	)
		return 0;

	if (supersets.size() == 4) {
		unsigned q = 0;
		while (q == player_us || q == player_them) ++q;
		unsigned r = q+1;
		while (r == player_us || r == player_them) ++r;
		//std::cout << "pqr = " << player_them << q << r << std::endl;

		return ::Crew::probHasCard(supersets[player_them], supersets[q], supersets[r], handSizes[player_them], handSizes[q], card);
	} else {
		return -1;
	}
}

int Knowledge::assignCardToPlayer(
	const Card& card
) const {
	// probabilities[p] is the probability of a player i<=p getting the card
	// where payer `player_us` is skipped.
	std::vector<double> probabilities(supersets.size()-1);
	probabilities[supersets.size()-2] = 1;
	for (unsigned p = 0; p < supersets.size()-1; ++p) {
		if (p == player_us)
			continue;

		unsigned adjusted_p = (p > player_us ? p - 1 : p);
		if (adjusted_p == supersets.size()-2)
			continue;
		probabilities[adjusted_p] = probHasCard(p, card);
		if (adjusted_p != 0)
			probabilities[adjusted_p] += probabilities[adjusted_p-1];
	}
	double random_value = ((double) rand() / (RAND_MAX));
	int result = 0;
	//std::cout << "PROB: " << probabilities << " with value " << random_value << std::endl;
	while (random_value > probabilities[result]) result++;
	return (result >= player_us ? result + 1 : result);
}

void Knowledge::givePlayerCard(int player, const Card& card) {
	for (unsigned p = 0; p < supersets.size(); ++p) {
		playerRemoveCard(p, card);
	}
	handSizes[player] -= 1;
}

std::vector<std::vector<Card>> Knowledge::generateHands() {
	unsigned player = (player_us == 0 ? 1 : 0);
	auto cards = supersets[player];
	while (++player < supersets.size()) {
		if (player == player_us)
			continue;
		cards = inEither(cards, supersets[player]);
	}
	Knowledge cpy = *this;
	std::vector<std::vector<Card>> result(supersets.size());
	for (const auto& card : cards) {
		unsigned p = cpy.assignCardToPlayer(card);
		cpy.givePlayerCard(p, card);
		result[p].push_back(card);
	}
	result[player_us] = supersets[player_us];
	return result;
}

}
