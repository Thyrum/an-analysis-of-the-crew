#include "strategy/playout-limit.h"

namespace Crew {

namespace Player {

PlayoutLimit::PlayoutLimit(Type type, unsigned value) 
	: m_type(type), m_value(value) {}

bool PlayoutLimit::reached(int playout) {
	if (m_type == COUNT)
		return playout >= m_value;
	else {
		using namespace std::chrono;
		auto now = duration_cast<milliseconds>(
			steady_clock::now().time_since_epoch()
		).count();
		return (now - m_startTime > m_value);
	}
}

void PlayoutLimit::start() {
	using namespace std::chrono;
	if (m_type == TIME) {
		m_startTime = duration_cast<milliseconds>(
			steady_clock::now().time_since_epoch()
		).count();
	}
}

}

}
