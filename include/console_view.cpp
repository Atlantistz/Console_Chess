#pragma once

#include "game_state.h"
#include "types.h"
#include <string>
#include <vector>

class ConsoleView {
 public:
  void print_board(const GameState& state) const;
  Move get_user_move_input(const std::vector<Move>& legal_moves, Color active_color) const;

  PieceType get_promotion_choice() const;

  void print_error(const std::string& message) const;

  void print_game_result(const std::string& result) const;

 private:
  std::optional<Move> parse_move(const std::string& input,
                                 const std::vector<Move>& legal_moves) const;

  Position algebraic_to_position(const std::string& alg) const;

  bool is_promotion_move(const Move& move, const GameState& temp_state) const;
};
