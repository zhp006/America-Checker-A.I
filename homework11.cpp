#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


class Move{
    public:
        int sx;
        int sy;
        int dx;
        int dy;
        string moveType;
        vector<pair<int, int>> skipped;

        Move(int source_x, int source_y, int dest_x, int dest_y, string mType)
        {
            sx = source_x;
            sy = source_y;
            dx = dest_x;
            dy = dest_y;
            moveType = mType;
        }


};

class Game
{
    public:
        string player;
        string mode;
        double timeLeft;
        vector<vector<char>> board;
        vector<vector<bool>> visited;

        /* Parse the input file */
        void parse(string file)
        {
            ifstream input;
            input.open(file);
            string line;
            stringstream ss;

            // get the mode
            input >> line;
            mode = line;

            // get the player
            input >> line;
            player = line;

            // get time left
            input >> line;
            timeLeft = stod(line);

            // get the board
            while(input >> line)
            {
                vector<char> row;
                for(auto c : line)
                    row.push_back(c);
                
                board.push_back(row);
            }
        }


        /* Evalutate the board */
        double eval()
        {
            double score = 0.0;
            for(auto row : board)
            {
                for(auto p : row)
                {
                    if(p == 'w')
                        score += 1.0;
                    else if(p == 'W')
                        score += 1.5;
                    else if(p == 'b')
                        score -= 1.0;
                    else if(p == 'B')
                        score -= 1.5;
                }
            }

            return score;
        }

        /* Get all valid moves for a piece*/
        vector<Move*> getAllMoves(int row, int col, bool isKing, vector<Move*> moves)
        {
            vector<Move*> allMoves;
            


            return allMoves;

        }

        /* Get all single move for a piece*/
        vector<Move*> getAllSingleMoves(int row, int col, bool isKing)
        {
            vector<Move*> moves;
            if(isKing)
            {
                vector<vector<int>> dir{{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
                for(auto d : dir)
                {
                    int dest_x = col + d[0];
                    int dest_y = row + d[1];

                    if(dest_x >= 0 && dest_x < board.size() && dest_y >= 0 && board.size())
                    {
                        if(board[dest_y][dest_x] == '.' || true)
                        {
                            Move* m = new Move(col, row, dest_x, dest_y, "E");
                            moves.push_back(m);
                        }
                    }
                }
            }
            else if(player == "Black")
            {
                return moves;
            }
            else
            {
                return moves;
            }

            return moves;
        }


        /* DEBUG print board */
        void printBoard()
        {
            for(auto row : board)
            {
                for(auto p : row)
                    cout << p << " ";
                cout << endl;
            }
        }
};


int main()
{
    Game game;
    game.parse("input.txt");


    auto moves = game.getAllSingleMoves(0,0, true);
    for(auto m : moves)
        game.board[m->dy][m->dx] = '*';
    
    game.printBoard();
    return 0;
}