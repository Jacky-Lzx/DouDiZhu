#include "Game.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <type_traits>

bool Game::decide_landlord(int &landlord) {
  landlord = -1;
  // 抢地主
  srand(time(NULL));
  int rand_index = rand() % 3;
  int current_index = rand_index;
  do {
    std::cout << "Player " << current_index
              << " decide to be landlord (1 for true, 0 for false): "
              << std::endl;
    int decision_landlord;
    std::cin >> decision_landlord;
    if (decision_landlord == 1) {
      std::cout << "Player " << current_index << " wants to be landlord!"
                << std::endl;
      landlord = current_index;
      break;
    }
    current_index = (current_index + 1) % 3;
  } while (current_index != rand_index);

  if (landlord == -1) {
    std::cout << "No one wants to be landlord, game restart." << std::endl;
    return false;
  }

  // 决定地主
  int next_player = (landlord + 1) % 3;
  int landlord_candidate = -1;
  while (next_player != landlord) {
    std::cout << "Player " << next_player
              << ": do you want to pick the landlord?" << std::endl;
    int input;
    std::cin >> input;
    if (input == 1) {
      if (landlord_candidate == -1) {
        landlord_candidate = next_player;
      }
    }
    next_player = (next_player + 1) % 3;
  }
  if (landlord_candidate != -1) {
    std::cout << "Player " << landlord << ": do you want to be landlord?"
              << std::endl;
    int input;
    std::cin >> input;
    if (input == 0) {
      landlord = landlord_candidate;
    }
  }
  std::cout << "Player " << landlord << " becomes the landlord!" << std::endl;

  return true;
}

void Game::init() {
  int landlord = 0;
  do { // while (!decide_landlord(landlord))
    // shuffle and assign hards here
    deck = Deck();
    for (int i = 0; i < 3; i++) {
      players[i].clear();
      for (int j = 0; j < 17; j++) {
        players[i].push_back(deck.pick());
      }
    }
    for (auto &p : players) {
      std::sort(p.begin(), p.end());
    }
    print_state();

  } while (!decide_landlord(landlord));

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