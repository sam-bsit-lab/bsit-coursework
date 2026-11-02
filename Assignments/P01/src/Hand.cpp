// src/Hand.cpp
#include "Hand.hpp"

void Hand::addCard(const Card& card)
{
    cards.push_back(card);
}

void Hand::clear()
{
    cards.clear();
}

int Hand::size() const
{
    return static_cast<int>(cards.size());
}

const Card& Hand::at(int index) const
{
    return cards.at(index);
}

void Hand::replace(int index, const Card& card)
{
    cards.at(index) = card;
}
