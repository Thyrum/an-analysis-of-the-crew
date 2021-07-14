#pragma once

#include <stdint.h>
#include <vector>

extern "C" {
#include "kissat/kissat.h"

const char* kissat_signal_name(int sig);
}


using KissatLit = int;
using KissatVar = int;

enum SatValue {
	SAT_VALUE_TRUE, SAT_VALUE_FALSE, SAT_VALUE_UNKNOWN
};

class KissatSolver {
public:
	KissatSolver(bool verbose = false);
	~KissatSolver();

	friend KissatSolver& operator<<(KissatSolver& solver, KissatLit lit);

	void addClause(const std::vector<KissatLit>& clause);
	SatValue value(KissatLit lit) const;
	SatValue solve();

private:
	kissat* m_solver;
	bool m_verbose;
};

KissatSolver& operator<<(KissatSolver& solver, KissatLit lit);
