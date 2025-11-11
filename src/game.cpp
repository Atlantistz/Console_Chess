#include "game.h"
#include <iostream>

Game::Game() : game_state_(), cache_is_valid_(false) {}

GameState Game::get_state() const {
  return game_state_;
}

std::vector<Move> Game::get_all_legal_moves() const {
  if (cache_is_valid_) {
    return legal_moves_cache_;
  }

  std::vector<Move> legal_moves;
  std::vector<Move> pseudo_moves =
      generate_pseudo_legal_moves(game_state_);

  for (const auto& move : pseudo_moves) {
    GameState temp_state = game_state_;
    
    make_temporary_move(temp_state, move);

    if (!is_king_in_check(game_state_.active_color_, temp_state)) {
      legal_moves.push_back(move);
    }
  }

  legal_moves_cache_ = legal_moves;
  cache_is_valid_ = true;
  return legal_moves;
}

bool Game::is_game_over() const {
  return get_all_legal_moves().empty();
}

std::string Game::get_result() const {
  if (!is_game_over()) {
    return "游戏进行中";
  }

  if (is_king_in_check(game_state_.active_color_, game_state_)) {
    return (game_state_.active_color_ == Color::WHITE)? "黑方胜利 (将死)"
                                                      : "白方胜利 (将死)";
  } else {
    return "平局 (逼和)";
  }
}

void Game::make_move(const Move& move) {
  make_temporary_move(game_state_, move);

  game_state_.active_color_ = (game_state_.active_color_ == Color::WHITE)
                                   ? Color::BLACK
                                    : Color::WHITE;
  
  if (game_state_.active_color_ == Color::WHITE) {
    game_state_.fullmove_number_++;
  }

  cache_is_valid_ = false;
}


void Game::make_temporary_move(GameState& state, const Move& move) const {
  char piece_moved = state.get_piece(move.from);
  char captured_piece = state.get_piece(move.to);


  state.en_passant_target_ = std::nullopt;

  state.set_piece(move.to, piece_moved);
  state.set_piece(move.from, '.');

  PieceType type = char_to_piece_type(piece_moved);
  if (type == PieceType::PAWN) {
    if (std::abs(move.from.row - move.to.row) == 2) {
      state.en_passant_target_ = Position{
          (move.from.row + move.to.row) / 2, move.from.col};
    }

    else if (move.to == state.en_passant_target_) {
      int captured_pawn_row = (state.active_color_ == Color::WHITE)
                                 ? move.to.row + 1
                                  : move.to.row - 1;
      state.set_piece(Position{captured_pawn_row, move.to.col}, '.');
    }

    else if (move.promotion_piece!= PieceType::EMPTY) {
      char new_piece;
      switch (move.promotion_piece) {
        case PieceType::QUEEN: new_piece = 'q'; break;
        case PieceType::ROOK: new_piece = 'r'; break;
        case PieceType::BISHOP: new_piece = 'b'; break;
        case PieceType::KNIGHT: new_piece = 'n'; break;
        default: new_piece = 'q';
      }
      if (state.active_color_ == Color::WHITE) {
        new_piece = std::toupper(new_piece);
      }
      state.set_piece(move.to, new_piece);
    }
  }

  else if (type == PieceType::KING) {
    int row = move.from.row;

    if (move.from.col == 4 && move.to.col == 6) {
      state.set_piece(Position{row, 5}, state.get_piece(Position{row, 7}));
      state.set_piece(Position{row, 7}, '.');
    }

    else if (move.from.col == 4 && move.to.col == 2) {
      state.set_piece(Position{row, 3}, state.get_piece(Position{row, 0}));
      state.set_piece(Position{row, 0}, '.');
    }
  }


  update_castling_rights(state, move, piece_moved);


  if (type == PieceType::PAWN || captured_piece!= '.') {
    state.half_move_clock_ = 0;
  } else {
    state.half_move_clock_++;
  }
}


void Game::update_castling_rights(GameState& state,
                                  const Move& move,
                                  char piece_moved) const {
  auto& rights = state.castling_rights_;
  Color color = state.active_color_;

  if (std::tolower(piece_moved) == 'k') {

    if (color == Color::WHITE) {
      rights.white_king_side_ = false;
      rights.white_queen_side_ = false;
    } else {
      rights.black_king_side_ = false;
      rights.black_queen_side_ = false;
    }
  }

  else if (move.from.row == 7 && move.from.col == 7) {
    rights.white_king_side_ = false;
  } else if (move.from.row == 7 && move.from.col == 0) {
    rights.white_queen_side_ = false;
  } else if (move.from.row == 0 && move.from.col == 7) {
    rights.black_king_side_ = false;
  } else if (move.from.row == 0 && move.from.col == 0) {
    rights.black_queen_side_ = false;
  }
}

bool Game::is_square_attacked(const Position& pos,
                            Color attacker_color,
                            const GameState& state) const {
  const std::vector<Position> rook_dirs = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  const std::vector<Position> bish_dirs = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (const auto& dir : rook_dirs) {
    for (int i = 1; ; ++i) {
      Position p = {pos.row + dir.row * i, pos.col + dir.col * i};
      if (!p.is_valid()) break;
      char piece = state.get_piece(p);
      if (piece!= '.') {
        if (get_piece_color(piece) == attacker_color &&
            (std::tolower(piece) == 'r' || std::tolower(piece) == 'q')) {
          return true;
        }
        break;
      }
    }
  }

  for (const auto& dir : bish_dirs) {
    for (int i = 1; ; ++i) {
      Position p = {pos.row + dir.row * i, pos.col + dir.col * i};
      if (!p.is_valid()) break;
      char piece = state.get_piece(p);
      if (piece!= '.') {
        if (get_piece_color(piece) == attacker_color &&
            (std::tolower(piece) == 'b' || std::tolower(piece) == 'q')) {
          return true;
        }
        break;
      }
    }
  }

  const std::vector<Position> knight_moves = {
      {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
      {1, -2},  {1, 2},  {2, -1},  {2, 1}};
  for (const auto& move : knight_moves) {
    Position p = {pos.row + move.row, pos.col + move.col};
    if (p.is_valid() && get_piece_color(state.get_piece(p)) == attacker_color &&
        std::tolower(state.get_piece(p)) == 'n') {
      return true;
    }
  }

  int dir = (attacker_color == Color::WHITE)? 1 : -1;
  Position p1 = {pos.row + dir, pos.col + 1};
  Position p2 = {pos.row + dir, pos.col - 1};
  if (p1.is_valid() && get_piece_color(state.get_piece(p1)) == attacker_color &&
      std::tolower(state.get_piece(p1)) == 'p') {
    return true;
  }
  if (p2.is_valid() && get_piece_color(state.get_piece(p2)) == attacker_color &&
      std::tolower(state.get_piece(p2)) == 'p') {
    return true;
  }

  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      Position p = {pos.row + dr, pos.col + dc};
      if (p.is_valid() && get_piece_color(state.get_piece(p)) == attacker_color &&
          std::tolower(state.get_piece(p)) == 'k') {
        return true;
      }
    }
  }

  return false;
}

bool Game::is_king_in_check(Color color, const GameState& state) const {
  Position king_pos;
  char king_char = (color == Color::WHITE)? 'K' : 'k';
  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      if (state.board_[r][c] == king_char) {
        king_pos = {r, c};
        break;
      }
    }
  }

  if (!king_pos.is_valid()) {
    return false; 
  }

  Color attacker_color = (color == Color::WHITE)? Color::BLACK : Color::WHITE;
  return is_square_attacked(king_pos, attacker_color, state);
}

std::vector<Move> Game::generate_pseudo_legal_moves(
    const GameState& state) const {
  std::vector<Move> moves;
  Color my_color = state.active_color_;

  for (int r = 0; r < 8; ++r) {
    for (int c = 0; c < 8; ++c) {
      Position from = {r, c};
      char piece = state.get_piece(from);
      if (piece == '.' || get_piece_color(piece)!= my_color) {
        continue;
      }

      switch (std::tolower(piece)) {
        case 'p':
          generate_pawn_moves(moves, state, from);
          break;
        case 'n':
          generate_knight_moves(moves, state, from);
          break;
        case 'b':
          generate_sliding_moves(
              moves, state, from,
              {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}});
          break;
        case 'r':
          generate_sliding_moves(
              moves, state, from,
              {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}); 
          break;
        case 'q':
          generate_sliding_moves(
              moves, state, from,
              {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
               {0, 1}, {0, -1}, {1, 0}, {-1, 0}});
          break;
        case 'k':
          generate_king_moves(moves, state, from);
          break;
      }
    }
  }
  return moves;
}

void Game::add_move(std::vector<Move>& moves,
                const GameState& state,
                const Position& from,
                const Position& to,
                bool is_capture) const {
  char piece = state.get_piece(from);
  Color color = get_piece_color(piece);
  
  if (to.is_valid()) {
    char target_piece = state.get_piece(to);
    Color target_color = get_piece_color(target_piece);

    if (is_capture) {
      if (target_color == color) return;
      if (target_color == Color::NONE) return;
    } else {
      if (target_color!= Color::NONE) return;
    }

    int promotion_row = (color == Color::WHITE)? 0 : 7;
    if (std::tolower(piece) == 'p' && to.row == promotion_row) {
      moves.push_back({from, to, PieceType::QUEEN});
      moves.push_back({from, to, PieceType::ROOK});
      moves.push_back({from, to, PieceType::BISHOP});
      moves.push_back({from, to, PieceType::KNIGHT});
    } else {
      moves.push_back({from, to});
    }
  }
}

void Game::generate_pawn_moves(std::vector<Move>& moves,
                           const GameState& state,
                           const Position& from) const {
  Color color = state.active_color_;
  int dir = (color == Color::WHITE)? -1 : 1;
  int start_row = (color == Color::WHITE)? 6 : 1;

  Position one_step = {from.row + dir, from.col};
  if (one_step.is_valid() && state.get_piece(one_step) == '.') {
    add_move(moves, state, from, one_step, false);

    if (from.row == start_row) {
      Position two_steps = {from.row + 2 * dir, from.col};
      if (two_steps.is_valid() && state.get_piece(two_steps) == '.') {
        add_move(moves, state, from, two_steps, false);
      }
    }
  }

  Position cap_left = {from.row + dir, from.col - 1};
  Position cap_right = {from.row + dir, from.col + 1};
  if (cap_left.is_valid() && state.get_piece(cap_left)!= '.' &&
      get_piece_color(state.get_piece(cap_left))!= color) {
    add_move(moves, state, from, cap_left, true);
  }
  if (cap_right.is_valid() && state.get_piece(cap_right)!= '.' &&
      get_piece_color(state.get_piece(cap_right))!= color) {
    add_move(moves, state, from, cap_right, true);
  }

  if (state.en_passant_target_) {
    Position target = *state.en_passant_target_;
    if (target == cap_left || target == cap_right) {
      add_move(moves, state, from, target, true);
    }
  }
}

void Game::generate_knight_moves(std::vector<Move>& moves,
                             const GameState& state,
                             const Position& from) const {
  const std::vector<Position> knight_moves = {
      {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
      {1, -2},  {1, 2},  {2, -1},  {2, 1}};
  for (const auto& move : knight_moves) {
    Position to = {from.row + move.row, from.col + move.col};
    add_move(moves, state, from, to, true);
  }
}

void Game::generate_king_moves(std::vector<Move>& moves,
                           const GameState& state,
                           const Position& from) const {
  for (int dr = -1; dr <= 1; ++dr) {
    for (int dc = -1; dc <= 1; ++dc) {
      if (dr == 0 && dc == 0) continue;
      Position to = {from.row + dr, from.col + dc};
      add_move(moves, state, from, to, true);
    }
  }

  Color color = state.active_color_;
  const auto& rights = state.castling_rights_;
  
  if (is_king_in_check(color, state)) {
    return;
  }
  
  
  int row = (color == Color::WHITE)? 7 : 0;
  Color enemy_color = (color == Color::WHITE)? Color::BLACK : Color::WHITE;

  if ((color == Color::WHITE && rights.white_king_side_) ||
      (color == Color::BLACK && rights.black_king_side_)) {
    if (state.get_piece({row, 5}) == '.' && state.get_piece({row, 6}) == '.') {
      if (!is_square_attacked({row, 5}, enemy_color, state) &&
         !is_square_attacked({row, 6}, enemy_color, state)) {
        add_move(moves, state, from, {row, 6}, false);
      }
    }
  }

  if ((color == Color::WHITE && rights.white_queen_side_) ||
      (color == Color::BLACK && rights.black_queen_side_)) {
    if (state.get_piece({row, 1}) == '.' && state.get_piece({row, 2}) == '.' &&
        state.get_piece({row, 3}) == '.') {
      if (!is_square_attacked({row, 2}, enemy_color, state) &&
         !is_square_attacked({row, 3}, enemy_color, state)) {
        add_move(moves, state, from, {row, 2}, false);
      }
    }
  }
}

void Game::generate_sliding_moves(
    std::vector<Move>& moves,
    const GameState& state,
    const Position& from,
    const std::vector<Position>& directions) const {
  for (const auto& dir : directions) {
    for (int i = 1; ; ++i) {
      Position to = {from.row + dir.row * i, from.col + dir.col * i};
      if (!to.is_valid()) {
        break;
      }
      
      char target_piece = state.get_piece(to);
      if (target_piece!= '.') {
        if (get_piece_color(target_piece)!= state.active_color_) {
          add_move(moves, state, from, to, true); 
        }
        break;
      }
      add_move(moves, state, from, to, false); 
    }
  }
}