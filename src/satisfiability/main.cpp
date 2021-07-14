#include <iostream>

#include "common/card.h"
#include "common/game.h"

#include <sstream>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <getopt.h>


#include "common/solver/gamesolver.h"
#include "common/parse.h"
#include "common/solver/brute-force.h"


// Non-unique parameters
bool redistribute_flag = false;
unsigned long playouts = 10000;

// Parameters unique to SOLVE
bool verbose_flag = false;
std::string gamefile;

// Parameters unique to BRUTE_FORCE
unsigned int players;
unsigned int suits;
unsigned int suitCards;
unsigned int trumpCards;
unsigned int objectives;

// Parameters unique to EXPERIMENTS
unsigned startAt = 1;
const unsigned MAX_OBJECTIVES = 36;

void usage(const char* name) {
	std::cerr << "Usage:\n\
    " << name << " solve ((-g|--game) <gamefile>) [-v|--verbose] [-r|--redistribute]\n\
        -g <gamefile>   Specify game to solve, REQUIRED\n\
        -v              Print CNF to stderr so that it can be saved using '2>FILE.cnf'\n\
        -r              Allow objectives to be redistributed amongst the players\n\
\n\
    " << name << " brute-force <num-players> <num-suits> <num-suit-cards> <num-trump-cards> <num-objectives> <games>\n\
\n\
    " << name << " experiments [(-s|--start-at) <value>] [(-P|--playouts) <value>] [-r|--redistribute]\n\
        Brute-force games with increasing amount of objectives. Status is printed to stdout\n\
        and generated data is printed to stderr.\n\
        Upon receiving SIGUSR1, more detailed status is printed to stdout.\n\
        of the current amount of objectives and the current game to stdout.\n\
        -s <number>     Amount of objectives to start at, default: 1\n\
        -r              Allow objectives to be redistributed amongst the players\n\
        -P <number>     Amount of playouts for each amount of objectives, default: 10.000" << std::endl;
}

enum class Mode {
	SOLVE, BRUTE_FORCE, EXPERIMENTS
} mode;

void parseOptions(int argc, char** argv) {
	if (argc < 2) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (std::strcmp(argv[1], "solve") == 0) { // solve variant
		mode = Mode::SOLVE;
		static struct option long_options[] = {
			{"help",         no_argument,       nullptr, 'h'},
			{"verbose",      no_argument,       nullptr, 'v'},
			{"game",         required_argument, nullptr, 'g'},
			{"redistribute", no_argument,       nullptr, 'r'},
		};
		for(;;) {
			switch(getopt_long(argc, argv, "vhg:r", long_options, nullptr)) {
			case 'h':
			default :
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case 'v':
				verbose_flag = true;
				continue;
			case 'g':
				gamefile = optarg;
				continue;
			case 'r':
				redistribute_flag = true;
				continue;
			case -1:
				break;
			}
			break;
		}

		if (gamefile.empty()) {
			std::cerr << "No gamefile given" << std::endl;
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	} else if (std::strcmp(argv[1], "brute-force") == 0) { // brute-force variant
		mode = Mode::BRUTE_FORCE;

		players    = atoi(argv[2]);
		suits      = atoi(argv[3]);
		suitCards  = atoi(argv[4]);
		trumpCards = atoi(argv[5]);
		objectives = atoi(argv[6]);
		playouts   = atoi(argv[7]);
	} else if (std::strcmp(argv[1], "experiments") == 0) { // experiments variant
		mode = Mode::EXPERIMENTS;
		static struct option long_options[] = {
			{"help",         no_argument,       nullptr, 'h'},
			{"redistribute", no_argument,       nullptr, 'r'},
			{"start-at",     required_argument, nullptr, 's'},
			{"playouts",     required_argument, nullptr, 'P'},
		};
		for(;;) {
			switch(getopt_long(argc, argv, "hrs:P:", long_options, nullptr)) {
			case 'h':
			default :
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case 'r':
				redistribute_flag = true;
				continue;
			case 's':
				startAt = std::atoi(optarg);
				if (startAt <= 0 || startAt > MAX_OBJECTIVES) {
					std::cerr << "Invalid start-at value: " << optarg << std::endl
						<< "We must have 0 < start-at <= " << MAX_OBJECTIVES << std::endl;
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				continue;
			case 'P':
				playouts = std::atoi(optarg);
				if (playouts <= 0) {
					std::cerr << "Invalid playout count: " << optarg << std::endl
						<< "We must have playouts>0 " << MAX_OBJECTIVES << std::endl;
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				continue;
			case -1:
				break;
			}
			break;
		}
	} else {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv) {
	parseOptions(argc, argv);
	std::srand(time(nullptr));

	if (mode == Mode::SOLVE) {
		std::ifstream gameStream(gamefile);
		if (!gameStream.good()) {
			std::cout << "Couldn't open file " << gamefile << std::endl;
			exit(EXIT_FAILURE);
		}

		Crew::Game game = Crew::parseGame(gameStream);
		Crew::GameSolver solver(game, redistribute_flag, verbose_flag);

		if (solver.winnable()) {
			std::cout << "SOLVABLE" << std::endl;
			std::cout << "column = trick, row = player" << std::endl;
			Crew::GameSolver::Solution solution = solver.getSolution();
			for (const auto& player : solution.hands) {
				for (const auto& card : player) {
					std::cout << ' ' << card;
				}
				std::cout << std::endl;
			}
			std::cout << "with objectives" << std::endl;
			for (const auto& player : solution.objectives) {
				for (const auto& card : player) {
					std::cout << ' ' << card;
				}
				std::cout << std::endl;
			}
		} else {
			std::cout << "UNSOLVABLE" << std::endl;
		}
	} else if (mode == Mode::BRUTE_FORCE) {
		// Brute force the thing
		unsigned long successes = Crew::BruteForce(
			players,
			suits,
			suitCards,
			trumpCards,
			objectives,
			playouts
		);
		std::cout << "Success rate: " << ((double)successes/(double)playouts)*100.0 << '%' << std::endl;
	} else if (mode == Mode::EXPERIMENTS) {
		for (unsigned objectives = startAt; objectives <= MAX_OBJECTIVES; ++objectives) {
			std::cout << "Now doing " << objectives << std::endl;
			unsigned long successes = Crew::BruteForce(
				4, // players
				5, // suits
				9, // suitCards,
				4, // trumpCards,
				objectives, // objectives,
				playouts, // playouts
				redistribute_flag, // redistribute
				true  // verbose
			);
			std::cerr << objectives << "\t" << successes << std::endl;
			std::cout << "Completed " << objectives << std::endl;
		}
	} else {
		usage(argv[0]);
		return 1;
	}

	return 0;
}
