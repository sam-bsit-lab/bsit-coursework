// include/Hand.hpp
#ifndef HAND_HPP
#define HAND_HPP

#include <vector>
#include "Card.hpp"

class Hand
{
private:
    std::vector<Card> cards;

public:
    void addCard(const Card& card);
    void clear();
    int size() const;
    const Card& at(int index) const;
    void replace(int index, const Card& card);
};

#endif