#pragma once

#include "card.h"

namespace Crew {

struct Hint {
	unsigned int turn;
	unsigned char player;
	Card card;

	enum Type {
		HIGHEST, ONLY, LOWEST
	} type;
};

}
