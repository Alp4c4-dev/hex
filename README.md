# Hex Board Game - C++ Console Edition

**Overview**
A console-based implementation of the classic strategy board game "Hex". Written entirely in C++, this project focuses on core programming logic, data structures, and algorithmic pathfinding to determine win conditions on a customizable hexagonal grid represented in the terminal.

**Features**
* **Player vs AI:** Play against an automated bot that uses Monte Carlo simulations to determine the optimal moves.
* **Customizable Board:** The grid size is completely customizable by the player at the start of the game.
* **Swap Rule:** Implements the standard Hex swap rule (pie rule) for the first move to ensure gameplay balance.
* **Win Detection Algorithm:** Implements the Union-Find (Disjoint Set) algorithm to efficiently check for a continuous path connecting the player's edges.
* **Board Representation:** Custom ASCII representation of the hexagonal grid directly in the console output.

**Tech Stack**
* Language: C++ (C++17 standard required)
* Environment: Console / Terminal

**Getting Started**
*These instructions are for macOS/Linux terminal environments.*

1. Clone the repository:
   `git clone https://github.com/Alp4c4-dev/hex.git`
2. Navigate to the project directory:
   `cd hex`
3. Compile the source code using the C++17 standard:
   `g++ -std=c++17 main.cpp -o hex_game`
4. Run the game:
   `./hex_game`

**How to Play**
Upon launching, choose your preferred board size and color (Blue or Red). Blue plays first and aims to connect the Left and Right edges. Red aims to connect the Top and Bottom edges. Input your moves using 1-based row and column coordinates when prompted.
