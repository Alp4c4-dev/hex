// Make a playable version of HEX game
// Variable board size represented with ASCII symbols
// Player chooses color (blue goes first, red goes second)
// Red player can always choose to steal first move - Swap rule

#include <iomanip>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <chrono>
#include <random>
#include <unordered_set>
#include <algorithm>

using namespace std;

// a node in the graph
struct Node
{
    // 'B' for Blue, 'R' for Red, '.' for empty
    char value;
    
    // coordinates on the board
    int x, y;
    
    // pointers to neighboring nodes
    vector<Node*> neighbors;
    
    // constructor
    Node(): value('.'), x(-1), y(-1) {}
    
};

// function to add neighbors
void addEdge(Node* a, Node* b)
{
    a -> neighbors.push_back(b);
    b -> neighbors.push_back(a);
}

// draws the board for a given dimension
void drawBoard(const vector<vector<Node>> board, int size)
{
    int row = 0;
    
    // draw column indexes
    cout << " " << 1;
    for (int i = 2; i <= size; ++i)
        cout << "   " << i;
    cout << endl;
    
    // draw first row index
    cout << 1 << "  " << board[row][0].value;
    
    // draw first row
    for (int i = 1; i < size; ++i)
        cout << " _ " << board[row][i].value;
    cout << endl;
    
    // draw following lines
    while (row < (size * 2) - 2)
    {
        row++;
        
        // odd rows are made of diagonal connectors
        if(row % 2 != 0)
        {
            // initial spaces, increasing with the number of rows
            for (int i = 0; i < row; ++i)
                cout << " ";
            
            // actual row
            cout << "   ";
            for (int i = 0; i < size - 1; ++i)
                cout << "\\ / ";
            cout << "\\";
            cout << endl;
        }
        // even rows are made of nodes and connectors
        else
        {
            // initial spaces, increasing with the number of rows
            for (int i = 0; i < row; ++i)
                cout << " ";
            
            // draw row index
            if (((row / 2) + 1) / 10 == 0)
                cout << (row / 2) + 1 << " ";
            else
                // with double digits indexes, remove the space to avoid misalignment
                cout << (row / 2) + 1;
            
            // actual row
            cout << " " << board[row / 2][0].value;
            for (int i = 1; i < size; ++i)
                cout << " _ " << board[row / 2][i].value;
            cout << endl;
        }
    }
    cout << endl;
}

// initialize the board with empty spaces
void initializeBoard(vector<vector<Node>>& board, int size)
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            board[i][j].value = '.';
            board[i][j].x = i;
            board[i][j].y = j;
            
            // connect neighbors
            // check if the node isn't on the top row
            if (i > 0)
                addEdge(&board[i][j], &board[i-1][j]);
            // check if the node isn't on the bottom row
            if (i < size - 1)
                addEdge(&board[i][j], &board[i + 1][j]);
            // check if the node isn't on the left column
            if (j > 0)
                addEdge(&board[i][j], &board[i][j - 1]);
            // check if the node isn't on the right column
            if (j < size - 1)
                addEdge(&board[i][j], &board[i][j + 1]);
            // check if the node isn't on the top row and right column
            if (i > 0 && j < size -1)
                addEdge(&board[i][j], &board[i - 1][j + 1]);
            // check if the node isn't on the bottom row and left column
            if (i < size - 1 && j > 0)
                addEdge(&board[i][j], &board[i + 1][j - 1]);
        }
    }
}

// implementing Union-Find for win condition
class UnionFind
{
public:
    vector<int> parent, rank;
    
    // constructor
    UnionFind(int n)
    {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; ++i)
            parent[i] = i;
    }
    
    int find(int x)
    {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        
        return parent[x];
    }
    
    void unite(int x, int y)
    {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY)
        {
            if (rank[rootX] > rank[rootY])
                parent[rootY] = rootX;
            else if(rank[rootX] < rank[rootY])
                parent[rootX] = rootY;
            else
            {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }
};

int cellIndex(int row, int col, int size)
{
    return row * size + col;
}

// implementing union-find to check win condition
bool checkWinUnionFind(const vector<vector<Node>>& board, const int size, const char player)
{
    int totalCells = size * size;
    
    // extra nodes for virtual left-right or top-bottom
    UnionFind uf(totalCells + 2);
    
    int virtualTop = totalCells;
    int virtualBottom = totalCells + 1;
    int virtualLeft = totalCells;
    int virtualRight = totalCells + 1;
    
    // directions for all six neighbors in a hex grid
    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {1, -1}};
    
    // initialize union-find with current board state
    for (int row = 0; row < size; ++row)
    {
        for (int col = 0; col < size; ++col)
        {
            if (board[row][col].value == player)
            {
                int currentIndex = cellIndex(row, col, size);
                
                // connect to virtual nodes
                if (player == 'R')
                {
                    if (row == 0)
                        uf.unite(currentIndex, virtualTop);
                    if (row == size -1)
                        uf.unite(currentIndex, virtualBottom);
                }
                else if (player == 'B')
                {
                    if (col == 0)
                        uf.unite(currentIndex, virtualLeft);
                    if (col == size -1)
                        uf.unite(currentIndex, virtualRight);
                }
                
                // connect to neighbors
                for (const auto& [dx, dy] : directions)
                {
                    int newRow = row + dx;
                    int newCol = col + dy;
                    if (newRow >= 0 && newRow < size && newCol >= 0 && newCol < size && board[newRow][newCol].value == player)
                    {
                        int neighborIndex = cellIndex(newRow, newCol, size);
                        uf.unite(currentIndex, neighborIndex);
                    }
                }
            }
        }
    }
    
    // check if the two virtual nodes are connected for the player
    if (player == 'R')
        return uf.find(virtualTop) == uf.find(virtualBottom);
    else if (player == 'B')
        return uf.find(virtualLeft) == uf.find(virtualRight);
    
    return false;
}

// simulates a random game, returns true if player wins
bool simulateRandomGame(vector<vector<Node>>& board, const int size, char player, char opponent)
{
    // random engine for move selection
    default_random_engine rng(chrono::steady_clock::now().time_since_epoch().count());
    
    // collect all empty cells
    vector<pair<int, int>> emptyCells;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j].value == '.')
                emptyCells.push_back({i, j});

    // shuffle the empty cells
    shuffle(emptyCells.begin(), emptyCells.end(), rng);
        
    // copy the board for simulation
    auto originalBoard = board;
        
    char currentPlayer = player;

    // make moves in shuffled order
    for (auto [i, j] : emptyCells)
    {
        if (board[i][j].value == '.')
        {
            board[i][j].value = currentPlayer;
            
            // switch player
            currentPlayer = (currentPlayer == player) ? opponent : player;
        }
    }
    
    bool playerWins = checkWinUnionFind(board, size, player);

    // restore the original board after the simulation
    board = originalBoard;

    return playerWins;
}

// monte carlo simulation to determine best move
pair<int, int> monteCarloBestMove(vector<vector<Node>>& board, const int size, const char player, int numSim)
{
    char opponent = (player == 'B') ? 'R' : 'B';
    pair<int, int> bestMove = {-1, -1};
    double bestWinRate = 0.0;
    
    // iterate through all possible moves
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            // check if the cell is empty
            if (board[i][j].value == '.')
            {
                int winCount = 0;
                
                // run simulation for this move
                for (int k = 0; k < numSim; ++k)
                {
                    // copy the board and place move
                    auto boardCopy = board;
                    boardCopy[i][j].value = player;
                    
                    // simulate the game from this position
                    if (simulateRandomGame(boardCopy, size, player, opponent))
                        ++winCount;
                }
                
                double winRate = static_cast<double>(winCount) / numSim;
                
                // track the move with the best win rate
                if (winRate > bestWinRate)
                {
                    bestWinRate = winRate;
                    bestMove = {i, j};
                }
            }
        }
    }
    
    if (bestMove.first == -1 || bestMove.second == -1)
    {
        cout << "No move with a good win rate found, picking any available move." << endl;

        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                if (board[i][j].value == '.')
                    return {i, j};
            }
        }
    }
    
    return bestMove;
}

// determines the number of simulation based on game stage
int determineNumSim (int size, int empty)
{
    // default, early game, more than 75% of empty cells
    int numSim = 500;
    
    // mid game, between 75% and 25% of empty cells
    if (empty > (0.25 * size) && empty <= (0.75 * size))
        numSim = 700;
    // late game, less than 25% of empty cells
    else if (empty <= (0.25 * size))
        numSim = 1000;
    
    return numSim;
}

// calculates the number of empty cells on the board
int calculateEmptyCells(const vector<vector<Node>>& board, const int size)
{
    int emptycells = 0;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (board[i][j].value == '.')
                emptycells++;
    
    return emptycells;
}

// determines whether the bot should swap positions or not
bool shouldSwap(vector<vector<Node>>& board, const int size, const char player, const char opponent, int numSim)
{
    int botWinsIfSwapped = 0;
    int botWinsIfNotSwapped = 0;
    
    for (int i = 0; i < numSim; ++i)
    {
        // copy board state
        auto tempBoard = board;
        
        // simulate without swapping
        if (simulateRandomGame(tempBoard, size, player, opponent))
            botWinsIfNotSwapped++;
        
        // reset board
        tempBoard = board;
        
        // simulate with swapping
        if(simulateRandomGame(tempBoard, size, opponent, player))
            botWinsIfSwapped++;
    }
    
    return botWinsIfSwapped > botWinsIfNotSwapped;
}

// handles first move and swap rule, returns a pair with order of players
pair<char, char> firstMove(vector<vector<Node>>& board, const int size, const char human, const char bot)
{
    pair<char, char> players;
    
    players.first = (human == 'B') ? human : bot;
    players.second = (human == 'B') ? bot : human;
    
    int row = 0, col = 0;
    int emptyCells = size * size;
    
    // draw game board
    drawBoard(board, size);
    
    // determine the first mover
    char firstPlayer = (human == 'B') ? human : bot;
    char swap;
    
    // the first player is bot, the human decides whether to swap
    if (firstPlayer == bot)
    {
        // bot moves and returns coordinates
        auto [botRow, botCol] = monteCarloBestMove(board, size, bot, determineNumSim(size, emptyCells));
        
        if (botRow == -1 || botCol == -1)
            cout << "Bot failed to make a valid move" << endl;
        
        else
        {
            row = botRow;
            col = botCol;
        }
        
        if (row >= 0 && row < size && col >= 0 && col < size && board[row][col].value == '.')
        {
            // place player's mark
            board[row][col].value = bot;
            
            // output move information
            cout << "Blue Player first move: (" << row + 1 << ", " << col + 1 << ")." << endl;
            drawBoard(board, size);
            
            // update empty cells count
            emptyCells = calculateEmptyCells(board, size);
        }
        
        // prompt for and get input on whether human player wants to swap
        cout << "Do you want to swap positions? (Y/N)" << endl;
        
        cin >> swap;
        
        // human player wants to swap
        if (swap == 'y' || swap == 'Y')
        {
            players.first = human;
            players.second = bot;
            
            cout << "Decided to swap, now you are Blue player and bot is Red player" << endl;
        }
    }
    // the first player is human, the bot decides whether to swap
    else
    {
        bool validMove = false;
        while (!validMove)
        {
            cout << "Player Blue, enter your move (row and column, 1-based): ";
            cin >> row >> col;

            // convert to 0-based index
            row -= 1;
            col -= 1;

            // check for valid input
            if (row >= 0 && row < size && col >= 0 && col < size && board[row][col].value == '.')
            {
                validMove = true;
            }
            else
            {
                cout << "Invalid move! Try again." << endl;
            }
            
            // place player's mark
            board[row][col].value = human;
            
            // output move information
            cout << "Blue Player first move: (" << row + 1 << ", " << col + 1 << ")." << endl;
            drawBoard(board, size);
            
            // update empty cells count
            emptyCells = calculateEmptyCells(board, size);
            
            bool botSwap = shouldSwap(board, size, bot, human, determineNumSim(size, emptyCells));
            
            if (botSwap)
            {
                players.first = bot;
                players.second = human;
                
                cout << "Bot decided to swap, now you are Red player and bot is Blue player" << endl;
            }
        }
    }
    
    return players;
}

// handles game loop until winning condition is met
void gameLoop(vector<vector<Node>>& board, const int size, const char player)
{
    // B for Blue, R for Red
    // Blue starts first
    char currentPlayer = 'B';
    
    // assign to the human player the value passed as argument
    char human = player;
    char bot = (human == 'B') ? 'R' : 'B';
    
    // check to avoid that human and bot play the same color
    if (human == 'R')
        bot = 'B';
    
    // utility for first move swap
    pair<char, char> players;
    
    players.first = (human == 'B') ? human : bot;
    players.second = (human == 'B') ? bot : human;
    
    bool gameOver = false;
    int row = 0, col = 0;
    int emptyCells = size * size;
    
    // handle first move before game loop
    players = firstMove(board, size, human, bot);
    
    // swap order of players
    human = (players.first == human) ? 'B' : 'R';
    bot = (players.second == bot) ? 'R' : 'B';
    
    currentPlayer = 'R';
    
    while (!gameOver)
    {
        // draw the current state of the board
        drawBoard(board, size);
        
        // if current player is human, prompt for the move
        if (currentPlayer == human)
        {
            bool validMove = false;
            while (!validMove)
            {
                cout << "Player " << (currentPlayer == 'B' ? "Blue" : "Red")
                << ", enter your move (row and column, 1-based): ";
                cin >> row >> col;

                // convert to 0-based index
                row -= 1;
                col -= 1;

                // check for valid input
                if (row >= 0 && row < size && col >= 0 && col < size && board[row][col].value == '.')
                {
                    validMove = true;
                }
                else
                {
                    cout << "Invalid move! Try again." << endl;
                }
            }
        }
        // else the current player is bot, determine move
        else
        {
            // bot moves and returns coordinates
            auto [botRow, botCol] = monteCarloBestMove(board, size, bot, determineNumSim(size, emptyCells));
            
            if (botRow == -1 || botCol == -1)
                cout << "Bot failed to make a valid move" << endl;
            
            else
            {
                row = botRow;
                col = botCol;
            }
        }
        
        // check for valid move
        if (row >= 0 && row < size && col >= 0 && col < size && board[row][col].value == '.')
        {
            // place player's mark
            board[row][col].value = currentPlayer;
            
            // output move information
            cout << (currentPlayer == 'B' ? "Blue" : "Red") << " Player move: (" << row + 1 << ", " << col + 1 << ")." << endl;
            
            // update empty cells count
            emptyCells = calculateEmptyCells(board, size);
            
            // check for current player winning after move
            if (checkWinUnionFind(board, size, currentPlayer))
            {
                drawBoard(board, size);
                cout << (currentPlayer == 'B' ? "Blue" : "Red") << " Player wins!" << endl;
                gameOver = true;
            }
            // if not, change current player
            else
            {
                currentPlayer = (currentPlayer == 'B' ? 'R' : 'B');
            }
        }
        else
        {
            cout << "Invalid move! Try again." << endl;
        }
    }
}

int main()
{
    // variables for player preferences
    int boardSize = 1;
    char answer = 'N';
    char player = 'B';
    
    // prompt for and get user preferences to set the game
    while ((answer != 'Y' && answer != 'y') || boardSize < 0)
    {
        // prompt for and get board size from player
        cout << "What board size do you want to play with?" << "\n"
        << "(input a single number, e.g. 7 for a 7x7 board)" << endl;
        cin >> boardSize;
        
        // prompt for and get color from player
        cout << "What color do you want to play with?" << "\n"
        << "Remember, Blue plays left-right, while Red plays Top-Bottom." << "\n"
        << "(B = Blue, R = Red, blue plays first): " << endl;
        cin >> player;
        
        // ensure player enters a valid color
        if (player != 'B' && player != 'R')
        {
            cout << "Invalid color choice! Please choose 'B' for Blue or 'R' for Red." << endl;
            continue;
        }
        
        // output the board with player decision
        cout << "Ok, you choose to play as " << (player == 'B' ? "Blue" : "Red") << " player, with a " << boardSize << " x " << boardSize << " board" << endl;
        
        // check for player confirmation
        cout << "Do you confirm your choices? (Y/N): ";
        cin >> answer;
    }
    
    // initialize the board
    vector<vector<Node>> board(boardSize, vector<Node>(boardSize));
    initializeBoard(board, boardSize);

    // start the game loop
    gameLoop(board, boardSize, player);

    
    return 0;
}
