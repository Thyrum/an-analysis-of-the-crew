#pragma once

#include <iostream>
#include <memory>

#include "common/card.h"
#include "common/game.h"
#include "strategy/players/baseplayer.h"
#include "common/status.h"


namespace Crew {

class Playout {
public:
	using PlayerList = std::vector<std::shared_ptr<Player::BasePlayer>>;

	Playout(Game game, PlayerList players, bool chooseTasks);
	Playout(
		PlayerList players,
		const Playout& playout
	);

	Status doTurn();
	Status play();

	Status getStatus() const;
	int getRoundNumber() const;

	const Game& getGame() const;

	void dump(std::ostream& os = std::cout) const;

private:
	PlayerList m_players;
	Game m_game;
	Status m_status;
	unsigned m_round;
};

bool chooseObjectives(
	Game& game,
	const Playout::PlayerList& players,
	std::vector<Card> objectives,
	unsigned startingPlayer
);

}

