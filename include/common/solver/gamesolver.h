#pragma once

#include "common/game.h"
#include "common/solver/kissatsolver.h"

namespace Crew {


class GameSolver {
public:
	struct Solution {
		std::vector<std::vector<Card>> hands;
		std::vector<std::vector<Card>> objectives;
	};

	// param redistribute signifies wether the sat solver is allowed to redistribute
	// the objectives over the players
	GameSolver(Game game, bool redistribute = false, bool verbose = false);
	bool winnable() const;

	const Solution& getSolution() const;
	int w(unsigned t) const;
	int r(unsigned t) const;

private:
	struct CardInfo {
		int player;
		int index;
		int trick;
		Card card;
	};

	struct ObjectiveInfo {
		int player;
		Card card;
	};

	struct CardQI {
		uint q, i;
	};

	Game m_game;
	KissatSolver m_solver;
	bool m_winnable, m_redistribute;
	mutable Solution m_solution;

	int x(unsigned t, unsigned i, unsigned p) const;
	int w(unsigned t, unsigned p) const;
	int r(unsigned t, unsigned s) const;
	int o(unsigned q, unsigned i, unsigned p) const;
	int xMax() const;
	int wMax() const;
	int rMax() const;
	int oMax() const;
	const Card& card(unsigned i, unsigned p) const;
	CardInfo toCardInfo(int predicate) const;
	ObjectiveInfo toObjectiveInfo(int predicate) const;

	CardQI toCardQI(const Crew::Card& card);

	void addValidCardAssingment();
	void addwtp();
	void addrts();
	void addObjectives();
	void addObjectivesRedistribute();
	void addSymmetryRemoval();
};

}
