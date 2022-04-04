#include "Game.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <type_traits>

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
  assert(c1.number != 3);
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
    break;
  }
  default: {
    os << t.type;
    break;
  }
  }
  return os;
}

void Deck::init() {
  for (int i = 0; i < 54; i++) {
    cards[i] = Card(i);
  }
}

/**
 * @brief Fisher-Yates shuffles
 */
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
  // TODO

  // 决定地主
  // TODO
  // Currently set landlord to player 0
  int landlord = 0;

  // 亮地主牌
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
    std::cout << "Cards of Player " << i << ": " << std::endl;
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
  int current_player = 0;
  int last_player = -1;
  while (!isGameEnd()) {
    CardSet last_play(TYPE_START, {});

    round++;
    print_state();

    // Test: stop at beginning
    // int cin_temp;
    // std::cin >> cin_temp;

    // do sth
    while (true) {
      if (current_player == last_player) {
        std::cout << "Player " << current_player << " wins this round!"
                  << std::endl;
        last_player = -1;
        break;
      }
      std::cout << "===== Current player: " << current_player
                << " =====" << std::endl;
      std::vector<CardSet> move = Strategy::get_possible_move(
          players[current_player], last_play.get_type());
      move = Strategy::trim_by_last_play(move, last_play);
      int index = 0;
      for (const auto &move : move) {
        std::cout << index++ << " ---\t";
        std::cout << move.get_type() << ": ";
        for (const auto &m : move.get_base()) {
          std::cout << m << " ";
        }
        if (!move.get_extra().empty()) {
          std::cout << " +++ ";
          for (const auto &m : move.get_extra()) {
            std::cout << m << " ";
          }
        }
        std::cout << std::endl;
      }
      if (index == 0) {
        std::cout << "Player " << current_player << " doesn't have choice."
                  << std::endl;
      } else {
        int choice;
        std::cin >> choice;
        while (choice < -1 || choice >= index) {
          std::cout << "Invaid index, choose again: " << std::endl;
          std::cin >> choice;
        }
        // The first player cannot give up
        if (last_play.get_type() == TYPE_START) {
          while (choice == -1) {
            std::cout << "The first one can not give up. Repeat: " << std::endl;
            std::cin >> choice;
          }
        }
        if (choice == -1) {
          std::cout << "Player " << current_player << " gives no choice."
                    << std::endl;
        } else {
          assert(choice >= 0 && choice < index);
          remove_card_set(move[choice], players[current_player]);
          last_play = move[choice];
          last_player = current_player;
        }
      }
      current_player = (current_player + 1) % 3;
    }
  }
  for (int i = 0; i < 3; i++) {
    if (players[i].empty()) {
      std::cout << "Player " << i << " wins the game!" << std::endl;
      return;
    }
  }
}
bool operator==(const Card &lhs, const Card &rhs) {
  if (lhs.suit == RED_JOKER || lhs.suit == BLACK_JOKER)
    return lhs.suit == rhs.suit;
  if (rhs.suit == RED_JOKER || rhs.suit == BLACK_JOKER)
    return lhs.suit == rhs.suit;

  return lhs.number == rhs.number;
}
bool operator!=(const Card &lhs, const Card &rhs) { return !(lhs == rhs); }

bool Card::equal_all(const Card &c) {
  return this->suit == c.suit && this->number == c.number;
}

std::vector<std::vector<Card>>
Strategy::get_consecutive_n_cards_set(const std::vector<Card> &current,
                                      const int &n) {
  std::vector<std::vector<Card>> ans;
  // Temp vector for current available card set that can play
  std::vector<Card> set;

  size_t index = 0;
  size_t length = current.size();
  while (index < length) {
    if (has_consecutive_cards(current, index, n)) {
      set = std::vector<Card>();
      for (int i = 0; i < n; i++) {
        set.push_back(current[index + i]);
      }
      ans.push_back(set);
    }

    index = jump_to_next_number(current, index);
  }
  return ans;
}

std::vector<std::vector<Card>>
Strategy::get_sequence(const std::vector<Card> &current,
                       const int &consecutive_num, const int &length) {
  std::vector<std::vector<Card>> ans;
  std::vector<Card> temp;
  for (size_t index = 0; index < current.size();
       index = jump_to_next_number(current, index)) {
    if (current[index] == Card(SPADE, 2) ||
        current[index] == Card(BLACK_JOKER) ||
        current[index] == Card(RED_JOKER)) {
      break;
    }
    temp = std::vector<Card>();
    size_t length_temp = 0;
    size_t index_temp = index;
    while (length_temp < length) {
      size_t num_temp = 0;
      size_t first = index_temp;
      size_t second = first;

      while (current[first] == current[second]) {
        num_temp++;
        second++;
        if (num_temp == consecutive_num) {
          temp.insert(temp.end(), current.begin() + first,
                      current.begin() + second);
          break;
        }
      }
      if (num_temp != consecutive_num) {
        break;
      }

      length_temp++;
      index_temp = jump_to_next_number(current, index_temp);
      if (current[first] != current[index_temp] - 1) {
        break;
      }
    }
    if (length_temp != length) {
      temp.clear();
    } else {
      ans.push_back(temp);
    }
  }
  return ans;
}

std::vector<CardSet>
Strategy::get_possible_move_by_type(const std::vector<Card> &current,
                                    Type type) {
  std::vector<CardSet> ans;

  // Temp vector for current available card set that can play
  std::vector<Card> set;
  switch (type.get_type_t()) {
  case Single: {
    auto ret = get_consecutive_n_cards_set(current, 1);
    for (const auto &r : ret) {
      ans.push_back(CardSet(type, r));
    }
    break;
  }

  case Double: {
    auto ret = get_consecutive_n_cards_set(current, 2);
    for (const auto &r : ret) {
      ans.push_back(CardSet(type, r));
    }
    break;
  }

  case Triple: {
    auto ret = get_consecutive_n_cards_set(current, 3);
    for (const auto &r : ret) {
      ans.push_back(CardSet(type, r));
    }
    break;
  }

  case SingleSeq: {
    auto ret = get_sequence(current, 1, type.get_length());
    for (const auto &r : ret) {
      ans.push_back(CardSet(type, r));
    }
    break;
  }

  case DoubleSeq: {
    auto ret = get_sequence(current, 2, type.get_length());
    for (const auto &r : ret) {
      ans.push_back(CardSet(type, r));
    }
    break;
  }

  case ThreeSeq: {
    auto ret = get_sequence(current, 3, type.get_length());
    for (const auto &r : ret) {
      ans.push_back(CardSet(type, r));
    }
    break;
  }

  case ThreeOne: {
    auto three = get_consecutive_n_cards_set(current, 3);
    auto one = get_consecutive_n_cards_set(current, 1);
    for (const auto &t : three) {
      for (const auto &o : one) {
        if (t[0] == o[0])
          continue;
        std::vector<Card> temp;
        temp.push_back(o[0]);
        ans.push_back(CardSet(type, t, temp));
      }
    }
    break;
  }

  case ThreeTwo: {
    auto three = get_consecutive_n_cards_set(current, 3);
    auto two = get_consecutive_n_cards_set(current, 2);
    for (const auto &t : three) {
      for (const auto &o : two) {
        if (t[0] == o[0])
          continue;
        std::vector<Card> temp;
        temp.insert(temp.end(), o.begin(), o.end());
        ans.push_back(CardSet(type, t, temp));
      }
    }
    break;
  }

  case Four_Two_Pair: {
    auto four = get_consecutive_n_cards_set(current, 4);
    auto two = get_consecutive_n_cards_set(current, 2);
    for (const auto &f : four) {
      for (size_t t1 = 0; t1 < two.size(); t1++) {
        for (size_t t2 = t1 + 1; t2 < two.size(); t2++) {
          if (f[0] == two[t1][0] || f[0] == two[t2][0])
            continue;
          std::vector<Card> temp;
          temp.insert(temp.end(), two[t1].begin(), two[t1].end());
          temp.insert(temp.end(), two[t2].begin(), two[t2].end());
          ans.push_back(CardSet(type, f, temp));
        }
      }
    }
    break;
  }

  case Four_Two_Single: {
    auto four = get_consecutive_n_cards_set(current, 4);
    auto one = get_consecutive_n_cards_set(current, 1);
    for (const auto &f : four) {
      for (size_t o1 = 0; o1 < one.size(); o1++) {
        for (size_t o2 = o1 + 1; o2 < one.size(); o2++) {
          if (f[0] == one[o1][0] || f[0] == one[o2][0])
            continue;
          std::vector<Card> temp;
          temp.insert(temp.end(), one[o1].begin(), one[o1].end());
          temp.insert(temp.end(), one[o2].begin(), one[o2].end());
          ans.push_back(CardSet(type, f, temp));
        }
      }
    }
    break;
  }

  case Airplane_Single: {
    // TODO
    // auto three_two = get_sequence(current, 3, 2);
    // auto one = get_consecutive_n_cards_set(current, 1);
    // for (const auto &t : three_two) {
    //   for (size_t o1 = 0; o1 < one.size(); o1++) {
    //     for (size_t o2 = o1 + 1; o2 < one.size(); o2++) {
    //     }
    //   }
    // }
    break;
  }

  case Airplane_Pair: {
    // TODO
    break;
  }

  case Bomb: {
    auto ret = get_consecutive_n_cards_set(current, 4);
    for (const auto &r : ret) {
      ans.push_back(CardSet(type, r));
    }
    break;
  }

  case UltraBomb: {
    if (current[current.size() - 1] == Card(RED_JOKER, -1) &&
        current[current.size() - 2] == Card(BLACK_JOKER, -1)) {
      set = std::vector<Card>(current.end() - 2, current.end());
      ans.push_back(CardSet(type, set));
    }
    break;
  }
  default:
    break;
  }
  return ans;
}

size_t Strategy::jump_to_next_number(const std::vector<Card> &current,
                                     size_t index) {
  Card t = current[index];
  while (t == current[index]) {
    index++;
    if (index == current.size())
      return index;
  }
  return index;
}

bool Strategy::has_consecutive_cards(const std::vector<Card> &current,
                                     size_t index, int num) {
  for (int i = 0; i < num; i++) {
    if (index + i == current.size())
      return false;
    if (current[index + i] != current[index])
      return false;
  }
  return true;
}

std::vector<Type> Strategy::get_possible_types(Type current_type) {
  std::vector<Type> types;
  switch (current_type.get_type_t()) {
  case TYPE_START: {
    type_t ptr = (type_t)((int)TYPE_START + 1);
    while (ptr != TYPE_END) {
      if (ptr == SingleSeq) {
        for (int i = 5; i <= 12; i++) {
          types.push_back(Type(ptr, i));
        }
      } else if (ptr == DoubleSeq) {
        for (int i = 3; i <= 12; i++) {
          types.push_back(Type(ptr, i));
        }
      } else if (ptr == ThreeSeq) {
        for (int i = 2; i <= 12; i++) {
          types.push_back(Type(ptr, i));
        }
      } else {
        types.push_back(Type(ptr));
      }
      ptr = (type_t)((int)ptr + 1);
    }
    break;
  }
  case Single:
  case Double:
  case Triple:

  case ThreeOne:
  case ThreeTwo:

  case SingleSeq:
  case DoubleSeq:
  case ThreeSeq:

  case Airplane_Pair:
  case Airplane_Single:

  case Four_Two_Single:
  case Four_Two_Pair: {
    types.push_back(current_type);
    types.push_back(Type(Bomb));
    types.push_back(Type(UltraBomb));
    break;
  }

  case Bomb: {
    types.push_back(Type(Bomb));
    types.push_back(Type(UltraBomb));
    break;
  }
  case UltraBomb: {
    break;
  }
  case TYPE_END: {
    assert(0);
  }
  }
  return types;
}

std::vector<CardSet> Strategy::get_possible_move(std::vector<Card> &current,
                                                 Type current_type) {
  std::sort(current.begin(), current.end());

  std::vector<CardSet> ans;

  std::vector<Type> possible_types = get_possible_types(current_type);

  for (const auto &p : possible_types) {
    auto ret = get_possible_move_by_type(current, p);
    ans.insert(ans.end(), ret.begin(), ret.end());
  }
  return ans;
}

std::vector<CardSet> Strategy::trim_by_last_play(std::vector<CardSet> &current,
                                                 CardSet last_play) {
  std::vector<CardSet> ans;
  for (const auto &c : current) {
    if (last_play < c) {
      ans.push_back(c);
    }
  }
  return ans;
}

bool operator==(const Type &t1, const Type &t2) {
  return (t1.type == t2.type) && (t1.length == t2.length);
}

bool operator<=(const Type &t1, const Type &t2) { return t1 < t2 || t1 == t2; }

bool operator<(const Type &t1, const Type &t2) {
  switch (t1.type) {
  case TYPE_START:
    return true;
  case Single:
  case Double:
  case Triple:

  case SingleSeq:
  case DoubleSeq:
  case ThreeSeq:

  case ThreeOne:
  case ThreeTwo:

  case Airplane_Single:
  case Airplane_Pair:

  case Four_Two_Single:
  case Four_Two_Pair:
    return t2.type == Bomb || t2.type == UltraBomb;

  case Bomb:
    return t2.type == UltraBomb;

  case UltraBomb:
  case TYPE_END: {
    return false;
    break;
  }
  }
}

bool operator<(const CardSet &c1, const CardSet &c2) {
  if (c1.type < c2.type) {
    return true;
  }
  if (c1.type == c2.type) {
    switch (c1.get_type().get_type_t()) {
    case TYPE_START:
      // should not go here
      assert(0);
    case Single:
    case Double:
    case Triple:
      return c1.base[0] < c2.base[0];

    case SingleSeq:
    case DoubleSeq:
    case ThreeSeq:
      return c1.base[0] < c2.base[0];

    case ThreeOne:
    case ThreeTwo:
      return c1.base[0] < c2.base[0];

    case Airplane_Single:
    case Airplane_Pair:
      return c1.base[0] < c2.base[0];

    case Four_Two_Single:
    case Four_Two_Pair:
      return c1.base[0] < c2.base[0];

    case Bomb:
      return c1.base[0] < c2.base[0];

    case UltraBomb:
    case TYPE_END:
      assert(0);
    }
    assert(0);
  }
  return false;
}

void Game::remove_card_set(const CardSet &card_set, std::vector<Card> &hand) {
  // TODO: Improve it later for performance
  for (const auto &b : card_set.get_base()) {
    for (auto i = hand.begin(); i != hand.end(); i++) {
      if (i->equal_all(b)) {
        hand.erase(i);
        break;
      }
    }
  }
  for (const auto &e : card_set.get_extra()) {
    for (auto i = hand.begin(); i != hand.end(); i++) {
      if (i->equal_all(e)) {
        hand.erase(i);
        break;
      }
    }
  }
}