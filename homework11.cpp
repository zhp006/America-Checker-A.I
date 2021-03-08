#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
using namespace std;


class Move{
    public:
        int sx;
        int sy;
        int dx;
        int dy;
        string moveType;
        pair<int,int> skipped;
        char captured;

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
        unordered_set<char> enemy;

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

            // enemy set
            if(player == "BLACK")
            {
                enemy.insert('w');
                enemy.insert('W');
            }
            else
            {
                enemy.insert('b');
                enemy.insert('B');
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

        /* Get all valid moves for a piece */
        vector<Move*> getAllMoves(int row, int col, bool isKing, vector<Move*> moves)
        {
            vector<Move*> allMoves = getAllSingleMoves(row, col, isKing);
            


            return allMoves;

        }

        /* Get all single move for a piece */
        vector<Move*> getAllSingleMoves(int row, int col, bool isKing)
        {
            vector<Move*> moves;
            vector<vector<int>> dir;
            if(isKing)
                dir = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
            else if(player == "BLACK")
                dir = {{-1, 1}, {1, 1}};
            else
                dir = {{-1, -1}, {1, -1}};

            for(auto d : dir)
            {
                int dest_x = col + d[0];
                int dest_y = row + d[1];

                if(dest_x >= 0 && dest_x < board.size() && dest_y >= 0 && board.size())
                {
                    if(board[dest_y][dest_x] == '.')
                    {
                        Move* m = new Move(col, row, dest_x, dest_y, "E");
                        moves.push_back(m);
                    }
                }
            }

            return moves;
        }

        /* Get all jump moves */
        void getAllJumpMoves(int row, int col, bool isKing, vector<Move*>& moves, vector<vector<Move*>>& allMoves)
        {

            vector<vector<int>> dir;
            if(isKing)
                dir = {{-2, -2}, {2, -2}, {2, 2}, {-2, 2}};
            else if(player == "BLACK")
                dir = {{-2, 2}, {2, 2}};
            else
                dir = {{-2, -2}, {2, -2}};
                

            bool leaf = true;
            for(auto d : dir)
            {
                int dest_x = col + d[0];
                int dest_y = row + d[1];

                int neighbor_x = col + d[0]/2;
                int neighbor_y = row + d[1]/2;

                if(dest_x >= 0 && dest_x < board.size() && dest_y >= 0 && board.size())
                {
                    if(board[dest_y][dest_x] == '.' && enemy.count(board[neighbor_y][neighbor_x]))
                    {
                        leaf = false;
                        Move* m = new Move(col, row, dest_x, dest_y, "J");
                        m->skipped = make_pair(neighbor_x, neighbor_y);
                        moves.push_back(m);

                        m->captured = board[neighbor_y][neighbor_x];
                        board[neighbor_y][neighbor_x] = 'x';
                        board[row][col] = '.';

                        //Check if the piece become a king after move
                        if(!isKing)
                        {
                            if(player == "BLACK" && dest_y == board.size() - 1)
                                isKing = true;
                            else if(player == "WHITE" && dest_y == 0)
                                isKing = true;
                        }

                        getAllJumpMoves(dest_y, dest_x, isKing, moves, allMoves);

                        Move* recover = moves.back();
                        board[recover->skipped.second][recover->skipped.first] = m->captured;
                        moves.pop_back();
                    }
                }
            }

            if(leaf)
                allMoves.push_back(moves);

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

    vector<Move*> moves;
    vector<vector<Move*>> allMoves;
    game.getAllJumpMoves(4,3, false, moves, allMoves);

    for(auto moves : allMoves)
    {
        for(auto m : moves)
            game.board[m->dy][m->dx] = '*';
    }

    // for(auto m : allMoves[1])
    //     game.board[m->dy][m->dx] = '*';

    for(auto moves : allMoves)
    {
        for(auto m : moves)
            cout << m->moveType << " " << (char)(m->sx + 'a') << (8 - m->sy) << " " << (char)(m->dx + 'a') << (8 - m->dy) << endl;
        cout << "-------------------" << endl;
    }

    // for(auto m : moves)
    //     game.board[m->dy][m->dx] = '*';
    
    // for(auto m : moves)
    //     cout << m->moveType << " " << (char)(m->sx + 'a') << (8 - m->sy) << " " << (char)(m->dx + 'a') << (8 - m->dy) << endl;
    game.printBoard();
    return 0;
}