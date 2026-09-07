#include <vector>

namespace core {
    class BoardSetup {
    public:
        std::vector<int> board;
        std::vector<int> displayBoard;
        int numCols;
        int numRows;
        int totalTiles;
        int numMines;
        bool minesGenerated = false;

        BoardSetup(int c, int r, int m) {
            numCols = c;
            numRows = r;
            totalTiles = numCols * numRows;
            numMines = totalTiles/m;
            for (int i=0; i < totalTiles; ++i) {
                board.push_back(0);
                displayBoard.push_back(0);
            }
        }
    };

    void engineLoop();

    void updatePositions(std::vector<int> positions, int currPos);
    void generateMines(int startTile, BoardSetup& newBoard);
    int debugInput(BoardSetup& newBoard);
    void debugBoardState(BoardSetup& newBoard);
    void populateDisplayBoard(BoardSetup& newBoard);
    int countNeighbors(int currTile, BoardSetup& newBoard);
    int checkTile(int tile, BoardSetup& newBoard);
    bool selectTile(int tile, BoardSetup& newBoard);
    void chainTile(int tile, BoardSetup& newBoard);
    void revealBoard(BoardSetup& newBoard);

    struct TileSafety {
        bool leftSafe;
        bool rightSafe;
        bool upSafe;
        bool downSafe;
    };

    TileSafety checkTileSafety(int tile, BoardSetup& newBoard);
}