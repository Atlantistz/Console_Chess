#pragma once

#include "game_state.h"
#include "types.h"
#include <string>
#include <vector>

class Game {
 public:
  Game();

  void make_move(const Move& move);
  GameState get_state() const;
  bool is_game_over() const;

  std::string get_result() const;

  std::vector<Move> get_all_legal_moves() const;

 private:
  GameState game_state_;
  mutable std::vector<Move> legal_moves_cache_;
  mutable bool cache_is_valid_ = false;


  bool is_move_legal(const Move& move) const;

  std::vector<Move> generate_pseudo_legal_moves(
      const GameState& state) const;

  bool is_king_in_check(Color color, const GameState& state) const;

  bool is_square_attacked(const Position& pos,
                            Color attacker_color,
                            const GameState& state) const;

  void make_temporary_move(GameState& temp_state, const Move& move) const;

  void update_castling_rights(GameState& state,
                                const Move& move,
                                char piece_moved) const;

  void generate_pawn_moves(std::vector<Move>& moves,
                           const GameState& state,
                           const Position& from) const;
  void generate_knight_moves(std::vector<Move>& moves,
                             const GameState& state,
                             const Position& from) const;
  void generate_king_moves(std::vector<Move>& moves,
                           const GameState& state,
                           const Position& from) const;
  void generate_sliding_moves(std::vector<Move>& moves,
                              const GameState& state,
                              const Position& from,
                              const std::vector<Position>& directions) const;

  void add_move(std::vector<Move>& moves,
                const GameState& state,
                const Position& from,
                const Position& to,
                bool is_capture) const;
};