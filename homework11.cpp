#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;


class Game
{
    public:
        string player;
        string mode;
        double timeLeft;
        vector<vector<char>> board;


        /* Parse the input file */
        void parse(string file)
        {
            cout << "called" << endl;
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
};


int main()
{
    Game game;
    game.parse("input.txt");

    cout << "game mode: " << game.mode << endl;
    cout << "game player: " << game.player << endl;
    cout << "time left: " << game.timeLeft << endl;
    cout << "------------------------------------" << endl;

    for(auto row : game.board)
    {
        for(auto p : row)
            cout << p << " ";
        cout << endl;
    }
    return 0;
}