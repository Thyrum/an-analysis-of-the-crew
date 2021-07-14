#include "common/solver/brute-force.h"

#include <iostream>
#include <csignal>

#include "common/solver/gamesolver.h"
#include "common/game-util.h"

namespace Crew {

Game currentGame;
int playout = 0;
unsigned long successes = 0;

void signalHandler(int) {
	std::cout << std::endl << std::endl;
	std::cout << "Working on playout: " << playout << std::endl;
	std::cout << "Successes so far:   " << successes << std::endl;
	currentGame.dump(std::cout);
	std::cout << std::endl << std::endl;
}

unsigned long BruteForce(
	unsigned int players,
	unsigned int suits,
	unsigned int suitCards,
	unsigned int trumpCards,
	unsigned int objectives,
	unsigned long playouts,
	bool redistribute,
	bool verbose
) {
	std::signal(SIGUSR1, signalHandler);

	auto hands = generateHands(players, suits, suitCards, trumpCards);
	currentGame = Game(hands, std::vector<std::vector<Card>>(players, std::vector<Card>()), suits, 0);

	const static unsigned long MAX_BAR_LENGTH = 20;
	unsigned int BAR_LENGTH = std::min(MAX_BAR_LENGTH, playouts);
	successes = 0;
	if (verbose) {
		for (unsigned int i = 0; i < BAR_LENGTH; ++i)
			std::cout << '_';
		std::cout << std::endl;
	}
	for (playout = 0; playout < playouts; ++playout) {
		randomize(currentGame, objectives);
		GameSolver solver(currentGame, redistribute);
		if (solver.winnable()) {
			successes++;
		}
		if (verbose && (playout+1) % (playouts/BAR_LENGTH) == 0) {
			std::cout << '#' << std::flush;
		}
	}
	if (verbose)
		std::cout << std::endl;
	std::signal(SIGUSR1, SIG_IGN);
	return successes;
}

}
