#include "Game.h"
#include <cstdlib>

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
Card operator-(const Card &c1, const int &i) {
  assert(i == 1);
  if (c1.number == 1) {
    return Card(c1.suit, 13);
  }
  return Card(c1.suit, c1.number - i);
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

inline std::ostream &operator<<(std::ostream &os, const Type &t) {
  switch (t.type) {
  case SingleSeq:
  case DoubleSeq:
  case ThreeSeq: {
    os << t.type << "+" << t.length;
  }
  default: {
    os << t.type;
  }
  }
  return os;
}

void Deck::init() {
  for (int i = 0; i < 54; i++) {
    cards[i] = Card(i);
  }
}

void Deck::shuffle() {
  srand(time(NULL));
  for (int i = 0; i < 54 - 1; i++) {
    int j = i + (rand() % (54 - i));
    std::swap(cards[i], cards[j]);
  }
}

void Game::init() {
  // Offering cards
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 17; j++) {
      players[i].push_back(deck.pick());
    }
  }
  print_state();

  // 抢地主

  // 决定地主
  int landlord = 0;

  // 亮牌
  std::vector<Card> landlord_cards;
  std::cout << "Cards of landlord: ";
  for (int i = 0; i < 3; i++) {
    landlord_cards.push_back(deck.pick());
    std::cout << landlord_cards.back() << " ";
  }
  std::cout << std::endl;
  players[landlord].insert(players[landlord].end(), landlord_cards.begin(),
                           landlord_cards.end());

  for (auto &p : players) {
    std::sort(p.begin(), p.end());
  }
  std::cout << "After sort: " << std::endl;
  print_state();
}

void Game::print_state() {
  std::cout << "Round: " << round << std::endl;

  for (size_t i = 0; i < 3; i++) {
    std::cout << "Cards of Player " << i + 1 << ": " << std::endl;
    std::cout << "\t";
    for (const auto &c : players[i]) {
      std::cout << c << " ";
    }
    std::cout << std::endl;
  }
}

bool Game::isGameEnd() {
  return (players[0].size() == 0 || players[1].size() == 0 ||
          players[2].size() == 0);
}

void Game::run() {
  Type current_status = TYPE_START;
  std::vector<Card> last_play;
  while (!isGameEnd()) {
    round++;
    std::cout << "Round: " << round << std::endl;

    print_state();

    // do sth
    int current_player = 0;
    while (true) {
      std::cout << "===== Current player: " << current_player << " =====" << std::endl;
      std::vector<std::pair<Type, std::vector<Card>>> move =
          Strategy::get_possible_move(players[current_player], current_status);
      move = Strategy::trim_by_last_play(move, current_status, last_play);
      int index = 0;
      for (const auto &move : move) {
        std::cout << index++ << " ---\t";
        std::cout << move.first << ": ";
        for (const auto &m : move.second) {
          std::cout << m << " ";
        }
        std::cout << std::endl;
      }
      if (index == 0) break;
      int choice;
      std::cin >> choice;
      assert(choice >= 0 && choice < index);
      remove_card_set(move[choice].second, players[current_player]);
      current_status = move[choice].first;
      last_play = move[choice].second;

      current_player = (current_player + 1) % 3;
    }
    break;
  }
}