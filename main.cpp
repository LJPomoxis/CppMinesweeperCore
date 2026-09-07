#include "core.hpp"
#include <iostream>

int main() {
    int dimensions, mineDensity;
    std::cout << "Enter board size: ";
    std::cin >> dimensions;
    std::cout << "Enter mine density (percentage of board): ";
    std::cin >> mineDensity;
    int columns, rows;
    columns = dimensions;
    rows = dimensions;

    mineDensity = 100 / mineDensity;

    core::BoardSetup newBoard(columns, rows, mineDensity);

    bool running = true;
    int selectedTile = 0;
    while (running) {
        core::debugBoardState(newBoard);
        selectedTile = core::debugInput(newBoard);
        //std::cout << selectedTile << std::endl;
        if (!newBoard.minesGenerated) {
            core::generateMines(selectedTile, newBoard);
            core::populateDisplayBoard(newBoard);
        }

        running = core::selectTile(selectedTile, newBoard);
    }

    std::cout << "Game over" << std::endl;

    core::revealBoard(newBoard);
    core::debugBoardState(newBoard);

    return 0;
}