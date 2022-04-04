#ifndef STRATEGY
#define STRATEGY

#include <vector>

#include "Card.h"

/**
 * @brief The class that determine different choices a player can take, given
 * the last played card set.
 */
class Strategy {
private:
  static std::vector<std::vector<Card>>
  get_consecutive_n_cards_set(const std::vector<Card> &current, const int &n);

  // number 2 and jokers are not included
  static std::vector<std::vector<Card>>
  get_sequence(const std::vector<Card> &current, const int &consecutive_num,
               const int &length);

  static std::vector<CardSet>
  get_possible_move_by_type(const std::vector<Card> &current, Type type);

  // return current.size() if index == current.size()
  static size_t jump_to_next_number(const std::vector<Card> &current,
                                    size_t index);

  static bool has_consecutive_cards(const std::vector<Card> &current,
                                    size_t index, int num);

  static std::vector<Type> get_possible_types(Type current_type);

public:
  static std::vector<CardSet> get_possible_move(std::vector<Card> &current,
                                                Type current_type);

  static std::vector<CardSet> trim_by_last_play(std::vector<CardSet> &current,
                                                CardSet last_play);
};

#endif // STRATEGY