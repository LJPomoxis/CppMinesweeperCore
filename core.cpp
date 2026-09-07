#include <iostream>
#include <vector>
#include <random>
#include <utility>
#include "core.hpp"

namespace core{
    void engineLoop(){

    }
    
    void updatePositions(std::vector<int> positions, int currPos) {
        if (currPos != positions.size() - 1) {
            std::swap(positions[currPos], positions.back());
        }
        positions.pop_back();
    }

    void generateMines(int startTile, BoardSetup& newBoard) {
        newBoard.minesGenerated = true; // set flag to declare board generated

        // not needed, good math to save though
        //int startRow = startTile / board.numCols;
        //int startColumn = startTile % board.numCols;

        std::random_device rand;
        std::mt19937 gen(rand());
        std::uniform_int_distribution<> dist(0, (newBoard.totalTiles - 1));

        std::vector<int> positions;
        bool easyMode = true;

        int left = startTile - 1;
        int right = startTile + 1;
        int upL = startTile - newBoard.numCols - 1;
        int upR = startTile - newBoard.numCols + 1;
        int downL = startTile + newBoard.numCols - 1;
        int downR = startTile + newBoard.numCols + 1;

        for (int i=0; i < newBoard.totalTiles; ++i) {
            // continue to prevent startTile and surrounding tiles being added as mines
            if ((i > (upL - 1) && i < (upR + 1)) && easyMode) continue;
            if ((i > (downL - 1) && i < (downR + 1)) && easyMode) continue;
            if ((i == left) && easyMode) continue;
            if ((i == right) && easyMode) continue;
            if (i == startTile) continue; // don't add startTile to available positions
            positions.push_back(i);
        }

        int placedMines = 0;
        int currPos = 0;
        while (placedMines < newBoard.numMines) {
            // each loop the distribution will be set based on the number of placed mines
            dist.param(std::uniform_int_distribution<>::param_type(0, (newBoard.totalTiles - placedMines) - 1));

            currPos = positions[dist(gen)];
            newBoard.board[currPos] = 1;

            // This will swap and pop the currPos, works because positions contains indices and order doesn't matter
            updatePositions(positions, currPos);
            placedMines++;
        }
    }

    // Currently doesn't account for boards of height (rows) larger than 26
    int debugInput(BoardSetup& newBoard) {
        int column;
        char row;
        std::cout << "Enter column: ";
        std::cin >> column;

        while (column < 0 || column > (newBoard.numCols - 1)) {
            std::cout << "Enter valid column: ";
            std::cin >> column;
        }

        std::cout << "Enter row: ";
        std::cin >> row;

        // check if character entered was not Uppercase
        if (int(row) > 90) {
            // 32 represents ascii value gap between uppercase and lowercase characters
            // e.g. A == 65 and a == 97
            row -= 32;
        }

        // A == 65, this will set the value to be in a range starting from 0
        row -= 65;
        while (row < 0 || row > (newBoard.numRows - 1)) {
            std::cout << "Enter valid row: ";
            std::cin >> row;

            if (int(row) > 90) row -= 32;
            row -= 65;
        }

        return int(column) + (newBoard.numCols*int(row));
    }

    void debugBoardState(BoardSetup& newBoard) {
        // print header  |   | 0 | 1 | 2 | ...
        // then each row | A | - | - | - | ...
        //               | B | - | - | - | ...

        int currTile = 0;
        std::string currCol = "0 ";
        // Not necessary as "0 " is simply a more intuitive representation
        //currCol[0] = 48;
        //currCol[1] = 32;
        for (int row=0; row < newBoard.numRows; ++row) {
            char currRow = 65 + row;
            if (row == 0) currRow = 32; // ascii code for blank space character
            std::cout << "| " << currRow << " |";

            if (row == 0) {
                for (int col=0; col < newBoard.numCols; ++col) {
                    currCol = "0 "; // necessary reset as our character math is relative assignment

                    if (col > 9) {
                        currCol[0] += (col / 10);
                        currCol[1] = 48 + (col % 10);
                    } else {
                        currCol[0] += col;
                    }
                    std::cout << " " << currCol << "|";
                }
                std::cout << std::endl;
                std::cout << "| A |";
            }

            for (int col=0; col < newBoard.numCols; ++col) {
                currCol = "0 "; // necessary reset as our character math is relative assignment
                
                currTile = col + (newBoard.numCols * row);
                // check if tile is marked as revealed, if not set to '-' char
                if (newBoard.board[currTile] == 2) {
                    currCol[0] += newBoard.displayBoard[currTile];
                    if (newBoard.displayBoard[currTile] == 0) {
                        currCol[0] = 32;
                    }
                } else {
                    currCol[0] = '-';
                }

                std::cout << " " << currCol << "|";
            }
            std::cout << std::endl;
        }
    }

    void populateDisplayBoard(BoardSetup& newBoard) {
        for (int i=0; i < newBoard.totalTiles; ++i) {
            if (newBoard.board[i] == 1) {
                newBoard.displayBoard[i] = 100;
            } else {
                newBoard.displayBoard[i] = countNeighbors(i, newBoard);
            }
        }
    }

    int countNeighbors(int currTile, BoardSetup& newBoard) {
        int mineCount = 0;
        TileSafety safetyCheck = checkTileSafety(currTile, newBoard);

        int upL = currTile - newBoard.numCols - 1;
        int up = currTile - newBoard.numCols;
        int upR = currTile - newBoard.numCols + 1;
        int left = currTile - 1;
        int right = currTile + 1;
        int downL = currTile + newBoard.numCols - 1;
        int down = currTile + newBoard.numCols;
        int downR = currTile + newBoard.numCols + 1;

        if (safetyCheck.upSafe) mineCount += checkTile(up, newBoard);
        if (safetyCheck.downSafe) mineCount += checkTile(down, newBoard);

        if (safetyCheck.leftSafe) {
            mineCount += checkTile(left, newBoard);
            if (safetyCheck.upSafe) mineCount += checkTile(upL, newBoard);
            if (safetyCheck.downSafe) mineCount += checkTile(downL, newBoard);
        }

        if (safetyCheck.rightSafe) {
            mineCount += checkTile(right, newBoard);
            if (safetyCheck.upSafe) mineCount += checkTile(upR, newBoard);
            if (safetyCheck.downSafe) mineCount += checkTile(downR, newBoard);
        }

        return mineCount;
    }

    int checkTile(int tile, BoardSetup& newBoard) {
        if (newBoard.board[tile] == 1) {
            return 1;
        }
        return 0;
    }

    // Update function to remove chainTile and consolidate chaining
    bool selectTile(int selectedTile, BoardSetup& newBoard) {
        if (checkTile(selectedTile, newBoard)) return false;
        newBoard.board[selectedTile] = 2;

        if (newBoard.displayBoard[selectedTile] != 0) return true;

        TileSafety safetyCheck = checkTileSafety(selectedTile, newBoard);

        int upL = selectedTile - newBoard.numCols - 1;
        int up = selectedTile - newBoard.numCols;
        int upR = selectedTile - newBoard.numCols + 1;
        int left = selectedTile - 1;
        int right = selectedTile + 1;
        int downL = selectedTile + newBoard.numCols - 1;
        int down = selectedTile + newBoard.numCols;
        int downR = selectedTile + newBoard.numCols + 1;

        if (safetyCheck.upSafe && newBoard.board[up] == 0) {
            newBoard.board[up] = 2;
            if (newBoard.displayBoard[up] == 0) chainTile(up, newBoard);
        }
        if (safetyCheck.downSafe && newBoard.board[down] == 0) {
            newBoard.board[down] = 2;
            if (newBoard.displayBoard[down] == 0) chainTile(down, newBoard);
        }

        if (safetyCheck.leftSafe) {
            if (newBoard.board[left] == 0) {
                newBoard.board[left] = 2;
                if (newBoard.displayBoard[left] == 0) chainTile(left, newBoard);
            }
            if (safetyCheck.upSafe && newBoard.board[upL] == 0) {
                newBoard.board[upL] = 2;
                if (newBoard.displayBoard[upL] == 0) chainTile(upL, newBoard);
            }
            if (safetyCheck.downSafe && newBoard.board[downL] == 0) {
                newBoard.board[downL] = 2;
                if (newBoard.displayBoard[downR] == 0) chainTile(downL, newBoard);
            }
        }

        if (safetyCheck.rightSafe) {
            if (newBoard.board[right] == 0) {
                newBoard.board[right] = 2;
                if (newBoard.displayBoard[right] == 0) chainTile(right, newBoard);
            }
            if (safetyCheck.upSafe && newBoard.board[upR] == 0) {
                newBoard.board[upR] = 2;
                if (newBoard.displayBoard[upR] == 0) chainTile(upR, newBoard);
            }
            if (safetyCheck.downSafe && newBoard.board[downR] == 0) {
                newBoard.board[downR] = 2;
                if (newBoard.displayBoard[downR] == 0) chainTile(downR, newBoard);
            }
        }

        return true;
    }

    void chainTile(int tile, BoardSetup& newBoard) {
        TileSafety safetyCheck = checkTileSafety(tile, newBoard);

        int upL = tile - newBoard.numCols - 1;
        int up = tile - newBoard.numCols;
        int upR = tile - newBoard.numCols + 1;
        int left = tile - 1;
        int right = tile + 1;
        int downL = tile + newBoard.numCols - 1;
        int down = tile + newBoard.numCols;
        int downR = tile + newBoard.numCols + 1;

        if (safetyCheck.upSafe && newBoard.board[up] == 0) {
            newBoard.board[up] = 2;
            if (newBoard.displayBoard[up] == 0) chainTile(up, newBoard);
        }
        if (safetyCheck.downSafe && newBoard.board[down] == 0) {
            newBoard.board[down] = 2;
            if (newBoard.displayBoard[down] == 0) chainTile(down, newBoard);
        }

        if (safetyCheck.leftSafe) {
            if (newBoard.board[left] == 0) {
                newBoard.board[left] = 2;
                if (newBoard.displayBoard[left] == 0) chainTile(left, newBoard);
            }
            if (safetyCheck.upSafe && newBoard.board[upL] == 0) {
                newBoard.board[upL] = 2;
                if (newBoard.displayBoard[upL] == 0) chainTile(upL, newBoard);
            }
            if (safetyCheck.downSafe && newBoard.board[downL] == 0) {
                newBoard.board[downL] = 2;
                if (newBoard.displayBoard[downR] == 0) chainTile(downL, newBoard);
            }
        }

        if (safetyCheck.rightSafe) {
            if (newBoard.board[right] == 0) {
                newBoard.board[right] = 2;
                if (newBoard.displayBoard[right] == 0) chainTile(right, newBoard);
            }
            if (safetyCheck.upSafe && newBoard.board[upR] == 0) {
                newBoard.board[upR] = 2;
                if (newBoard.displayBoard[upR] == 0) chainTile(upR, newBoard);
            }
            if (safetyCheck.downSafe && newBoard.board[downR] == 0) {
                newBoard.board[downR] = 2;
                if (newBoard.displayBoard[downR] == 0) chainTile(downR, newBoard);
            }
        }
    }

    void revealBoard(BoardSetup& newBoard) {
        for (int i=0; i < newBoard.totalTiles; ++i) {
            if (newBoard.board[i] == 0) newBoard.board[i] = 2;
        }
    }

    TileSafety checkTileSafety(int tile, BoardSetup& newBoard) {
        bool leftSafe = false;
        bool rightSafe = false;
        bool upSafe = false;
        bool downSafe = false;

        if (tile % newBoard.numCols != 0) leftSafe = true;
        if (tile % newBoard.numCols != (newBoard.numCols - 1)) rightSafe = true;
        if (tile / newBoard.numRows != 0) upSafe = true;
        if (tile / newBoard.numRows != (newBoard.numRows - 1)) downSafe = true;

        return TileSafety{leftSafe, rightSafe, upSafe, downSafe};
    }

}