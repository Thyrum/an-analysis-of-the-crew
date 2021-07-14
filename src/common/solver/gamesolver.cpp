#include "common/solver/gamesolver.h"
#include "strategy/set-operations.h"
#include <algorithm>
#include <functional>

namespace Crew {

GameSolver::GameSolver(Game game, bool redistribute, bool verbose)
	: m_game(game),  m_solver(verbose), m_redistribute(redistribute) {
	addValidCardAssingment();
	addwtp();
	addrts();
	if (m_redistribute) {
		addObjectivesRedistribute();
	} else {
		addObjectives();
		m_solution.objectives = game.objectives;
	}

	m_winnable = (m_solver.solve() == SAT_VALUE_TRUE ? true : false);
}

bool GameSolver::winnable() const {
	return m_winnable;
}

const GameSolver::Solution& GameSolver::getSolution() const {
	// Construct the solution
	if (winnable() && m_solution.hands.empty()) {
		m_solution.hands = std::vector<std::vector<Card>>(m_game.P, std::vector<Card>(m_game.T));
		if (m_redistribute) {
			// Construct the objectives
			m_solution.objectives = std::vector<std::vector<Card>>(m_game.P, std::vector<Card>(0));
			for (KissatLit o = 1 + rMax(); o <= oMax(); ++o) {
				if (m_solver.value(o) == SAT_VALUE_TRUE) {
					ObjectiveInfo oi = toObjectiveInfo(o);
					m_solution.objectives.at(oi.player).push_back(oi.card);
				}
			}
		}

		// Construct the player hands
		for (KissatLit c = 1; c <= xMax(); ++c) {
			if (m_solver.value(c) == SAT_VALUE_TRUE) {
				CardInfo card = toCardInfo(c);
				m_solution.hands.at(card.player).at(card.trick) = card.card;
			}
		}
	}
	return m_solution;
}

int GameSolver::w(unsigned t) const {
	if (!winnable())
		return -1;

	for (unsigned p = 0; p < m_game.P; ++p) {
		if (m_solver.value(w(t,p)) == SAT_VALUE_TRUE)
			return p;
	}
	return -1;
}

int GameSolver::r(unsigned t) const {
	if (!winnable())
		return -1;

	for (unsigned s = 0; s < m_game.S; ++s) {
		if (m_solver.value(r(t,s)) == SAT_VALUE_TRUE)
			return s;
	}
	return -1;
}

int GameSolver::x(uint t, uint i, uint p) const {
	return 1 + i 
		+ (m_game.HMax * t)
		+ (m_game.HMax * m_game.T * p);
}

int GameSolver::xMax() const {
	return m_game.HMax * m_game.T * m_game.P;
}

int GameSolver::w(uint t, uint p) const {
	return 1 + xMax()
		+ p
		+ (m_game.P * t);
}

int GameSolver::wMax() const {
	return xMax() + (m_game.T * m_game.P);
}

int GameSolver::r(uint t, uint s) const {
	return 1 + wMax()
		+ s
		+ (m_game.S * t);
}

int GameSolver::rMax() const {
	return wMax() + m_game.T * m_game.S;
}

int GameSolver::o(unsigned q, unsigned i, unsigned p) const {
	return 1 + rMax()
		+ q
		+ (m_game.P * i)
		+ (m_game.P * m_game.HMax * p);
}

int GameSolver::oMax() const {
	return rMax() + m_game.P * m_game.P * m_game.HMax;
}

const Card& GameSolver::card(uint i, uint p) const {
	return m_game.hands.at(p).at(i);
}

GameSolver::CardInfo GameSolver::toCardInfo(int predicate) const {
	if (predicate < 0)
		predicate = -predicate;

	if (predicate > xMax() || predicate == 0)
		return {-1,-1,-1,{0,0}};

	--predicate;

	int player = predicate / (m_game.HMax * m_game.T);
	int index = predicate % m_game.HMax;
	int trick = (predicate % (m_game.HMax * m_game.T)) / m_game.HMax;
	return {
		player, index, trick, card(index, player)
	};
}

GameSolver::ObjectiveInfo GameSolver::toObjectiveInfo(int predicate) const {
	if (predicate < 0)
		predicate = -predicate;

	if (predicate > oMax() || predicate <= rMax())
		return {-1,{0,0}};

	predicate -= rMax() + 1;

	int q = predicate % m_game.P;
	int i = (predicate % (m_game.P * m_game.HMax)) / m_game.P;
	int p = predicate / (m_game.P * m_game.HMax);
	return {
		p, card(i,q)
	};
}

void GameSolver::addValidCardAssingment() {
	for (unsigned p = 0; p < m_game.P; ++p) {
		for (unsigned t = 0; t < m_game.T; ++t) {
			for (unsigned i = 0; i < m_game.H(p); ++i) {
				m_solver << x(t,i,p);
			}
			m_solver << 0;

			for (unsigned i = 0; i < m_game.H(p); ++i) {
				for (unsigned j = i+1; j < m_game.H(p); ++j) {
					m_solver << -x(t,i,p) << -x(t,j,p) << 0;
				}
			}
		}
	}

	for (unsigned p = 0; p < m_game.P; ++p) {
		for (unsigned i = 0; i < m_game.H(p); ++i) {
			for (unsigned t = 0; t < m_game.T; ++t) {
				for (unsigned ta = t+1; ta < m_game.T; ++ta) {
					m_solver << -x(t,i,p) << -x(ta,i,p) << 0;
				}
			}
		}
	}

	for (unsigned p = 0; p < m_game.P; ++p) {
		for (unsigned i = 0; i < m_game.H(p); ++i) {
			for (unsigned t = 0; t < m_game.T; ++t) {
				for (unsigned j = 0; j < m_game.H(p); ++j) {
					uint s = card(j,p).suit;
					if ((uint)card(i,p).suit == s)
						continue;
					m_solver << -x(t,i,p) << -r(t,s);
					for (unsigned ta = 0; ta < t; ++ta) {
						m_solver << x(ta,j,p);
					}
					m_solver << 0;
				}
			}
		}
	}
}

void GameSolver::addwtp() {
	for (uint p = 0; p < m_game.P; ++p) {
		for (uint q = 0; q < m_game.P; ++q) {
			if (p == q)
				continue;
			for (uint t = 0; t < m_game.T; ++t) {
				for (uint s = 0; s < m_game.S; ++s) {
					for (uint i = 0; i < m_game.H(p); ++i) {
						for (uint j = 0; j < m_game.H(q); ++j) {
							if (!card(i,p).lt(card(j,q),s))
								continue;
							m_solver << -w(t,p) << -r(t,s) << -x(t,i,p) << -x(t,j,q) << 0;
						}
					}
				}
			}
		}
	}

	for (uint t = 0; t < m_game.T; ++t) {
		for (uint p = 0; p < m_game.P; ++p) {
			m_solver << w(t,p);
		}
		m_solver << 0;
	}
}

void GameSolver::addrts() {
	for (uint t = 1; t < m_game.T; ++t) {
		for (uint p = 0; p < m_game.P; ++p) {
			for (uint i = 0; i < m_game.H(p); ++i) {
				m_solver << -w(t-1,p) << -x(t,i,p) << r(t,card(i,p).suit) << 0;
			}
		}
	}

	for (uint i = 0; i < m_game.H(m_game.c); ++i) {
		m_solver << -x(0,i,m_game.c) << r(0,card(i,m_game.c).suit) << 0;
	}

	for (uint t = 0; t < m_game.T; ++t) {
		for (uint s = 0; s < m_game.S; ++s) {
			for (uint sa = 0; sa < m_game.S; ++sa) {
				if (sa == s)
					continue;
				m_solver << -r(t,s) << -r(t,sa) << 0;
			}
		}
	}
}

GameSolver::CardQI GameSolver::toCardQI(const Crew::Card& card) {
	for (uint q = 0; q < m_game.P; ++q) {
		for (uint i = 0; i < m_game.hands[q].size(); ++i) {
			if (m_game.hands[q][i] == card)
				return {q, i};
		}
	}
	return {0, 0};
}

void GameSolver::addObjectives() {
	for (uint p = 0; p < m_game.P; ++p) {
		for (uint q = 0; q < m_game.P; ++q) {
			auto it = std::find_first_of(
				m_game.hands[q].begin(), m_game.hands[q].end(),
				m_game.objectives[p].begin(), m_game.objectives[p].end()
			);
			while (it != m_game.hands[q].end()) {
				uint i = it - m_game.hands[q].begin();

				for (uint t = 0; t < m_game.T; ++t) {
					m_solver << -x(t,i,q) << w(t,p) << 0;
				}

				for (uint t = 0; t < m_game.T; ++t) {
					m_solver << x(t,i,q);
				}
				m_solver << 0;

				it = std::find_first_of(
					++it, m_game.hands[q].end(),
					m_game.objectives[p].begin(), m_game.objectives[p].end()
				);
				i = it - m_game.hands[q].begin();
			}
		}
	}
}

// See following URL for why this goes through all combinations
// https://stackoverflow.com/questions/12991758/creating-all-possible-k-combinations-of-n-items-in-c
template<typename T>
void forEachCombination(
	std::vector<T> vec,
	int offset, int k,
	std::function<void(std::vector<T>)> func
	) {
	static std::vector<T> combination;
	if (k == 0) {
		func(combination);
		return;
	}
	for (int i = offset; i <= vec.size() - k; ++i) {
		combination.push_back(vec[i]);
		forEachCombination(vec, i+1, k-1, func);
		combination.pop_back();
	}
}

void GameSolver::addObjectivesRedistribute() {
	// First add the requirements for o(qip)
	std::vector<Card> objectives = m_game.objectives[0];
	for (uint i = 1; i < m_game.objectives.size(); ++i) {
		objectives.insert(
			objectives.end(),
			m_game.objectives[i].begin(),
			m_game.objectives[i].end()
		);
	}
	std::sort(objectives.begin(), objectives.end(), cardComp);

	//std::cout << "Redistribute Objectives 1" << std::endl;
	for (uint p = 0; p < m_game.P; ++p) {
		uint np = m_game.objectives[p].size();
		if (objectives.size() != np) {
			forEachCombination<Crew::Card>(
				objectives,
				0, np+1,
				[&](std::vector<Card> comb) {
				for (const auto& card : comb) {
				auto[q,i] = toCardQI(card);
				m_solver << -o(q,i,p);
				}
				m_solver << 0;
				}
			);
		}
	}

	//std::cout << "Redistribute Objectives 2" << std::endl;
	for (const auto& card : objectives) {
		for (uint p = 0; p < m_game.P; ++p) {
			auto[q,i] = toCardQI(card);
			m_solver << o(q,i,p);
		}
		m_solver << 0;
	}

	// Then add the (changed) requirements for actually obtaining the right card
	//std::cout << "Redistribute Objectives 3" << std::endl;
	for (const auto& card : objectives) {
		auto[q,i] = toCardQI(card);
		for (uint p = 0; p < m_game.P; ++p) {
			for (uint t = 0; t < m_game.T; ++t) {
				m_solver << -o(q,i,p) << -x(t,i,q) << w(t,p) << 0;
			}
		}
		for (uint t = 0; t < m_game.T; ++t) {
			m_solver << x(t,i,q);
		}
		m_solver << 0;
	}
}

void GameSolver::addSymmetryRemoval() {
}

}
