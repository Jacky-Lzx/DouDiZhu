#ifndef GAME
#define GAME

#include "Card.h"
#include "Strategy.h"

class Game {
private:
  Deck deck;
  int round;
  std::vector<std::vector<Card>> players;
  void print_state();
  bool isGameEnd();

  void remove_card_set(const CardSet &card_set, std::vector<Card> &hand);

public:
  Game() : deck(), round(0), players(3, std::vector<Card>()) {}
  void init();
  void run();
};

#endif // GAME