#include "game_state.h"

GameState::GameState() {
  board_ = {{
      {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
      {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
      {'.', ' ', '.', ' ', '.', ' ', '.', ' '},
      {' ', '.', ' ', '.', ' ', '.', ' ', '.'},
      {'.', ' ', '.', ' ', '.', ' ', '.', ' '},
      {' ', '.', ' ', '.', ' ', '.', ' ', '.'},
      {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
      {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'} 
  }};

  active_color_ = Color::WHITE;
  castling_rights_ = CastlingRights();
  en_passant_target_ = std::nullopt;
  half_move_clock_ = 0;
  fullmove_number_ = 1;
}