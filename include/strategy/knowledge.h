#pragma once

#include <vector>
#include <unordered_set>
#include <iterator>
#include <algorithm>

#include "common/card.h"
#include "common/game.h"
#include "common/util.h"
#include "common/trick.h"

namespace Crew{

struct Knowledge {
	Knowledge() = default;

	void reset(const Game& game, unsigned player);
	void addTrick(const Trick& trick);
	void playerPlayedCard(int player, const Card& card, int requestedSuit);

	std::vector<std::vector<Card>> supersets;
	std::vector<int> handSizes;
	int player_us;

	bool playerCanHaveSuit(int player, int suit) const;
	bool playerCanHaveCard(int player, const Card& card) const;

	void playerRemoveSuit(int player, int suit);
	void playerRemoveCards(int player, const CardSet& cards);

	template<typename T>
	void playerRemoveCards(int player, typename T::const_iterator begin, typename T::const_iterator end) {
		for (; begin != end; ++begin) {
			playerRemoveCard(player, *begin);
		}
	}

	template<typename T>
	void playerAddCards(int player, typename T::const_iterator begin, typename T::const_iterator end) {
		for (; begin != end; ++begin) {
			supersets.at(player).push_back(*begin);
		}
	}

	void playerRemoveCard(int player, const Card& card);

	// The probability that for a given game, these supersets will occur, where the hand of player @p player
	// is knwown
	double probHasCard(
		unsigned player_them,
		const Card& card
	) const;

	int assignCardToPlayer(
		const Card& card
	) const;

	void givePlayerCard(int player, const Card& card);

	std::vector<std::vector<Card>> generateHands();
};

}
