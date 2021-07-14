set(SATISFIABILITY_SRC ${the-crew_SOURCE_DIR}/src/satisfiability)
set(SATISFIABILITY_SRC_FILES ${SATISFIABILITY_SRC}/main.cpp)

include_directories(${the-crew_SOURCE_DIR}/include)
add_executable(satisfiability ${SATISFIABILITY_SRC_FILES})
target_link_libraries(satisfiability common GMP::gmpxx)

add_custom_target(runsatisfiability
	COMMAND satisfiability
	DEPENDS satisfiability
	WORKING_DIRECTORY ${the-crew_SOURCE_DIR}
)
