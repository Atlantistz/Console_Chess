#pragma once

#include <array>
#include <optional>
#include <string>

using Board = std::array<std::array<char, 8>, 8>;
enum class Color { WHITE, BLACK, NONE };
enum class PieceType { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN, EMPTY };
struct Position {
  int row = -1;
  int col = -1;

  bool is_valid() const {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
  }

  bool operator==(const Position& other) const {
    return row == other.row && col == other.col;
  }
};

struct Move {
  Position from;
  Position to;

  PieceType promotion_piece = PieceType::EMPTY;

  bool operator==(const Move& other) const {
    return from == other.from && to == other.to;
  }
};

inline Color string_to_color(const std::string& s) {
  return (s == "WHITE")? Color::WHITE : Color::BLACK;
}

inline std::string color_to_string(Color c) {
  return (c == Color::WHITE)? "WHITE" : "BLACK";
}

inline PieceType char_to_piece_type(char c) {
  switch (std::tolower(c)) {
    case 'k': return PieceType::KING;
    case 'q': return PieceType::QUEEN;
    case 'r': return PieceType::ROOK;
    case 'b': return PieceType::BISHOP;
    case 'n': return PieceType::KNIGHT;
    case 'p': return PieceType::PAWN;
    default: return PieceType::EMPTY;
  }
}

inline Color get_piece_color(char c) {
  if (std::isspace(c) || c == '.' || c == ' ') {
    return Color::NONE;
  }
  return std::isupper(c)? Color::WHITE : Color::BLACK;
}