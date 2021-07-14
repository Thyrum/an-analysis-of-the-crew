#include "strategy/players/ignorant-montecarlo.h"
#include "strategy/players/random.h"
#include "strategy/util.h"
#include "strategy/playout.h"
#include "common/util.h"

#include "common/game.h"

#include <cfloat>
#include <chrono>

namespace Crew {

namespace Player {

IgnorantMC::IgnorantMC(PlayoutLimit limit) : m_limit(limit) {}

Card IgnorantMC::getMove(const Game& game, unsigned player) {
	Playout::PlayerList randomPlayers(game.P, std::make_shared<Random>());
	auto otherPlayerCards = getOtherPlayerCards(game, player);

	int missionCount = 0;
	for (const auto& objs : game.objectives) {
		missionCount += objs.size();
	}

	double leastMissionsLeft = DBL_MAX;
	double mostRoundsCompleted = 0;
	auto playables = getPlayable(game, player);

	if (playables.size() == 1)
		return playables[0];

	Card bestCard = playables[0];
	for (const auto& card : playables) {
		int missionsLeft = 0;
		int rounds = 0;

		m_limit.start();
		for (int i = 0;; ++i) {
			Game newGame = game;
			Status status = newGame.playCard(card);
			if (status == POSSIBLE) {
				newGame.hands = generateHands(game, player, otherPlayerCards);
				Playout p(newGame, randomPlayers, false);
				p.play();
				for (const auto& objs : p.getGame().objectives) {
					missionsLeft += objs.size();
				}
				rounds += p.getRoundNumber();
			} else if (status == IMPOSSIBLE) {
				for (const auto& objs : game.objectives) {
					missionsLeft += objs.size();
				}
			}
			if (m_limit.reached(i)) {
				//std::cout << "Did " << i << " playouts" << std::endl;
				double avgMissionsLeft = (double)missionsLeft / (double)i;
				double avgRoundsCompleted = (double)rounds / (double)i;
				if (avgMissionsLeft < leastMissionsLeft
				    || (avgMissionsLeft == leastMissionsLeft
				        && avgRoundsCompleted > mostRoundsCompleted)
				) {
					leastMissionsLeft = avgMissionsLeft;
					mostRoundsCompleted = avgRoundsCompleted;
					bestCard = card;
				}
				break;
			}
		}
		if (missionsLeft < leastMissionsLeft || (missionsLeft == leastMissionsLeft && rounds > mostRoundsCompleted)) {
			leastMissionsLeft = missionsLeft;
			mostRoundsCompleted = rounds;
			bestCard = card;
		}
	}

	return bestCard;
}

int IgnorantMC::chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player) {
	if (objectives.size() == 1)
		return 0;

	Playout::PlayerList randomPlayers(game.P, std::make_shared<Random>());
	auto otherPlayerCards = getOtherPlayerCards(game, player);

	int missionCount = objectives.size();
	for (const auto& objs : game.objectives) {
		missionCount += objs.size();
	}

	double leastMissionsLeft = DBL_MAX;
	int mostRoundsCompleted = 0;
	unsigned bestCard = 0;

	for (unsigned obj = 0; obj < objectives.size(); ++obj) {
		auto newObjs = objectives;
		newObjs.erase(newObjs.begin() + obj);

		m_limit.start();
		for (unsigned i = 0;; ++i) {
			auto newGame = game;
			newGame.hands = generateHands(game, player, otherPlayerCards);
			newGame.objectives[player].push_back(objectives[obj]);

			double missionsLeft = 0;
			double rounds = 0;
			chooseObjectives(newGame, randomPlayers, newObjs, (player + 1)%game.P);
			Playout p(newGame, randomPlayers, false);
			p.play();

			for (const auto& objs : p.getGame().objectives) {
				missionsLeft += objs.size();
			}
			rounds += p.getRoundNumber();
			if (m_limit.reached(i)) {
				//std::cout << "Did " << i << " playouts" << std::endl;
				double avgMissionsLeft = (double)missionsLeft / (double)i;
				double avgRoundsCompleted = (double)rounds / (double)i;
				if (avgMissionsLeft < leastMissionsLeft
				    || (avgMissionsLeft == leastMissionsLeft
				        && avgRoundsCompleted > mostRoundsCompleted)
				) {
					leastMissionsLeft = avgMissionsLeft;
					mostRoundsCompleted = avgRoundsCompleted;
					bestCard = obj;
				}
				break;
			}
		}
	}
	return bestCard;
}

std::vector<std::vector<Card>> IgnorantMC::generateHands(
	const Game& game,
	unsigned player,
	std::vector<Card> otherPlayerCards
	) {
	std::vector<std::vector<Card>> newHands(game.P, std::vector<Card>());
	std::random_shuffle(otherPlayerCards.begin(), otherPlayerCards.end());
	unsigned lastCard = 0;
	for (unsigned p = 0; p < game.P; ++p) {
		if (p == player)
			newHands[p] = game.hands[p];
		else {
			newHands[p].insert(
				newHands[p].end(),
				otherPlayerCards.begin() + lastCard,
				otherPlayerCards.begin() + lastCard + game.hands[p].size()
			);
			lastCard += game.hands[p].size();
		}
	}
	return newHands;
}

std::vector<Card> IgnorantMC::getOtherPlayerCards(
	const Game& game,
	unsigned player
) {
	std::vector<Card> otherPlayerCards;
	for (unsigned p = 0; p < game.P; ++p) {
		if (p == player)
			continue;
		otherPlayerCards.insert(otherPlayerCards.end(), game.hands[p].begin(), game.hands[p].end());
	}
	return otherPlayerCards;
}


}

}
