#include <common/game-util.h>

#include <algorithm>

namespace Crew {

// Shuffle hand cards and randomly redistribute just as many objectives as
// many objectives as there were before.
void randomize(Game& game) {
	size_t objectiveCount = 0;
	for (const auto& objectives : game.objectives)
		objectiveCount += objectives.size();

	randomize(game, objectiveCount);
}

// Shuffle hand cards and randomly distribute 'objectiveCount' objectives.
// Also assigns the highest trump card to player 0 for easy commander selection.
void randomize(Game& game, size_t objectiveCount) {
	// Get the cards vector
	std::vector<Card> cards;
	cards.reserve(game.P * game.hands.at(0).size());
	for (auto& hand : game.hands) {
		cards.insert(cards.end(), hand.begin(), hand.end());
		hand.clear();
	}
	// Distribute the hand cards
	std::random_shuffle(cards.begin(), cards.end());

	// Let the highest trump card be the first card to eliminate symmetry in
	// starting player
	auto highest = std::max_element(
		cards.begin(),
		cards.end(),
		[](const Card& c1, const Card& c2) {
			return c1.lt(c2, 0);
		}
	);
	std::iter_swap(cards.begin(), highest);

	unsigned int handCards = cards.size() / game.P;
	for (unsigned int p = 0; p < game.P; ++p) {
		game.hands[p].insert(
			game.hands[p].end(),
			cards.begin()+p*handCards,
			cards.begin()+(p+1)*handCards
		);
	}
	for (unsigned int c = game.P * handCards; c < cards.size(); ++c) {
		game.hands[c-game.P*handCards].push_back(cards[c]);
	}

	game.T = handCards;
	if (game.P * handCards == cards.size())
		game.HMax = handCards;
	else
		game.HMax = handCards+1;

	// Distribute the mission cards
	std::random_shuffle(cards.begin(), cards.end());

	for (auto& objectives : game.objectives)
		objectives.clear();

	for (size_t i = 0; i < objectiveCount; ++i) {
		while (cards[i].suit == 0)
			cards.erase(cards.begin() + i);
		game.objectives[i%game.P].emplace_back(cards[i]);
	}
}

std::vector<std::vector<Card>> generateHands(
	unsigned int players,
	unsigned int suits,
	unsigned int suitCards,
	unsigned int trumpCards
) {
	std::vector<std::vector<Card>> hands(players, std::vector<Card>(0));

	size_t cardCount = trumpCards + (suits-1) * suitCards;
	for (unsigned int i = 0; i < cardCount; ++i) {
		if (i < trumpCards) {
			hands[i % players].emplace_back(0,i);
		} else {
			unsigned int j = i-trumpCards;
			hands[i % players].emplace_back((j/suitCards)+1,j%suitCards);
		}
	}

	return hands;
}

}
