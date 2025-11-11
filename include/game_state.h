#pragma once

#include "types.h"
#include <array>
#include <optional>

struct CastlingRights {
  bool white_king_side_ = true;
  bool white_queen_side_ = true;
  bool black_king_side_ = true;
  bool black_queen_side_ = true;
};

struct GameState {
  Board board_;
  Color active_color_ = Color::WHITE;
  CastlingRights castling_rights_;
  
  std::optional<Position> en_passant_target_ = std::nullopt;
  
  int half_move_clock_ = 0;
  int fullmove_number_ = 1;
  GameState();
  
  char get_piece(const Position& pos) const {
    if (!pos.is_valid()) return ' ';
    return board_[pos.row][pos.col];
  }

  void set_piece(const Position& pos, char piece) {
    if (pos.is_valid()) {
      board_[pos.row][pos.col] = piece;
    }
  }
};