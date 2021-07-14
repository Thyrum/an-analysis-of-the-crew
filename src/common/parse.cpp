#include "common/parse.h"

#include <iostream>
#include <sstream>
#include <algorithm>


namespace Crew {

Game parseGame(std::istream& input) {
	std::vector<std::vector<Card>> hands;
	std::vector<std::vector<Card>> objectives;

	std::string line;
	std::getline(input, line);

	while ((line.empty() || line[0] == '#') && std::getline(input, line));
	std::istringstream iss(line);

	int players, startingPlayer, suitCount;
	iss >> players >> startingPlayer >> suitCount;

	int player = 0;
	while (player < players && std::getline(input, line) ) {
		if (line.length() != 0 && line[0] == '#')
			continue;

		hands.push_back(std::vector<Card>());

		// Only take the part of the line starting at the first '['
		auto cardStart = line.find('[');
		auto cardEnd = line.find(']', cardStart+1);
		while (cardStart != std::string::npos) {
			hands[player].emplace_back(
				std::string_view(line.c_str()+cardStart, cardEnd-cardStart+1)
			);
			cardStart = line.find('[', cardEnd+1);
			cardEnd = line.find(']', cardStart+1);
		}
		++player;
	}
	player = 0;
	while (player < players && std::getline(input, line)) {
		if (line.length() != 0 && line[0] == '#')
			continue;

		objectives.push_back(std::vector<Card>());

		// Only take the part of the line starting at the first '['
		auto cardStart = line.find('[');
		auto cardEnd = line.find(']', cardStart+1);
		while (cardStart != std::string::npos) {
			objectives[player].emplace_back(
				std::string_view(line.c_str()+cardStart, cardEnd-cardStart+1)
			);
			cardStart = line.find('[', cardEnd+1);
			cardEnd = line.find(']', cardStart+1);
		}
		++player;
	}

	// Ensure every player has a (possibly empty) objectives list
	for (int i = objectives.size(); i < players; ++i)
		objectives.push_back(std::vector<Card>());

	return Game(hands, objectives, suitCount, startingPlayer);
}

}
