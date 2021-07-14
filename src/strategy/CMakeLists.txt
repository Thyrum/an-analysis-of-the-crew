set(STRATEGY_SRC ${the-crew_SOURCE_DIR}/src/strategy)
set(STRATEGY_SRC_FILES ${STRATEGY_SRC}/knowledge.cpp
                       ${STRATEGY_SRC}/main.cpp
                       ${STRATEGY_SRC}/playout-limit.cpp
                       ${STRATEGY_SRC}/players/baseplayer.cpp
                       ${STRATEGY_SRC}/players/montecarlo.cpp
                       ${STRATEGY_SRC}/players/random.cpp
                       ${STRATEGY_SRC}/players/true-montecarlo.cpp
                       ${STRATEGY_SRC}/players/ignorant-montecarlo.cpp
                       ${STRATEGY_SRC}/players/user.cpp
                       ${STRATEGY_SRC}/playout.cpp
                       ${STRATEGY_SRC}/util.cpp)

find_package(Boost OPTIONAL_COMPONENTS math)

include_directories(${the-crew_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
add_executable(strategy ${STRATEGY_SRC_FILES})
target_link_libraries(strategy common quadmath GMP::gmpxx ${Boost_LIBRARIES})

if (Boost_FOUND)
	target_compile_definitions(strategy PRIVATE HAS_BOOST)
endif()

add_custom_target(runstrategy
	COMMAND strategy
	DEPENDS strategy
	WORKING_DIRECTORY ${the-crew_SOURCE_DIR}
)
