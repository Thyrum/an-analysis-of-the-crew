#include <iostream>
#include <fstream>

#include "common/game.h"
#include "common/game-util.h"
#include "common/card.h"
#include "common/parse.h"
#include "common/util.h"
#include "strategy/playout.h"
#include "strategy/players/user.h"
#include "strategy/players/baseplayer.h"
#include "strategy/players/random.h"
#include "strategy/players/montecarlo.h"
#include "strategy/players/true-montecarlo.h"
#include "strategy/players/ignorant-montecarlo.h"
#include "strategy/playout-limit.h"

#include <cstring>
#include <string_view>
#include <vector>
#include <getopt.h>
#include <csignal>
#include <functional>

using namespace Crew::Player;
using namespace Crew;

Game game;
Playout::PlayerList playerList;

bool redistribute_flag = false;

// For experiments
unsigned int players     = 4;
unsigned int suits       = 5;
unsigned int suitCards   = 9;
unsigned int trumpCards  = 4;
unsigned int playouts    = 100;
const char* playerString = "itm";

const unsigned MAX_OBJECTIVES = 36;
unsigned startAt = 1;
unsigned run = 0;
unsigned currentPlayer = 0;
std::vector<int> objsLeft;

PlayoutLimit defaultPl(PlayoutLimit::TIME, 100);

void usage(const char* name) {
	std::cerr << "Usage:\n\
    " << name << " play ((-g|--game) <gamefile>) ((-p|--players) <playerstring>)\n\
        -g <gamefile>      Specify the game to play, REQUIRED\n\
        -p <playerstring>  Comma separated list of players (in order), REQUIRED\n\
                           u = User; r = Random; m = MC; i = IgnorantMC; t = TrueMC\n\
                           Monte Carlo players (m, t and i) can be followed by 'p<playouts>' or 't<milliseconds>'\n\
                           to specify the number of playouts respectively milliseconds per valid move.\n\
        -r                 Allow objectives to be redistributed. This puts the specified objectives on one\n\
                           big 'pile' and allows the players to choose an objective in order.\n\
\n\
    " << name << " experiments [(-s|--start-at) <value>] [(-P|--playouts) <value>] [-r|--redistribute]\n\
        Randomly generate games with increasing amount of objectives and try all specified players\n\
        on them. Status is printed to stdout and generated data to stderr. Data is a tab separated list of\n\
        objective count with total amount of failed missions (over ALL games) with the given mission count.\n\
        Hence the value can be bigger than the amount of objectives.\n\
        Upon receiving SIGUSR1, more detailed status is printed to stdout.\n\
        -s <number>        Amount of objectives to start at, default: 1\n\
        -r                 Allow objectives to be redistributed amongst the players\n\
        -P <number>        Amount of playouts for each amount of objectives, default: 10.000\n\
        -p <playerstring>  Comma separated list of players (in order), REQUIRED\n\
                           u = User; r = Random; m = MC; i = IgnorantMC; t = TrueMC\n\
                           Monte Carlo players (m, t and i) can be followed by 'p<playouts>' or 't<milliseconds>'\n\
                           to specify the number of playouts respectively milliseconds per valid move." << std::endl;
}

enum class Mode {
	PLAY, EXPERIMENTS
} mode;

std::shared_ptr<BasePlayer> getPlayer(std::string_view string) {
	if (string.empty())
		return std::shared_ptr<BasePlayer>(nullptr);

	PlayoutLimit pl = defaultPl;
	if (string.size() != 1) {
		PlayoutLimit::Type type;
		switch (string[1]) {
		case 'p':
			type = PlayoutLimit::COUNT;
			break;
		case 't':
			type = PlayoutLimit::TIME;
			break;
		default:
			return std::shared_ptr<BasePlayer>(nullptr);
		}
		unsigned value = toInt(string.begin()+2, string.end());
		if (value == 0)
			return std::shared_ptr<BasePlayer>(nullptr);
		pl = PlayoutLimit(type, value);
	}
	switch (string[0]) {
	case 'u':
		return std::shared_ptr<User>(new User);
	case 'r':
		return std::shared_ptr<Random>(new Random);
	case 't':
		return std::shared_ptr<TrueMC>(new TrueMC(pl));
	case 'i':
		return std::shared_ptr<IgnorantMC>(new IgnorantMC(pl));
	case 'm':
		return std::shared_ptr<MC>(new MC(pl));
	default:
		return std::shared_ptr<BasePlayer>(nullptr);
	}
}

void forEachPlayer(std::string_view list, std::function<void(std::string_view player)> func) {
	while (!list.empty()) {
		auto size = list.find(',');
		if (size == std::string_view::npos)
			size = list.size();
		func(std::string_view(list.data(), size));

		list.remove_prefix(size);
		if (list[0] == ',')
			list.remove_prefix(1);
	}
}

Playout::PlayerList getPlayers(std::string_view string) {
	Playout::PlayerList result;

	forEachPlayer(string, [&](std::string_view player) {
		result.push_back(getPlayer(player));
	});

	if (std::find(result.begin(), result.end(), nullptr) != result.end())
		return Playout::PlayerList();

	return result;
}


void parseOptions(int argc, char** argv) {
	if (argc < 2) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (std::strcmp(argv[1], "play") == 0) { // Play game
		mode = Mode::PLAY;
		static struct option long_options[] = {
			{"game",         required_argument, nullptr, 'g'},
			{"players",      required_argument, nullptr, 'p'},
			{"redistribute", no_argument,       nullptr, 'r'},
		};
		for(;;) {
			switch(getopt_long(argc, argv, "hg:p:r", long_options, nullptr)) {
			case 'h':
			default :
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case 'g': {
				std::ifstream gameStream(optarg);
				if (!gameStream.good()) {
					std::cerr << "Couldn't open game file " << optarg << std::endl;
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				game = parseGame(gameStream);
				continue;
			}
			case 'p':
				playerList = getPlayers(optarg);
				if (playerList.empty()) {
					std::cerr << "Invalid player string: " << optarg << std::endl;
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				continue;
			case 'r':
				redistribute_flag = true;
				continue;
			case -1:
				break;
			}
			break;
		}
		if (game.P != playerList.size()) {
			std::cerr << "Player count mismatch, " << game.P << " required but " << playerList.size() << " given" << std::endl;
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	} else if (std::strcmp(argv[1], "experiments") == 0) { // Do experiments
		mode = Mode::EXPERIMENTS;
		static struct option long_options[] = {
			{"players",      required_argument, nullptr, 'p'},
			{"playouts",     required_argument, nullptr, 'P'},
			{"redistribute", no_argument,       nullptr, 'r'},
			{"start-at",     required_argument, nullptr, 's'},
		};
		for(;;) {
			switch(getopt_long(argc, argv, "hp:P:rs:", long_options, nullptr)) {
			case 'h':
			default :
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case 'P':
				playouts = std::atoi(optarg);
				if (playouts == 0) {
					std::cerr << "Invalid run count: " << optarg << std::endl;
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				continue;
			case 'p':
				playerString = optarg;
				continue;
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

void signalHandler(int) {
	std::cout << std::endl << std::endl;
	std::cout << "Working on run:    " << 1+run << std::endl;
	std::cout << "Working on player: " << 1+currentPlayer << std::endl;
	std::cout << "objsLeft:          " << objsLeft << std::endl;
	std::cout << std::endl << std::endl;
}

int main(int argc, char** argv) {
	std::srand(time(NULL));

	parseOptions(argc, argv);

	if (mode == Mode::PLAY) {
		Playout playout(game, playerList, redistribute_flag);
		auto result = playout.play();
		switch (result) {
		case DONE:
			std::cout << "WE DID IT!" << std::endl;
			break;
		case IMPOSSIBLE:
			std::cout << "WE FAILED HORRIBLY!" << std::endl;
			break;
		case POSSIBLE:
			std::cout << "THIS SHOULD NOT BE POSSIBLE" << std::endl;
			break;
		}
		std::cout << "FINAL GAME STATE:" << std::endl;
		playout.dump();
	} else if (mode == Mode::EXPERIMENTS) {
		std::signal(SIGUSR1, signalHandler);
		std::vector<Playout::PlayerList> playerLists;

		forEachPlayer(playerString, [&](std::string_view player) {
			playerLists.push_back(Playout::PlayerList(0));
			for (unsigned i = 0; i < players; ++i) {
				auto ptr = getPlayer(player);
				if (ptr == nullptr) {
					std::cerr << "Invalid player string: " << player << std::endl;
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				playerLists.back().push_back(ptr);
			}
		});

		for (unsigned objectives = startAt; objectives <= MAX_OBJECTIVES; ++objectives) {
			std::cout << "Now doing " << objectives << " objectives" << std::endl;

			objsLeft = std::vector<int>(playerLists.size(), 0);
			std::vector<std::vector<Card>> hands = generateHands(players, suits, suitCards, trumpCards);
			Game currentGame(hands, std::vector<std::vector<Card>>(players, std::vector<Card>()), suits, 0);

			for (run = 0; run < playouts; ++run) {
				randomize(currentGame, objectives);
				for (currentPlayer = 0; currentPlayer < playerLists.size(); ++currentPlayer) {
					//std::cout << "Doing player " << currentPlayer << std::endl;
					Playout p(currentGame, playerLists[currentPlayer], redistribute_flag);
					Status status;
					p.play();
					for (const auto& objs : p.getGame().objectives) {
						objsLeft[currentPlayer] += objs.size();
					}
				}
			}
			std::cerr << objectives;
			for (const auto objs : objsLeft)
				std::cerr << '\t' << objs;
			std::cerr << std::endl;
			std::cout << "Completed " << objectives << " objectives" << std::endl;
		}
	}
}
