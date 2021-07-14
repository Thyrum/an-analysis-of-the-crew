set(COMMON_SRC ${the-crew_SOURCE_DIR}/src/common)
set(COMMON_SRC_FILES ${COMMON_SRC}/card.cpp
                     ${COMMON_SRC}/game.cpp
                     ${COMMON_SRC}/util.cpp
                     ${COMMON_SRC}/game-util.cpp
                     ${COMMON_SRC}/trick.cpp
                     ${COMMON_SRC}/parse.cpp
                     ${COMMON_SRC}/probability/math.cpp
                     ${COMMON_SRC}/solver/brute-force.cpp
                     ${COMMON_SRC}/solver/gamesolver.cpp
                     ${COMMON_SRC}/solver/kissatsolver.cpp)

add_library(common STATIC ${COMMON_SRC_FILES})
target_include_directories(common PUBLIC ${the-crew_SOURCE_DIR}/include)
target_link_libraries(common Kissat)
