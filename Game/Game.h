#ifndef GAME
#define GAME

#include <algorithm>
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

  friend bool operator==(const Card &lhs, const Card &rhs) {
    if (lhs.suit == RED_JOKER || lhs.suit == BLACK_JOKER)
      return lhs.suit == rhs.suit;
    if (rhs.suit == RED_JOKER || rhs.suit == BLACK_JOKER)
      return lhs.suit == rhs.suit;

    return lhs.number == rhs.number;
  }
  friend bool operator!=(const Card &lhs, const Card &rhs) {
    return !(lhs == rhs);
  }

  bool equal_all(const Card &c) {
    return this->suit == c.suit && this->number == c.number;
  }
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

class Strategy {
private:
  enum Type {
    TYPE_START,
    Single,    // single card
    Double,    // double cards
    Triple,    // three cards
    ThreeOne,  // 三带一
    TreeTwo,   // 三带二
    SingleSeq, // 顺子
    DoubleSeq, // 双顺
    ThreeSeq,  // 三顺
    Airplane,  // 飞机
    FourSeq,   // 四带二（两张或两对）
    Bomb,      // 炸弹
    UltraBomb, // 王炸
    TYPE_END,
  };

  static std::vector<std::vector<Card>>
  get_consecutive_n_cards_set(const std::vector<Card> &current, const int &n) {
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
      // if (index == length - 1)
      //   break;
    }
    return ans;
  }

  static std::vector<std::vector<Card>>
  get_possible_move_by_type(const std::vector<Card> &current, Type type) {
    std::vector<std::vector<Card>> ans;
    size_t length = current.size();
    size_t index = 0;

    // Temp vector for current available card set that can play
    std::vector<Card> set;
    switch (type) {

    case Single: {
      auto ret = get_consecutive_n_cards_set(current, 1);
      ans.insert(ans.end(), ret.begin(), ret.end());
      break;
    }

    case Double: {
      auto ret = get_consecutive_n_cards_set(current, 2);
      ans.insert(ans.end(), ret.begin(), ret.end());
      break;
    }

    case Triple: {
      auto ret = get_consecutive_n_cards_set(current, 3);
      ans.insert(ans.end(), ret.begin(), ret.end());
      break;
    }

      // TODO: implement other card sets

    case Bomb: {
      auto ret = get_consecutive_n_cards_set(current, 4);
      ans.insert(ans.end(), ret.begin(), ret.end());
      break;
    }

    case UltraBomb: {
      if (current[current.size() - 1] == Card(RED_JOKER, -1) &&
          current[current.size() - 2] == Card(BLACK_JOKER, -1)) {
        set = std::vector<Card>(current.end() - 2, current.end());
        ans.insert(ans.end(), set);
      }
      break;
    }
    default:
      break;
    }
    return ans;
  }

  // return current.size() if index == current.size()
  static size_t jump_to_next_number(const std::vector<Card> &current,
                                    size_t index) {
    Card t = current[index];
    while (t == current[index]) {
      index++;
      if (index == current.size())
        return index;
    }
    return index;
  }

  static bool has_consecutive_cards(const std::vector<Card> &current,
                                    size_t index, int num) {
    for (int i = 0; i < num; i++) {
      if (index + i == current.size())
        return false;
      if (current[index + i] != current[index])
        return false;
    }
    return true;
  }

public:
  static std::vector<std::vector<Card>>
  get_possible_move(std::vector<Card> &current) {
    std::sort(current.begin(), current.end());

    std::vector<std::vector<Card>> ans;

    Type type = TYPE_START;
    while (type != TYPE_END) {
      type = (Type)((unsigned int)type + 1);
      auto ret = get_possible_move_by_type(current, type);
      ans.insert(ans.end(), ret.begin(), ret.end());
    }
    return ans;
  }
};

class Game {
private:
  Deck deck;
  int round;
  std::vector<Card> player_1;
  std::vector<Card> player_2;
  std::vector<Card> player_3;
  void print_state();
  bool isGameEnd();

  void remove_card_set(const std::vector<Card> &card_set,
                       std::vector<Card> &hand) {
    for (size_t index = 0; index < card_set.size(); index++) {
      for (auto i = hand.begin(); i != hand.end(); i++) {
        if (i->equal_all(card_set[index])) {
          hand.erase(i);
          break;
        }
      }
    }
  }

public:
  Game() : deck(), round(0), player_1(), player_2(), player_3() {}
  void init();
  void run();
};

#endif