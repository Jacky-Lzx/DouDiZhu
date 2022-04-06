#ifndef CARD
#define CARD

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

enum Suit { SPADE, HEART, DIAMOND, CLUB, BLACK_JOKER, RED_JOKER };

class Card {
  Suit suit;
  // In range [1, 13]
  int number;

public:
  Card() = default;
  Card(Suit _suit, int _number) : suit(_suit), number(_number) {}
  Card(int num);

  friend bool operator<(const Card &c1, const Card &c2);
  // Now we only support card - 1, and assert input card is not 3
  friend Card operator-(const Card &c1, const int &i);
  // Don't consider suit here, if the numbers are equal, then two cards are
  // equal
  friend bool operator==(const Card &lhs, const Card &rhs);
  friend bool operator!=(const Card &lhs, const Card &rhs);

  friend std::ostream &operator<<(std::ostream &os, const Card &c);

  bool equal_all(const Card &c);
};

enum type_t {
  TYPE_START, // no type, can use all kinds of types
  Single,     // single card
  Double,     // double cards
  Triple,     // three cards

  SingleSeq, // 顺子
  DoubleSeq, // 双顺
  ThreeSeq,  // 三顺

  ThreeOne, // 三带一
  ThreeTwo, // 三带二

  Airplane_Single, // 飞机 (两张)
  Airplane_Pair,   // 飞机 (两对)

  Four_Two_Single, // 四带二（两张）
  Four_Two_Pair,   // 四带四（两对）

  Bomb,      // 炸弹
  UltraBomb, // 王炸
  TYPE_END,  // no type, can not use any kind of types
};

// Wrapper class to represent special types like (SingleSeq x 5)
//   or (DoubleSeq x 3)
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
  type_t get_type_t() { return type; }
  int get_length() { return length; }

  friend std::ostream &operator<<(std::ostream &os, const Type &t);
  friend bool operator==(const Type &t1, const Type &t2);
  friend bool operator<=(const Type &t1, const Type &t2);
  friend bool operator<(const Type &t1, const Type &t2);
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
    switch (_type.get_type_t()) {
    case ThreeOne: // 三带一
    case ThreeTwo: // 三带二
    case Airplane_Single:
    case Airplane_Pair:
    case Four_Two_Single: // 四带二（两张或两对）
    case Four_Two_Pair: { // 四带二（两张或两对）
      extra = _extra;
      break;
    }
    default: {
      assert(0 && "Should not reach here.");
    }
    }
  }

  std::vector<Card> get_base() const { return base; }
  std::vector<Card> get_extra() const { return extra; }
  Type get_type() const { return type; }

  friend bool operator<(const CardSet &c1, const CardSet &c2);
};

/**
 * @brief Used in the beginning, when assigning cards to players
 */
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

#endif // CARD