#include "strategy/playout.h"

#include <cassert>

#include "common/util.h"
#include "strategy/util.h"

#include <csignal>

namespace Crew {

Playout::Playout(Game game, PlayerList players, bool chooseTasks)
	: m_players(players), m_game(game), m_status(DONE), m_round(0)
{
	assert(m_players.size() == m_game.hands.size());
	for (unsigned i = 0; i < m_players.size(); ++i)
		m_players[i]->init(game, i);
	for (const auto& objs : m_game.objectives) {
		if (objs.size() != 0) {
			m_status = POSSIBLE;
			break;
		}
	}
	if (chooseTasks) {
		std::vector<Card> objectives;
		for (auto& objs : m_game.objectives) {
			objectives.insert(objectives.end(), objs.begin(), objs.end());
			objs.clear();
		}
		chooseObjectives(m_game, players, objectives, m_game.c);
	}
}

Status Playout::doTurn() {
	unsigned player = m_game.currentPlayer();
	auto card = m_players[player]->getMove(m_game, player);
	auto it = std::find(m_game.hands[player].begin(), m_game.hands[player].end(), card);
	if (it == m_game.hands[player].end()) {
		std::cerr << "Player " << player << " tried to play card " << card << " which he does not own" << std::endl;
		return IMPOSSIBLE;
	}
	// We want to obtain the completed trick if relevant, thus we don't use `game.playCard`.
	m_game.currentTrick.add(card, player);
	m_game.hands[player].erase(it);

	// If the trick is completed, notify the players
	if (m_game.currentTrick.completed()) {
		++m_round;
		for (const auto& player : m_players)
			player->addTrick(m_game, m_game.currentTrick);
	}

	m_status = m_game.applyTrick();
	return m_status;
}

Status Playout::play() {
	if (m_status != POSSIBLE)
		return m_status;
	while ((m_status = doTurn()) == POSSIBLE);
	return m_status;
}

Status Playout::getStatus() const {
	return m_status;
}

int Playout::getRoundNumber() const {
	return m_round;
}

const Game& Playout::getGame() const  {
	return m_game;
}

void Playout::dump(std::ostream& os) const {
	os << "==== START PLAYOUT DUMP ====" << std::endl;
	m_game.dump(os);
	os << "====- END PLAYOUT DUMP -====" << std::endl;
}

bool chooseObjectives(
	Game& game,
	const Playout::PlayerList& players,
	std::vector<Card> objectives,
	unsigned startingPlayer
) {
	auto objectiveCount = objectives.size();
	for (unsigned i = 0; i < objectiveCount; ++i) {
		unsigned p = (startingPlayer + i) % game.P;
		auto card = players[p]->chooseTask(game, objectives, p);
		if (card >= objectives.size()) {
			std::cerr << "Player " << p << " chose an invalid objective card" << std::endl;
			return false;
		}
		game.objectives[p].push_back(objectives[card]);
		objectives.erase(objectives.begin() + card);
	}
	return true;
}

}
