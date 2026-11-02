#include "VideoPoker.hpp"

#include <algorithm>
#include <stdexcept>

using namespace std;

VideoPoker::VideoPoker(int startingCredits)
{
    if (startingCredits < 0)
        throw invalid_argument("Starting credits cannot be negative.");

    credits = startingCredits;
    currentBet = 0;
    payout = 0;
    result = HandRank::Nothing;
    currentPhase = credits == 0 ? Phase::GameOver : Phase::Betting;
    held.assign(5, false);
}

// Tests can choose the next ten cards instead of shuffling.
bool VideoPoker::setNextDeck(const Deck& stacked)
{
    if (currentPhase != Phase::Betting || stacked.size() < 10)
        return false;

    deck = stacked;
    useStackedDeck = true;
    return true;
}

bool VideoPoker::deal(int bet)
{
    if (currentPhase != Phase::Betting || bet < 1 || bet > 5 || bet > credits)
        return false;

    if (!useStackedDeck)
    {
        deck.reset();
        deck.shuffle();
    }
    useStackedDeck = false;

    hand.clear();
    held.assign(5, false);
    currentBet = bet;
    credits -= bet;
    payout = 0;
    result = HandRank::Nothing;

    for (int i = 0; i < 5; i++)
        hand.addCard(deck.deal());

    currentPhase = Phase::Holding;
    return true;
}

bool VideoPoker::toggleHold(int index)
{
    if (currentPhase != Phase::Holding || index < 0 || index >= hand.size())
        return false;

    held[index] = !held[index];
    return true;
}

bool VideoPoker::draw()
{
    if (currentPhase != Phase::Holding)
        return false;

    for (int i = 0; i < hand.size(); i++)
    {
        if (!held[i])
            hand.replace(i, deck.deal());
    }

    result = evaluate(hand);
    payout = currentBet * multiplier(result, currentBet);
    credits += payout;
    currentBet = 0;
    held.assign(5, false);
    currentPhase = credits == 0 ? Phase::GameOver : Phase::Betting;
    return true;
}

// These just give the views the information they need.
Phase VideoPoker::phase() const { return currentPhase; }
int VideoPoker::getCredits() const { return credits; }
int VideoPoker::bet() const { return currentBet; }
const Hand& VideoPoker::getHand() const { return hand; }
HandRank VideoPoker::lastResult() const { return result; }
int VideoPoker::lastPayout() const { return payout; }

bool VideoPoker::isHeld(int index) const
{
    return index >= 0 && index < static_cast<int>(held.size()) && held[index];
}

HandRank VideoPoker::evaluate(const Hand& hand)
{
    if (hand.size() != 5)
        return HandRank::Nothing;

    int counts[13] = {};
    vector<int> ranks;
    bool flush = true;
    for (int i = 0; i < 5; i++)
    {
        int rank = hand.at(i).getRank();
        counts[rank]++;
        ranks.push_back(rank);
        if (hand.at(i).getSuit() != hand.at(0).getSuit())
            flush = false;
    }
    sort(ranks.begin(), ranks.end());

    // Ace is 0, so A-2-3-4-5 already counts as consecutive.
    bool straight = true;
    for (int i = 1; i < 5; i++)
    {
        if (ranks[i] != ranks[i - 1] + 1)
            straight = false;
    }
    bool royalRanks = ranks == vector<int>({0, 9, 10, 11, 12});
    straight = straight || royalRanks;

    int pairs = 0;
    bool three = false;
    bool four = false;
    bool highPair = false;
    for (int rank = 0; rank < 13; rank++)
    {
        if (counts[rank] == 4) four = true;
        if (counts[rank] == 3) three = true;
        if (counts[rank] == 2)
        {
            pairs++;
            // Ace, Jack, Queen, or King.
            if (rank == 0 || rank >= 10) highPair = true;
        }
    }

    // Strongest first: a straight flush must be checked before a flush.
    if (flush && royalRanks) return HandRank::RoyalFlush;
    if (flush && straight) return HandRank::StraightFlush;
    if (four) return HandRank::FourOfAKind;
    if (three && pairs == 1) return HandRank::FullHouse;
    if (flush) return HandRank::Flush;
    if (straight) return HandRank::Straight;
    if (three) return HandRank::ThreeOfAKind;
    if (pairs == 2) return HandRank::TwoPair;
    if (highPair) return HandRank::JacksOrBetter;
    return HandRank::Nothing;
}

// Tables follow the HandRank order in VideoPoker.hpp.
int VideoPoker::multiplier(HandRank rank, int bet)
{
    const int payouts[] = {0, 1, 2, 3, 4, 6, 9, 25, 50, 250};
    int index = static_cast<int>(rank);
    if (index < 0 || index >= 10)
        return 0;
    if (rank == HandRank::RoyalFlush && bet == 5)
        return 800;
    return payouts[index];
}

string VideoPoker::rankName(HandRank rank)
{
    const string names[] = {
        "Nothing", "Jacks or Better", "Two Pair", "Three of a Kind",
        "Straight", "Flush", "Full House", "Four of a Kind",
        "Straight Flush", "Royal Flush"
    };
    int index = static_cast<int>(rank);
    if (index < 0 || index >= 10)
        return "Nothing";
    return names[index];
}
