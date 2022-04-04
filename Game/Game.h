#ifndef GAME
#define GAME

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
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

enum type_t {
  TYPE_START, // no type, can use all kinds of types
  Single,     // single card
  Double,     // double cards
  Triple,     // three cards

  SingleSeq, // 顺子
  DoubleSeq, // 双顺
  ThreeSeq,  // 三顺

  // Airplane,   // 飞机
  ThreeOne,           // 三带一
  ThreeTwo,           // 三带二
  FourSeq_Two_Single, // 四带二（两张）
  FourSeq_Two_Pair,   // 四带二（两对）

  Bomb,      // 炸弹
  UltraBomb, // 王炸
  TYPE_END,  // no type, can not use any kind of types
};

class Type {
private:
  type_t type;
  int length;

public:
  Type() = delete;
  Type(type_t _type) : type(_type), length(0) {
    assert(_type != SingleSeq && _type != DoubleSeq && _type != ThreeSeq);
  }
  Type(type_t _type, int _length) : type(_type), length(_length) {
    assert(_type == SingleSeq || _type == DoubleSeq || _type == ThreeSeq);
  }
  type_t get_type() { return type; }
  int get_length() { return length; }

  friend std::ostream &operator<<(std::ostream &os, const Type &t);
};

class CardSet {
private:
  Type type;
  std::vector<Card> base;
  std::vector<Card> extra;

public:
  CardSet() = delete;
  CardSet(Type _type, std::vector<Card> _base) : type(_type), base(_base) {}
  CardSet(Type _type, std::vector<Card> _base, std::vector<Card> _extra)
      : CardSet(_type, _base) {
    switch (_type.get_type()) {
    case ThreeOne:           // 三带一
    case ThreeTwo:           // 三带二
    case FourSeq_Two_Single: // 四带二（两张或两对）
    case FourSeq_Two_Pair: { // 四带二（两张或两对）
      extra = _extra;
      break;
    }
    default: {
      assert(0 && "Should not reach here.");
    }
    }
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
    }
    return ans;
  }

  static std::vector<std::pair<Type, std::vector<Card>>>
  get_possible_move_by_type(const std::vector<Card> &current, Type type) {
    std::vector<std::pair<Type, std::vector<Card>>> ans;
    size_t length = current.size();
    size_t index = 0;

    // Temp vector for current available card set that can play
    std::vector<Card> set;
    switch (type.get_type()) {

    case Single: {
      auto ret = get_consecutive_n_cards_set(current, 1);
      for (const auto &r : ret) {
        ans.push_back(std::pair<Type, std::vector<Card>>(Single, r));
      }
      break;
    }

    case Double: {
      auto ret = get_consecutive_n_cards_set(current, 2);
      for (const auto &r : ret) {
        ans.push_back(std::pair<Type, std::vector<Card>>(Double, r));
      }
      break;
    }

    case Triple: {
      auto ret = get_consecutive_n_cards_set(current, 3);
      for (const auto &r : ret) {
        ans.push_back(std::pair<Type, std::vector<Card>>(Triple, r));
      }
      break;
    }

      // TODO: implement other card sets

    case Bomb: {
      auto ret = get_consecutive_n_cards_set(current, 4);
      for (const auto &r : ret) {
        ans.push_back(std::pair<Type, std::vector<Card>>(Bomb, r));
      }
      break;
    }

    case UltraBomb: {
      if (current[current.size() - 1] == Card(RED_JOKER, -1) &&
          current[current.size() - 2] == Card(BLACK_JOKER, -1)) {
        set = std::vector<Card>(current.end() - 2, current.end());
        ans.push_back(std::pair<Type, std::vector<Card>>(UltraBomb, set));
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

  static std::vector<Type> get_possible_types(Type current_type) {
    std::vector<Type> types;
    switch (current_type.get_type()) {
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
    case Single:   // single card
    case Double:   // double cards
    case Triple:   // three cards
    case ThreeOne: // 三带一
    case ThreeTwo: // 三带二

    case SingleSeq: // 顺子

    case DoubleSeq: // 双顺

    case ThreeSeq: // 三顺

    case FourSeq_Two_Single: // 四带二（两张或两对）
    case FourSeq_Two_Pair: { // 四带二（两张或两对）
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
    }
    return types;
  }

public:
  static std::vector<std::pair<Type, std::vector<Card>>>
  get_possible_move(std::vector<Card> &current, Type current_type) {
    std::sort(current.begin(), current.end());

    std::vector<std::pair<Type, std::vector<Card>>> ans;

    std::vector<Type> possible_types = get_possible_types(current_type);

    for (const auto &p : possible_types) {
      auto ret = get_possible_move_by_type(current, p);
      ans.insert(ans.end(), ret.begin(), ret.end());
    }
    return ans;
  }

  static std::vector<std::pair<Type, std::vector<Card>>>
  trim_by_last_play(std::vector<std::pair<Type, std::vector<Card>>> &current,
                    Type current_type, std::vector<Card> last_play) {
    // TODO: finish this function
    return current;
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