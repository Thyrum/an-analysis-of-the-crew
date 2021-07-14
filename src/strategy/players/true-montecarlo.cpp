#include "strategy/players/true-montecarlo.h"
#include "strategy/playout.h"
#include "strategy/players/random.h"
#include "strategy/util.h"

#include <cfloat>
#include <chrono>

namespace Crew {

namespace Player {

TrueMC::TrueMC(PlayoutLimit limit) : m_limit(limit) {}

void TrueMC::init(const Game& game, unsigned player) {
	m_know.reset(game, player);
}

void TrueMC::reset(const Game& newGame, unsigned player) {
	std::cout << "reset TrueMC player" << std::endl;
	m_know.reset(newGame, player);
}

void TrueMC::addTrick(const Game&, const Trick& trick) {
	m_know.addTrick(trick);
}

Card TrueMC::getMove(const Game& game, unsigned player) {
	Playout::PlayerList randomPlayers(game.P, std::make_shared<Random>());

	m_know.addTrick(game.currentTrick);

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
		//std::cout << "TrueMC player " << player << " trying card " << card << std::endl;
		int missionsLeft = 0;
		int rounds = 0;

		m_limit.start();
		for (int i = 0;; ++i) {
			Game newGame = game;
			newGame.hands = m_know.generateHands();
			Status status = newGame.playCard(card);

			if (status == POSSIBLE) {
				Playout p(newGame, randomPlayers, false);
				p.play();
				for (const auto& objs : p.getGame().objectives)
					missionsLeft += objs.size();
				rounds += p.getRoundNumber();
			} else if (status == IMPOSSIBLE) {
				for (const auto& objs : newGame.objectives)
					missionsLeft += objs.size();
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
	}

	return bestCard;
}

int TrueMC::chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player) {
	if (objectives.size() == 1)
		return 0;

	Playout::PlayerList randomPlayers(game.P, std::make_shared<Random>());

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
			newGame.hands = m_know.generateHands();
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

}
}
