#ifndef VIDEOPOKER_HPP
#define VIDEOPOKER_HPP

#include <string>
#include <vector>

#include "Deck.hpp"
#include "Hand.hpp"

enum class Phase
{
    Betting,
    Holding,
    GameOver
};

enum class HandRank
{
    Nothing,
    JacksOrBetter,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOfAKind,
    StraightFlush,
    RoyalFlush
};

class VideoPoker
{
private:
    Deck deck;
    bool useStackedDeck = false;
    Hand hand;
    std::vector<bool> held;

    Phase currentPhase;
    HandRank result;

    int credits;
    int currentBet;
    int payout;

public:
    VideoPoker(int startingCredits = 100);

    // Testing hook: at least ten cards, used for the next deal only.
    bool setNextDeck(const Deck& stacked);
    bool deal(int bet);
    bool toggleHold(int index);
    bool draw();

    Phase phase() const;
    int getCredits() const;
    int bet() const;
    const Hand& getHand() const;
    bool isHeld(int index) const;
    HandRank lastResult() const;
    int lastPayout() const;

    static HandRank evaluate(const Hand& h);
    static int multiplier(HandRank rank, int bet);
    static std::string rankName(HandRank rank);
};

#endif
