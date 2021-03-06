
#include "Strategy.h"

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
    auto three_two = get_sequence(current, 3, 2);
    auto one = get_consecutive_n_cards_set(current, 1);
    for (const auto &t : three_two) {
      for (size_t o1 = 0; o1 < one.size(); o1++) {
        for (size_t o2 = o1 + 1; o2 < one.size(); o2++) {
          if (t[0] == one[o1][0] || t[0] == one[o2][0] || t[3] == one[o1][0] ||
              t[3] == one[o2][0]) {
            continue;
          }
          std::vector<Card> temp;
          temp.insert(temp.end(), one[o1].begin(), one[o1].end());
          temp.insert(temp.end(), one[o2].begin(), one[o2].end());
          ans.push_back(CardSet(type, t, temp));
        }
      }
    }
    auto two = get_consecutive_n_cards_set(current, 2);
    for (const auto &t : three_two) {
      for (size_t o1 = 0; o1 < two.size(); o1++) {
        if (t[0] == two[o1][0] || t[3] == two[o1][0]) {
          continue;
        }
        std::vector<Card> temp;
        temp.insert(temp.end(), one[o1].begin(), one[o1].end());
        ans.push_back(CardSet(type, t, temp));
      }
    }
    break;
  }

  case Airplane_Pair: {
    auto three_two = get_sequence(current, 3, 2);
    auto two = get_consecutive_n_cards_set(current, 2);
    for (const auto &t : three_two) {
      for (size_t o1 = 0; o1 < two.size(); o1++) {
        for (size_t o2 = o1 + 1; o2 < two.size(); o2++) {
          if (t[0] == two[o1][0] || t[0] == two[o2][0] || t[3] == two[o1][0] ||
              t[3] == two[o2][0]) {
            continue;
          }
          std::vector<Card> temp;
          temp.insert(temp.end(), two[o1].begin(), two[o1].end());
          temp.insert(temp.end(), two[o2].begin(), two[o2].end());
          ans.push_back(CardSet(type, t, temp));
        }
      }
    }
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