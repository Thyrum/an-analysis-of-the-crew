#pragma once

#include <ostream>
#include <string_view>
#include <vector>

namespace Crew {

static const char BLACK  = 0;
static const char YELLOW = 1;
static const char GREEN  = 2;
static const char PINK   = 3;
static const char BLUE   = 4;


struct Card {
	Card();
	Card(int suit, int value);
	Card(const std::string_view str);

	int suit;
	int value;

	bool isTrump() const;

	bool gt(const Card& other, int playedSuit) const;
	bool lt(const Card& other, int playedSuit) const;

	bool operator==(const Card& other) const;
	bool operator!=(const Card& other) const;
};


using CardSet = std::vector<Card>;

// Function used to create an absolute ordening on the cards
bool cardComp(const Card& c1, const Card& c2);

std::istream& operator>>(std::istream& is, Crew::Card card);
std::ostream& operator<<(std::ostream& os, Crew::Card card);


}

namespace std {
  template <>
  struct hash<Crew::Card> {
    std::size_t operator()(const Crew::Card& c) const {
      using std::hash;
      return (hash<int>()(c.suit) ^ (hash<int>()(c.value) << 1));
    }
  };
}
