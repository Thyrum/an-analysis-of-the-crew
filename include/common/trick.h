#pragma once

#include <vector>
#include <ostream>

#include "common/card.h"

namespace Crew {


class Trick {
public:
	Trick(int nPlayers, Card card, int player);
	Trick(int nPlayers);
	Trick() = default;

	void add(Card card, int player);
	void reset();
	void reset(int nPlayers);

	bool contains(Card card) const;
	Card getHighest() const;
	int getWinner() const;
	int getSuit() const;
	bool completed() const;
	unsigned cardsPlayed() const;
	const std::vector<Card>& getCards() const;
	const Card& getCard(unsigned player) const;

private:
	int m_suit, m_winner;
	std::vector<Card> m_cards;
};

std::ostream& operator<<(std::ostream& os, const Trick& trick);

}
