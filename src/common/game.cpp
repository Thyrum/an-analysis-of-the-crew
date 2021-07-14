#include "common/game.h"

#include <algorithm>
#include <iomanip>

namespace Crew {

Game::Game(
		std::vector<std::vector<Card>> handsIn,
		std::vector<std::vector<Card>> objectivesIn,
		unsigned int suitCount,
		unsigned int commander
	) : P(handsIn.size()), S(suitCount),
	T(std::min_element(
			handsIn.begin(),
			handsIn.end(),
			[](const std::vector<Card>& h1, const std::vector<Card>& h2) {
				return h1.size() < h2.size();
			}
	)->size()),
	HMax(std::max_element(
			handsIn.begin(),
			handsIn.end(),
			[](const std::vector<Card>& h1, const std::vector<Card>& h2) {
				return h1.size() < h2.size();
			}
	)->size()),
	c(commander),
	hands(handsIn), objectives(objectivesIn), currentTrick(handsIn.size())
{}

Game::Game() {};

unsigned int Game::H(unsigned int p) const {
	return hands.at(p).size();
}

unsigned Game::currentPlayer() const {
	return (c + currentTrick.cardsPlayed()) % P;
}

Status Game::getStatus() const {
	bool lastTrick = false;
	for (const auto& hand : hands) {
		if (hand.size() == 0) {
			lastTrick = true;
			break;
		}
	}

	for (const auto& objs : objectives) {
		if (objs.size() != 0) {
			return (lastTrick ? IMPOSSIBLE : POSSIBLE);
		}
	}
	return DONE;
}

Status Game::applyTrick() {
	if (currentTrick.completed()) {
		for (unsigned p = 0; p < P; ++p) {
			auto it = std::find_first_of(
				objectives[p].begin(), objectives[p].end(),
				currentTrick.getCards().begin(), currentTrick.getCards().end()
			);
			if (p == currentTrick.getWinner()) {
				while (it != objectives[p].end()) {
					it = objectives[p].erase(it);
					it = std::find_first_of(
						it, objectives[p].end(),
						currentTrick.getCards().begin(), currentTrick.getCards().end()
					);
				}
			} else if (it != objectives[p].end()) {
				return IMPOSSIBLE;
			}
		}
		c = currentTrick.getWinner();
		currentTrick.reset();
	}

	return getStatus();
}

Status Game::playCard(const Card& card) {
	auto it = std::find(
		hands[currentPlayer()].begin(), hands[currentPlayer()].end(), card
	);
	if (it == hands[currentPlayer()].end()) {
		std::cout << "Player " << currentPlayer() << " tried to play "
		          << card << " which he does not own" << std::endl;
		return IMPOSSIBLE;
	}
	hands[currentPlayer()].erase(it);
	currentTrick.add(card, currentPlayer());
	return applyTrick();
}

void Game::dump(std::ostream& os) const {
	os << "# ==== START GAME DUMP ====" << std::endl;
	os << P << ' ' << c << ' ' << S << std::endl;
	os << "# ====----- HANDS -----====" << std::endl;
	for (unsigned i = 0; i < hands.size(); ++i) {
		for (const auto& card : hands[i])
			os << card;
		os << std::endl;
	}
	os << "# ====-- OBJECTIVES  --====" << std::endl;
	for (unsigned i = 0; i < objectives.size(); ++i) {
		for (const auto& card : objectives[i])
			os << card;
		os << std::endl;
	}
	os << "# ====- CURRENT TRICK -====" << std::endl;
	os << "# " << currentTrick << std::endl;
	os << "# ====- END GAME DUMP -====" << std::endl;
}

}

