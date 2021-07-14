#include "common/trick.h"

#include <algorithm>
#include <csignal>

namespace Crew {

Trick::Trick(int nPlayers, Card card, int player)
: m_suit(card.suit), m_winner(player), m_cards(nPlayers, {-1,-1}) {
	m_cards[player] = card;
}

Trick::Trick(int nPlayers) : m_suit(-1), m_winner(-1), m_cards(nPlayers, {-1, -1}) {
}

void Trick::add(Card card, int player) {
	m_cards.at(player) = card;

	if (m_suit == -1) {
		m_suit = card.suit;
		m_winner = player;
	} else if (card.gt(getHighest(), m_suit)) {
		m_winner = player;
	}
}

void Trick::reset() {
	std::fill(
		m_cards.begin(),
		m_cards.end(),
		Card(-1, -1)
	);
	m_suit = -1;
	m_winner = -1;
}

void Trick::reset(int nPlayers) {
	m_cards = std::vector<Card>(nPlayers, Card(-1, -1));
	m_suit = -1;
	m_winner = -1;
}

bool Trick::contains(Card card) const {
	return std::find(m_cards.begin(), m_cards.end(), card) != m_cards.end();
}

Card Trick::getHighest() const {
	return (m_winner == -1 ? m_cards[0] : m_cards[m_winner]);
}

int Trick::getWinner() const {
	return m_winner;
}

int Trick::getSuit() const {
	return m_suit;
}

bool Trick::completed() const {
	return std::find(m_cards.begin(), m_cards.end(), Card(-1, -1)) == m_cards.end();
}

unsigned Trick::cardsPlayed() const {
	return m_cards.size() - std::count(m_cards.begin(), m_cards.end(), Card(-1, -1));
}

const std::vector<Card>& Trick::getCards() const {
	return m_cards;
}

const Card& Trick::getCard(unsigned player) const {
	return m_cards.at(player);
}

std::ostream& operator<<(std::ostream& os, const Trick& trick) {
	if (trick.cardsPlayed() == 0) {
		os << "[]";
	}
	else {
		os << '[' << static_cast<int>(trick.getSuit()) << ',';
		for (const auto& card : trick.getCards()) {
			if (card.suit != -1)
				os << card;
		}
		os << ']';
	}
	return os;
}

}
