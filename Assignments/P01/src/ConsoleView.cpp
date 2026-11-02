#include "ConsoleView.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


ConsoleView::ConsoleView(VideoPoker& game)
    : game(game)
{
}


void ConsoleView::printHand() const
{
    const Hand& hand = game.getHand();

    for (int i = 0; i < hand.size(); i++)
    {
        cout << "[" << i + 1 << "] "
             << hand.at(i).toString();

        if (game.isHeld(i))
        {
            cout << "*";
        }

        cout << "   ";
    }

    cout << endl;
}


void ConsoleView::run()
{
    cout << "\nJACKS OR BETTER VIDEO POKER\n";
    cout << "---------------------------\n";

    while (game.phase() != Phase::GameOver)
    {
        // -------------------------
        // BETTING
        // -------------------------
        if (game.phase() == Phase::Betting)
        {
            string input;

            cout << "\nCredits: " << game.getCredits() << endl;
            cout << "Bet (1-5, q to quit): ";

            if (!getline(cin, input)) return;

            if (input == "q" || input == "Q")
            {
                return;
            }

            stringstream ss(input);

            int bet;
            char extra;

            // Make sure the input contains only one valid integer.
            if (!(ss >> bet) || (ss >> extra))
            {
                cout << "Invalid bet. Enter a number from 1 to 5.\n";
                continue;
            }

            if (!game.deal(bet))
            {
                cout << "Invalid bet. Check your credits and enter 1-5.\n";
                continue;
            }
        }

        // -------------------------
        // HOLDING / DRAWING
        // -------------------------
        while (game.phase() == Phase::Holding)
        {
            cout << endl;
            printHand();

            cout << "Toggle holds (example: 1 2), "
                 << "or press Enter to draw (q to quit): ";

            string input;
            if (!getline(cin, input)) return;

            if (input == "q" || input == "Q") return;
            // Empty input means the player is ready to draw.
            if (input.empty())
            {
                game.draw();

                cout << endl;
                printHand();

                cout << VideoPoker::rankName(game.lastResult())
                     << "! Pays " << game.lastPayout()
                     << ". Credits: " << game.getCredits()
                     << endl;

                break;
            }

            stringstream ss(input);

            vector<int> indexes;
            int index;
            bool valid = true;

            while (ss >> index)
            {
                if (index < 1 || index > 5)
                {
                    valid = false;
                }
                else
                {
                    indexes.push_back(index - 1);
                }
            }

            // Detect letters or other bad input.
            if (!ss.eof())
            {
                valid = false;
            }

            if (!valid || indexes.empty())
            {
                cout << "Enter card numbers from 1 to 5.\n";
                continue;
            }

            for (int i = 0; i < static_cast<int>(indexes.size()); i++)
            {
                game.toggleHold(indexes[i]);
            }
        }
    }

    cout << "\nGame over. You have 0 credits remaining.\n";
}
