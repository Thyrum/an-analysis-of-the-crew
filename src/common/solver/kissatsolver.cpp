#include "common/solver/kissatsolver.h"
#include <iostream>

extern "C" {
const char* kissat_signal_name(int) {
	return "SIGUNKNOWN";
}
}

SatValue toSatValue(int result) {
	switch (result) {
	case 10:
		return SAT_VALUE_TRUE;
	case 20:
		return SAT_VALUE_FALSE;
	default:
		return SAT_VALUE_UNKNOWN;
	}
}

SatValue toSatValueLit(KissatLit value) {
  if (value > 0) return SAT_VALUE_TRUE;
  return SAT_VALUE_FALSE;
}

KissatSolver::KissatSolver(bool verbose)
	: m_solver(kissat_init()), m_verbose(verbose) {
}

KissatSolver::~KissatSolver() {
	kissat_release(m_solver);
}

void KissatSolver::addClause(const std::vector<KissatLit>& clause) {
	for (const auto& lit : clause)
		*this << lit;

	*this << 0;
}

SatValue KissatSolver::value(KissatLit lit) const {
	return toSatValueLit(kissat_value(m_solver, lit));
}

SatValue KissatSolver::solve() {
	return toSatValue(kissat_solve(m_solver));
}

KissatSolver& operator<<(KissatSolver& solver, KissatLit lit) {
	kissat_add(solver.m_solver, lit);
	if (solver.m_verbose) {
		if (lit == 0)
			std::cerr << lit << std::endl;
		else
			std::cerr << lit << ' ';
	}
	return solver;
}
