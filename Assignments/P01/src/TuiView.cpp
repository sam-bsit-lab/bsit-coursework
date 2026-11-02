#include "TuiView.hpp"

#include <ftxui/component/component.hpp> // Renderer and keyboard events
#include <ftxui/component/screen_interactive.hpp> // Screen and event loop
#include <ftxui/dom/elements.hpp> // Text, cards, colors, and layout

using namespace ftxui;
using namespace std;

namespace
{
// Based on the card layout in rugbyprof's Resources/ftxui/main.cpp.
Element drawCard(const Card& card, bool selected, bool held, int position)
{
    string rank = card.rankSymbol();
    string suit = card.suitSymbol();
    auto face = vbox({
        hbox({text(rank), filler(), text(suit)}),
        filler(),
        text(suit) | bold | center,
        filler(),
        hbox({text(suit), filler(), text(rank)})
    }) | color(card.isRed() ? Color::Red : Color::White)
       | size(WIDTH, EQUAL, 7) | size(HEIGHT, EQUAL, 5);

    face = selected ? face | borderDouble : face | border;
    auto label = text(held ? "HOLD" : to_string(position)) | center;
    if (held) label = label | color(Color::Yellow) | bold;
    else label = label | dim;
    return vbox({face, label});
}
}

TuiView::TuiView(VideoPoker& game) : game(game) {}

void TuiView::run()
{
    auto screen = ScreenInteractive::TerminalOutput();
    int selected = 0;
    int wager = 1;
    string message = "Choose a bet, then press Enter to deal.";

    auto renderer = Renderer([&] {
        bool holding = game.phase() == Phase::Holding;
        bool finished = !holding && game.getHand().size() == 5;
        int shownBet = holding ? game.bet() : wager;

        Elements cards;
        for (int i = 0; i < 5; i++)
        {
            if (i > 0) cards.push_back(text(" "));
            if (game.getHand().size() == 5)
                cards.push_back(drawCard(game.getHand().at(i), holding && i == selected,
                                         game.isHeld(i), i + 1));
            else
                cards.push_back(vbox({
                    text("?") | center | color(Color::Cyan)
                        | size(WIDTH, EQUAL, 7) | size(HEIGHT, EQUAL, 5) | border,
                    text(to_string(i + 1)) | center | dim
                }));
        }

        Elements paytable;
        for (int i = static_cast<int>(HandRank::RoyalFlush);
             i >= static_cast<int>(HandRank::JacksOrBetter); i--)
        {
            HandRank rank = static_cast<HandRank>(i);
            auto row = hbox({
                text(VideoPoker::rankName(rank)),
                filler(),
                text(" " + to_string(shownBet * VideoPoker::multiplier(rank, shownBet)))
            });
            if (finished && rank == game.lastResult())
                row = row | color(Color::Yellow) | bold;
            paytable.push_back(row);
        }

        // Two short columns leave room for the cards and controls.
        Elements leftPays(paytable.begin(), paytable.begin() + 5);
        Elements rightPays(paytable.begin() + 5, paytable.end());
        auto payoutPanel = hbox({
            vbox(leftPays) | flex, text("    "), vbox(rightPays) | flex
        });

        Color messageColor = Color::Cyan;
        if (finished)
            messageColor = game.lastPayout() > 0 ? Color::Green : Color::White;
        if (game.phase() == Phase::GameOver) messageColor = Color::Red;

        string phaseLabel = holding ? "HOLD / DRAW" : "PLACE YOUR BET";
        string help = "Up/Down: bet    Enter: deal    q: quit";
        if (holding)
            help = "Left/Right: select    Space or 1-5: hold    Enter: draw    q: quit";
        if (game.phase() == Phase::GameOver)
        {
            phaseLabel = "GAME OVER";
            help = "No credits left. Press q to quit.";
        }

        return vbox({
            text("JACKS OR BETTER") | color(Color::Cyan) | bold | center,
            hbox({text(" Credits: " + to_string(game.getCredits())) | bold,
                  filler(), text(phaseLabel) | color(Color::Yellow),
                  filler(), text("Bet: " + to_string(shownBet) + " ") | bold}),
            separator(),
            window(text(" Your hand "), vbox({
                hbox(cards) | center,
                text(message) | color(messageColor) | bold | center
            })),
            window(text(" Pays at bet " + to_string(shownBet) + " "), payoutPanel),
            text(help) | center
        }) | borderRounded;
    });
    // The view translates keys into calls; VideoPoker decides what is legal.
    auto app = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q') || event == Event::Character('Q'))
        {
            screen.Exit();
            return true;
        }
        if (game.phase() == Phase::Betting)
        {
            if (event == Event::ArrowUp && wager < 5) wager++;
            else if (event == Event::ArrowDown && wager > 1) wager--;
            else if (event == Event::Return)
            {
                if (game.deal(wager)) { selected = 0; message = "Choose your holds, then press Enter to draw."; }
                else message = "You do not have enough credits for that bet.";
            }
            else return false;
            return true;
        }
        if (game.phase() == Phase::Holding)
        {
            if (event == Event::ArrowLeft) selected = (selected + 4) % 5;
            else if (event == Event::ArrowRight) selected = (selected + 1) % 5;
            else if (event == Event::Character(' ')) game.toggleHold(selected);
            else if (event == Event::Return)
            {
                game.draw();
                message = VideoPoker::rankName(game.lastResult()) +
                          " pays " + to_string(game.lastPayout()) + ". Press Enter for the next hand.";
            }
            else
            {
                for (int i = 0; i < 5; i++)
                {
                    if (event == Event::Character(static_cast<char>('1' + i)))
                    {
                        game.toggleHold(i);
                        return true;
                    }
                }
                return false;
            }
            return true;
        }
        return false;
    });

    screen.Loop(app);
}
