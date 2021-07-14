#include "common/card.h"

#include <charconv>
#include <algorithm>

namespace Crew {

Card::Card() {}

Card::Card(int suit, int value)
	: suit(suit), value(value) {}

Card::Card(const std::string_view str) {
	auto comma = std::find(str.begin(), str.end(), ',');
	auto start = std::find(str.begin(), str.end(), '[');
	auto end   = std::find(str.begin(), str.end(), ']');

	std::from_chars(start+1, comma, suit);
	std::from_chars(comma+1, end, value);
}


bool Card::isTrump() const {
	return suit == 0;
}

bool Card::gt(const Card& other, int playedSuit) const {
	if (isTrump()) {
		return !other.isTrump() || value > other.value;
	}
	else if (other.isTrump()) {
		return false;
	}
	else if (suit != playedSuit) {
		return false;
	}
	else {
		return other.suit != playedSuit || value > other.value;
	}
}

bool Card::lt(const Card& other, int playedSuit) const {
	if (isTrump()) {
		return other.isTrump() && other.value > value;
	}
	else if (other.isTrump()) {
		return true;
	}
	else if (other.suit != playedSuit) {
		return false;
	}
	else {
		return suit != playedSuit || value < other.value;
	}
}

bool Card::operator==(const Card& other) const {
	return (suit == other.suit && value == other.value);
}

bool Card::operator!=(const Card& other) const {
	return (suit != other.suit || value != other.value);
}

bool cardComp(const Card& c1, const Card& c2) {
	if (c1.suit == c2.suit)
		return c1.value < c2.value;
	else
		return c1.suit > c2.suit;
}

std::istream& operator>>(std::istream& is, Crew::Card card) {
	std::string cardString;
	is >> cardString;
	const char* c = cardString.c_str();
	auto res = std::from_chars(
		c + cardString.find('[') + 1,
		c + cardString.find(','),
		card.suit
	);
	res = std::from_chars(
		c + cardString.find('[') + 1,
		c + cardString.find(','),
		card.suit
	);
	return is;
}

std::ostream& operator<<(std::ostream& os, Crew::Card card) {
	os << '[' << card.suit << ',' << card.value << ']';
	return os;
}

}
