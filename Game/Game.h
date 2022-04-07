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

  /**
   * @brief Decide who is the landlord.
   *
   * @param landlord If the landlord is decided, its index will be stored in
   this variable
   * @return true The landlord is successfully decided
   * @return false The landlord is not decided, reshuffle and assing cards
   */
  bool decide_landlord(int &landlord);

public:
  Game() : deck(), round(0), players(3, std::vector<Card>()) {}
  void init();
  void run();
};

#endif // GAME