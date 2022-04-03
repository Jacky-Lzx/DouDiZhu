#include "Game.h"

Card::Card(int num) {
  assert(num >= 0 && num < 54 && "Card num out of bound");
  if (num == 52) {
    suit = BLACK_JOKER;
    number = -1;
    return;
  } else if (num == 53) {
    suit = RED_JOKER;
    number = -1;
    return;
  }

  suit = (Suit)(num / 13);
  number = num - ((int)suit * 13) + 1;
}

bool operator<(const Card &c1, const Card &c2) {
  if (c1.suit == RED_JOKER || c1.suit == RED_JOKER) {
    return false;
  }
  if (c2.suit == RED_JOKER || c2.suit == RED_JOKER) {
    return true;
  }
  if (c1.suit == BLACK_JOKER || c1.suit == BLACK_JOKER) {
    return false;
  }
  if (c2.suit == BLACK_JOKER || c2.suit == BLACK_JOKER) {
    return true;
  }

  return (c1.number < 3 ? c1.number + 13 : c1.number) <
         (c2.number < 3 ? c2.number + 13 : c2.number);
}

inline std::ostream &operator<<(std::ostream &os, const Card &c) {
  os << "[";
  switch (c.suit) {
  case SPADE:
    os << "Spade";
    os << " " << c.number << "]";
    break;
  case HEART:
    os << "Heart";
    os << " " << c.number << "]";
    break;
  case DIAMOND:
    os << "Diamond";
    os << " " << c.number << "]";
    break;
  case CLUB:
    os << "Club";
    os << " " << c.number << "]";
    break;
  case RED_JOKER:
    os << "Red Joker";
    os << "]";
    break;
  case BLACK_JOKER:
    os << "Black Joker";
    os << "]";
    break;
  }
  return os;
}

void Deck::init() {
  for (int i = 0; i < 54; i++) {
    cards[i] = Card(i);
  }
}

void Deck::shuffle() {
  for (int i = 0; i < 54 - 1; i++) {
    int j = i + (rand() % (54 - i));
    std::swap(cards[i], cards[j]);
  }
}

void Game::init() {
  // Offering cards
  for (int j = 0; j < 17; j++) {
    player_1.push_back(deck.pick());
  }
  for (int j = 0; j < 17; j++) {
    player_2.push_back(deck.pick());
  }
  for (int j = 0; j < 17; j++) {
    player_3.push_back(deck.pick());
  }
  print_state();

  // 抢地主

  // 决定地主
  int landlord = 1;

  // 亮牌
  std::vector<Card> landlord_cards;
  std::cout << "Cards of landlord: ";
  for (int i = 0; i < 3; i++) {
    landlord_cards.push_back(deck.pick());
    std::cout << landlord_cards.back() << " ";
  }
  std::cout << std::endl;
  switch (landlord) {
  case 1:
    player_1.insert(player_1.end(), landlord_cards.begin(),
                    landlord_cards.end());
    break;
  case 2:
    player_2.insert(player_2.end(), landlord_cards.begin(),
                    landlord_cards.end());
    break;
  case 3:
    player_3.insert(player_3.end(), landlord_cards.begin(),
                    landlord_cards.end());
    break;
  default:
    assert(0);
  }

  print_state();

  std::sort(player_1.begin(), player_1.end());
  std::sort(player_2.begin(), player_2.end());
  std::sort(player_3.begin(), player_3.end());
  std::cout << "After sort: " << std::endl;
  print_state();
}

void Game::print_state() {
  std::cout << "Round: " << round << std::endl;

  std::cout << "Cards of Player 1: " << std::endl;
  std::cout << "\t";
  for (const auto &c : player_1) {
    std::cout << c << " ";
  }
  std::cout << std::endl;

  std::cout << "Cards of Player 2: " << std::endl;
  std::cout << "\t";
  for (const auto &c : player_2) {
    std::cout << c << " ";
  }
  std::cout << std::endl;

  std::cout << "Cards of Player 3: " << std::endl;
  std::cout << "\t";
  for (const auto &c : player_3) {
    std::cout << c << " ";
  }
  std::cout << std::endl;
}