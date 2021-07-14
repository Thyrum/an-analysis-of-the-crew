#pragma once

#include <chrono>

namespace Crew {

namespace Player {

class PlayoutLimit {
public:
	enum Type {
		TIME, COUNT
	};

	PlayoutLimit(Type type, unsigned value);
	bool reached(int playout);
	void start();

private:
	std::chrono::milliseconds::rep m_startTime;
	unsigned m_value;
	Type m_type;
};

}
}
