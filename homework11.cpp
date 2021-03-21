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
        char captured = '/';
        double finalScore;
        bool isKing = false;
        bool isCrowned = false;

        Move(int source_x, int source_y, int dest_x, int dest_y, string mType)
        {
            sx = source_x;
            sy = source_y;
            dx = dest_x;
            dy = dest_y;
            moveType = mType;
        }

        Move(int source_x, int source_y, int dest_x, int dest_y, string mType, bool king)
        {
            sx = source_x;
            sy = source_y;
            dx = dest_x;
            dy = dest_y;
            moveType = mType;
            isKing = king;
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
        vector<vector<Move*>> getAllMoves(int row, int col, bool isKing, double currentScore)
        {
            cout << "calling getting all moves" << endl;
            vector<vector<Move*>> allMoves;
            vector<Move*> moves;
            getAllJumpMoves(row, col, isKing, moves, allMoves, false, currentScore);

            vector<Move*> singleMoves = getAllSingleMoves(row, col, isKing, currentScore);
            for(auto m : singleMoves)
            {
                vector<Move*> tmp;
                tmp.push_back(m);
                allMoves.push_back(tmp);
            }

            return allMoves;

        }

        /* Get all single move for a piece */
        vector<Move*> getAllSingleMoves(int row, int col, bool isKing, double currentScore)
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
                        Move* m = new Move(col, row, dest_x, dest_y, "E", isKing);
                        if(!isKing)
                        {
                            if(player == "BLACK" && dest_y == board.size() - 1)
                                m->isCrowned = true;
                            else if(player == "WHITE" && dest_y == 0)
                                m->isCrowned = true;
                        }

                        moves.push_back(m);
                    }
                }
            }

            return moves;
        }

        /* Get all jump moves */
        void getAllJumpMoves(int row, int col, bool isKing, vector<Move*>& moves, vector<vector<Move*>>& allMoves, bool crowned, double currentScore)
        {

            if(crowned)
            {
                allMoves.push_back(moves);
                return;
            }

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

                if(dest_x >= 0 && dest_x < board.size() && dest_y >= 0 && dest_y < board.size())
                {
                    if(board[dest_y][dest_x] == '.' && enemy.count(board[neighbor_y][neighbor_x]))
                    {
                        leaf = false;
                        Move* m = new Move(col, row, dest_x, dest_y, "J");
                        m->skipped = make_pair(neighbor_x, neighbor_y);

                        moves.push_back(m);
                        double originScore = currentScore;
                        m->captured = board[neighbor_y][neighbor_x];
                        board[neighbor_y][neighbor_x] = 'x';
                        char before = board[row][col];
                        board[row][col] = '.';  

                        //Check if the piece become a king after move
                        if(!isKing)
                        {
                            if(player == "BLACK" && dest_y == board.size() - 1)
                            {
                                isKing = true;
                                crowned = true;
                                m->isCrowned = true;
                            }
                            else if(player == "WHITE" && dest_y == 0)
                            {
                                isKing = true;
                                crowned = true;
                                m->isCrowned = true;
                            }
                        }

                        getAllJumpMoves(dest_y, dest_x, isKing, moves, allMoves, crowned, currentScore);
                        if(crowned)
                            crowned = false;

                        Move* recover = moves.back();
                        board[recover->skipped.second][recover->skipped.first] = m->captured;
                        moves.pop_back();
                        board[row][col] = before;
                    }

                }
            }

            if(leaf)
                allMoves.push_back(moves);
        }

        vector<pair<int, int>> getAllPiece(string player)
        {
            vector<pair<int, int>> pieces;
            for(int i = 0; i < board.size(); i++)
            {
                for(int j = 0; j < board[0].size(); j++)
                {
                    if(player == "BLACK")
                    {
                        if(board[i][j] == 'b' || board[i][j] == 'B')
                            pieces.push_back(make_pair(i,j));
                    }
                    else
                    {
                        if(board[i][j] == 'w' || board[i][j] == 'W')
                            pieces.push_back(make_pair(i,j));
                    }
                }
            }

            return pieces;
        }

        //evaluate the result of a series of moves and update the final score on the last move
        void evalMoves(vector<Move*>& moves, int startRow, int startCol, double currentScore, bool isKing)
        {
            auto tmpBoard = board;
            double finalScore = 0.0;
            if(moves.empty())
                return;
            if(!moves.empty())
                tmpBoard[startRow][startCol] = '.';
            for(int i = 0; i < moves.size(); i++)
            {
                if(i == moves.size() - 1)
                {
                    if(moves[i]->captured != '/')
                        tmpBoard[moves[i]->skipped.second][moves[i]->skipped.first] = '@';

                    //check if it is crowned
                    if(isKing || moves[i]->isCrowned)
                    {
                        if(player == "BLACK")
                            tmpBoard[moves[i]->dy][moves[i]->dx] = 'B';
                        else
                            tmpBoard[moves[i]->dy][moves[i]->dx] = 'W'; 
                    }
                    else
                    {
                        if(player == "BLACK")
                            tmpBoard[moves[i]->dy][moves[i]->dx] = 'b';
                        else
                            tmpBoard[moves[i]->dy][moves[i]->dx] = 'w';
                    }

                    //evaluate the board
                    moves[i]->finalScore = evalAny(tmpBoard);
                    finalScore = moves[i]->finalScore;
                }
                else
                {
                    if(moves[i]->captured != '/')
                        tmpBoard[moves[i]->skipped.second][moves[i]->skipped.first] = '@';
                }             
            }

            cout << "result board after moves" << endl;
            cout << "final score: " << evalAny(tmpBoard) << endl;
            printAnyBoard(tmpBoard);
            cout << "--------end of this bord------------" << endl;
        }


        double evalAny(vector<vector<char>>& board)
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

        /* DEBUG print any board */
        void printAnyBoard(vector<vector<char>>& board)
        {
            for(auto row : board)
            {
                for(auto p : row)
                    cout << p << " ";
                cout << endl;
            }
        }

};


void printCurBoard(vector<vector<char>> board)
{
    for(auto row : board)
    {
        for(auto p : row)
            cout << p << " ";
        cout << endl;
    }
}

int main()
{
    Game game;
    game.parse("input.txt");
    double currentScore = game.eval();
    string player = game.player;

    cout << "initial board: " << endl;
    game.printBoard();
    cout << "inital score: " << game.eval() << endl;
    cout << "*****************************" << endl;

    auto pieces = game.getAllPiece(player);
    for(auto p : pieces)
    {
        //cout << "row: " << p.first << " col: " << p.second << endl;
        bool isKing = (game.board[p.first][p.second] == 'W' || game.board[p.first][p.second] == 'B') ? true : false;
        auto allMoves = game.getAllMoves(p.first, p.second, isKing, currentScore);

        cout << "current piece moves for row: " << p.first << " col: " << p.second << endl;
        for(auto moves : allMoves)
        {
            string outputMove = "";
            for(auto m : moves)
            {
                outputMove = m->moveType + " " + (char)(m->sx + 'a') + to_string((8 - m->sy)) + " " + (char)(m->dx + 'a') + to_string((8 - m->dy));
            
                if(outputMove.size())
                    cout << outputMove << endl;
            }
            game.evalMoves(moves, p.first, p.second, currentScore, isKing);
        }
    }

    // vector<vector<Move*>> allMoves;
    // vector<Move*> moves;
    // game.getAllJumpMoves(6,4, false, moves,  allMoves, false, currentScore);
    //vector<Move*> moves = game.getAllSingleMoves(6,4,true, currentScore);
    //auto allMoves = game.getAllMoves(2,3, true, currentScore);
  

    // for(auto m : moves)
    // {
    //     game.board[m->dy][m->dx] = '*';
    //     cout << m->moveType << " " << (char)(m->sx + 'a') << (8 - m->sy) << " " << (char)(m->dx + 'a') << (8 - m->dy) << endl;
    //     cout << "Score: " << m->finalScore << endl;
    // }
        
    
    // if(allMoves.size() != 1)
    //     game.board[6][4] = '!';


    // string outputMove = "";
    // for(auto moves : allMoves)
    // {
    //     //double originScore = currentScore;
    //     auto tmpBoard = game.board;
    //     for(int i = 0; i < moves.size(); i++)
    //     {

    //         if(i == moves.size() - 1)
    //         {
    //             if(moves[i]->captured != '/')
    //             {
    //                 //game.board[moves[i]->skipped.second][moves[i]->skipped.first] = 'x';
    //                 // if(moves[i]->captured == 'b')
    //                 //     currentScore += 1;
    //                 // else if(moves[i]->captured == 'B')
    //                 //     currentScore += 1.5;
    //                 // else if(moves[i]->captured == 'w')
    //                 //     currentScore -= 1;
    //                 // else if(moves[i]->captured == 'W')
    //                 //     currentScore -= 1.5;
    //                 tmpBoard[moves[i]->skipped.second][moves[i]->skipped.first] = 'x';
                    
    //             }

    //             //check if it is crowned
    //             if(moves[i]->isCrowned)
    //             {
    //                 if(player == "BLACK")
    //                     currentScore -= 0.5;
    //                 else
    //                     currentScore += 0.5;
    //             }
                    
    //             tmpBoard[moves[i]->dy][moves[i]->dx] = '@';           
    //         }
                
    //         else
    //         {
    //             if(moves[i]->captured != '/')
    //             {
    //                 tmpBoard[moves[i]->skipped.second][moves[i]->skipped.first] = 'x';
    //                 // if(moves[i]->captured == 'b')
    //                 //     currentScore += 1;
    //                 // else if(moves[i]->captured == 'B')
    //                 //     currentScore += 1.5;
    //                 // else if(moves[i]->captured == 'w')
    //                 //     currentScore -= 1;
    //                 // else if(moves[i]->captured == 'W')
    //                 //     currentScore -= 1.5;
    //             }
    //             //tmpBoard[moves[i]->dy][moves[i]->dx] = '@';
                    
    //         }
    //         outputMove = moves[i]->moveType + " " + (char)(moves[i]->sx + 'a') + to_string((8 - moves[i]->sy)) + " " + (char)(moves[i]->dx + 'a') + to_string((8 - moves[i]->dy));
            
    //         if(outputMove.size())
    //             cout << outputMove << endl;
                
            
    //     }
    //     if(outputMove.size())
    //     {
    //         //moves.back()->finalScore = currentScore;
    //         //cout << "final score on this move: " << moves.back()->finalScore << endl;  
    //         cout << "board after move: " << endl;
    //         printCurBoard(tmpBoard);
    //         cout << "-------------------" << endl;
    //         //game.printBoard();
    //     }
    //     //currentScore = originScore;
    // }

    
    // string output = "";
    // for(auto moves : allMoves)
    // {
    //     for(auto m : moves)
    //     {
    //         output = m->moveType + " " + (char)(m->sx + 'a') + to_string((8 - m->sy)) + " " + (char)(m->dx + 'a') + to_string((8 - m->dy));
    //         cout << output << endl;
    //     }
    //         //cout << m->moveType << " " << (char)(m->sx + 'a') << (8 - m->sy) << " " << (char)(m->dx + 'a') << (8 - m->dy) << endl;
    //     if(output.size())    
    //         cout << "-------------------" << endl;
    // }

    
    //game.printBoard();
    return 0;
}