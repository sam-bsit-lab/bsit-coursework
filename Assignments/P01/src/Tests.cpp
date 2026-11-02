#include "Tests.hpp"
#include "VideoPoker.hpp"

#include <initializer_list>
#include <iostream>

using namespace std;

namespace
{
int failures = 0;

// Print one result and remember whether anything failed.
void check(bool passed, const string& name)
{
    cout << (passed ? "PASS " : "FAIL ") << name << '\n';
    if (!passed) failures++;
}

// Card values use the same 0-51 numbering as Card.
void checkHand(initializer_list<int> values, HandRank expected, const string& name)
{
    Hand hand;
    for (int value : values)
        hand.addCard(Card(value));

    check(VideoPoker::evaluate(hand) == expected, name);
}
}

int runTests()
{
    failures = 0;

    // One example of each hand, plus the special straight cases.
    checkHand({1, 16, 32, 47, 11}, HandRank::Nothing, "Nothing");
    checkHand({10, 23, 1, 18, 34}, HandRank::JacksOrBetter, "Pair of Jacks");
    checkHand({1, 14, 5, 18, 34}, HandRank::TwoPair, "Two Pair");
    checkHand({1, 14, 27, 18, 34}, HandRank::ThreeOfAKind, "Three of a Kind");
    checkHand({1, 15, 29, 43, 5}, HandRank::Straight, "Straight");
    checkHand({1, 3, 6, 8, 11}, HandRank::Flush, "Flush");
    checkHand({1, 14, 27, 5, 18}, HandRank::FullHouse, "Full House");
    checkHand({1, 14, 27, 40, 5}, HandRank::FourOfAKind, "Four of a Kind");
    checkHand({1, 2, 3, 4, 5}, HandRank::StraightFlush, "Straight Flush");
    checkHand({0, 9, 10, 11, 12}, HandRank::RoyalFlush, "Royal Flush");
    checkHand({9, 22, 1, 18, 34}, HandRank::Nothing, "Pair of tens does not pay");
    checkHand({0, 14, 28, 42, 4}, HandRank::Straight, "A-2-3-4-5");
    checkHand({0, 22, 36, 50, 12}, HandRank::Straight, "10-J-Q-K-A mixed suits");
    checkHand({11, 25, 26, 40, 2}, HandRank::Nothing, "Q-K-A-2-3 is not a straight");

    // Hold a royal flush and check the actual payout at each bet.
    for (int bet = 1; bet <= 5; bet++)
    {
        VideoPoker royal;
        royal.setNextDeck(Deck({Card(0), Card(9), Card(10), Card(11), Card(12),
                               Card(1), Card(2), Card(3), Card(4), Card(5)}));
        royal.deal(bet);
        for (int i = 0; i < 5; i++)
            royal.toggleHold(i);
        royal.draw();

        int expected = bet == 5 ? 4000 : bet * 250;
        check(royal.lastPayout() == expected &&
              royal.getCredits() == 100 - bet + expected,
              "Royal payout at bet " + to_string(bet));
    }

    // Keep two Jacks and replace the other three cards from the same deck.
    VideoPoker game;
    game.setNextDeck(Deck({Card(10), Card(23), Card(1), Card(18), Card(34),
                          Card(0), Card(2), Card(4), Card(6), Card(8)}));
    check(!game.draw() && !game.toggleHold(0), "Cannot draw or hold before dealing");
    check(!game.deal(0) && !game.deal(6), "Reject invalid bets");
    check(game.deal(5) && game.getCredits() == 95, "Subtract the bet");
    check(!game.deal(1), "Cannot deal twice");
    check(!game.toggleHold(-1) && !game.toggleHold(5), "Reject invalid card positions");
    game.toggleHold(0);
    game.toggleHold(1);
    check(game.draw(), "Draw succeeds");
    check(game.getHand().at(0).getValue() == 10 &&
          game.getHand().at(1).getValue() == 23, "Held cards stay");
    check(game.getHand().at(2).getValue() == 0 &&
          game.getHand().at(3).getValue() == 2 &&
          game.getHand().at(4).getValue() == 4, "Unheld cards are replaced in order");
    check(game.lastResult() == HandRank::JacksOrBetter &&
          game.lastPayout() == 5 && game.getCredits() == 100, "Pair returns the bet");
    check(!game.draw() && game.getCredits() == 100, "Cannot collect a payout twice");

    cout << (failures == 0 ? "All tests passed.\n" : "Some tests failed.\n");
    return failures == 0 ? 0 : 1;
}
