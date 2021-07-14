#include "strategy/players/user.h"

#include <iostream>

namespace Crew {

namespace Player {

Card User::getMove(const Game& game, unsigned player) {
	std::cout << std::endl;
	std::cout << "====USER " << player << " TURN====" << std::endl;
	game.dump(std::cout);
	std::cout << "==== CURRENT TRICK ====" << std::endl;
	std::cout << game.currentTrick << std::endl;

	int suit, value;
	std::cout << "Which suit should be played? ";
	std::cin >> suit;
	std::cout << "Which value should be played? ";
	std::cin >> value;
	std::cout << "====END USER " << player << " TURN====" << std::endl << std::endl;

	return { suit, value };
}

int User::chooseTask(const Game& game, const std::vector<Card>& objectives, unsigned player) {
	std::cout << std::endl;
	std::cout << "====USER " << player << " OBJECTIVE CHOOSE====" << std::endl;
	game.dump(std::cout);

	int task;
	std::cout << "Available tasks:" << std::endl;
	for (unsigned int i = 0; i < objectives.size(); ++i) {
		std::cout << i << ": " << objectives[i] << std::endl;
	}
	std::cout << "Which task do you choose? ";
	std::cin >> task;
	std::cout << "====END USER " << player << " TASK CHOOSE====" << std::endl << std::endl;
	return task;
}

}

}
