An Analysis of the Cooperative Card Game The Crew
=================================================
This is the code repository for the Bachelor Thesis "An Anlysis of the
Cooperative Card Game The Crew", written by Aron de Jong.
This thesis was written as part of the bachelor program of Mathematics and
Computer Science at Leiden University under supervision of Mark van den Bergh
(MI) and Walter Kosters (LIACS).

Building
--------

We use `cmake` to generate our buildsystem. The project can be built using
```sh
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
or by running `make`, using the provided `Makefile` to automate the process.
Generated executables are put into the `bin` folder.

Running
-------
Two executables are provided, `satisfiability` and `strategy`. For more
documentation on the different executables, run them without arguments to print
the usage.

- **satisfiability**  
  Used to solve provided games and to determine the percentage of games that is
  solvable using the brute-force method.
- **strategy**  
  Used to play games of The Crew. A game can be provided or randomly generated.
  Also capable of simulating many games in order to test which agent is better.

Games
-----
A few example games are provided in the `games` folder. Note that even games
where cards can be redistributed start with distributed objectives, the
program will then redistribute these objectives, keeping the amount of
objectives per player the same.

The first line of a game file must specify `PLAYER COUNT`, `STARTING PLAYER` and
`SUIT COUNT` (in that order) separated by whitespace. Players are zero-indexed.
Next, `PLAYER COUNT` lines with lists of cards specify the hands of the players
in order. A card is `[SUIT, VALUE]` and the trump suit is always suit `0`. The
next `PLAYER COUNT` lines list the objectives of the players in order. Comments
start with a `#`. Below is an example of a game.
```
# PLAYER COUNT, STARTING PLAYER, SUIT COUNT
2 0 2
# HANDS
[1,2] [0,1]
[0,2] [1,0]
# OBJECTIVES
[1,2]
[0,1]
```
