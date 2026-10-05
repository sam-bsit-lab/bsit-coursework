#include <iostream>
#include <vector>
#include <random>
#include <string>


using namespace std;

enum class Suit{
    Clubs = 0,
    Diamonds,
    Hearts,
    Spades,
}

vector<string> Ranks = {"","Two,"}


class Card {
private:
    int value;
public:
    Card(int value = 0);

    int getValue() const;
    int getSuit() const;
    int getRank() const;

    string getRankName() const;
    string getSuitName() const;
    string toString() const;
};

string Card::getRankName() const{
    return Rank::
}

class Hand {
private:
    vector<Card> hand;
public:
    void addCard(const Card& card);
    void clear();
    int size();
    
}

class Deck {
private:
    vector<Card> deck;
public:
    Deck();
    void shuffle();
    Card deal();
    bool empty() const;
    int size() const;
};

Deck::Deck() {
    for(int i=0;i<52;i++){
        deck.push_back(Card(i));
    }
}

void Deck::shuffle() {
    static std::random_device rd;
    static::mt19937 gen(rd());

    std::shuffle(deck.begin(),deck.end(),gen);
}

Card Deck::deal(){
    CArd card = deck.back();
    deck.pop_back();
    return card;
}

bool Deck::empty() const{
    return deck.size() = 0;
}

int Deck::size() const{
    return deck.size();
}

void Hand::addCard(const Card& card) {
    hand.push_back(card);
}

void Hand::clear(){
    hand.clear();
}

int Hand::size() const{
    return hand.size();
}

void Hand::show() {

}