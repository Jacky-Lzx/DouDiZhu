#ifndef GAME
#define GAME

#include <cassert>
#include <iostream>
#include <vector>

enum Suit { SPADE, HEART, DIAMOND, CLUB, RED_JOKER, BLACK_JOKER };

class Card {
  Suit suit;
  // In range [1, 13]
  int number;

public:
  Card() = default;
  Card(Suit _suit, int _number) : suit(_suit), number(_number) {}
  Card(int num);
  friend std::ostream &operator<<(std::ostream &os, const Card &c);
  friend bool operator<(const Card &c1, const Card &c2);
};

class Deck {
private:
  Card cards[54];

  // Indicating current index of the first card in the deck
  int index;

  void init();
  void shuffle();

public:
  Deck() : index(0) {
    init();
    shuffle();
  }

  Card pick() { return cards[index++]; }
};

class Game {
private:
  Deck deck;
  int round;
  std::vector<Card> player_1;
  std::vector<Card> player_2;
  std::vector<Card> player_3;
  void print_state();

public:
  Game() : deck(), round(0), player_1(), player_2(), player_3() {}
  void init();
};

#endif