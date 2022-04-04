
#include "Card.h"

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

std::ostream &operator<<(std::ostream &os, const Card &c) {
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

std::ostream &operator<<(std::ostream &os, const Type &t) {
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