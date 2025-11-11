#include "console_view.h"
#include <iostream>
#include <limits>
#include <string>

void ConsoleView::print_board(const GameState& state) const {
  std::cout << "\n   a  b  c  d  e  f  g  h\n";
  
  int r = 8;
  for (const auto& row : state.board_) {
    std::cout << r << " ";
    for (const auto& cell : row) {
      if (cell == '.' || cell == ' ') {
        std::cout << "[" << cell << "]";
      } else {
        std::cout << "{" << cell << "}";
      }
    }
    std::cout << " " << r-- << "\n";
  }
  std::cout << "   a  b  c  d  e  f  g  h\n" << std::endl;
}

Move ConsoleView::get_user_move_input(
     const std::vector<Move>& legal_moves, Color active_color) const {
std::string input;
std::optional<Move> parsed_move;

while (true) {
     std::cout << "轮到 [" << color_to_string(active_color) << "] 走棋。"
<< std::endl;
std::cout << "请输入走法 (例如: e2e4): ";
std::cin >> input;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    if (input == "exit") {
      exit(0);
    }

    parsed_move = parse_move(input, legal_moves);

    if (parsed_move) {
      return *parsed_move;
    } else {
      print_error("无效的走法 '" + input + "'。请重试。");
      print_error(
          "走法必须是 4 个字符 (例如 e2e4) 且必须是一个合法的走法。");
    }
  }
}

PieceType ConsoleView::get_promotion_choice() const {
  char choice;
  while (true) {
    std::cout << "兵已到底线！请选择升变的棋子 (q, r, b, n): ";
    std::cin >> choice;
    choice = std::tolower(choice);
    
    if (choice == 'q') return PieceType::QUEEN;
    if (choice == 'r') return PieceType::ROOK;
    if (choice == 'b') return PieceType::BISHOP;
    if (choice == 'n') return PieceType::KNIGHT;

    print_error("无效的选择。请输入 'q', 'r', 'b', 或 'n'。");

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

void ConsoleView::print_error(const std::string& message) const {
  std::cout << "!! 错误: " << message << std::endl;
}

void ConsoleView::print_game_result(const std::string& result) const {
  std::cout << "=========================" << std::endl;
  std::cout << "游戏结果: " << result << std::endl;
  std::cout << "=========================" << std::endl;
}


std::optional<Move> ConsoleView::parse_move(
    const std::string& input, const std::vector<Move>& legal_moves) const {
  if (input.length()!= 4) {
    return std::nullopt;
  }

  Position from = algebraic_to_position(input.substr(0, 2));
  Position to = algebraic_to_position(input.substr(2, 2));

  if (!from.is_valid() ||!to.is_valid()) {
    return std::nullopt;
  }

  Move move = {from, to};

  for (const auto& legal_move : legal_moves) {
    if (move == legal_move) {
      return legal_move;
    }
  }
  return std::nullopt;
}

Position ConsoleView::algebraic_to_position(const std::string& alg) const {
  if (alg.length()!= 2) {
    return {-1, -1}; 
  }

  int col = alg[0] - 'a';
  int row = '8' - alg[1]; // <-- 修复: 使用 alg[1]
  return {row, col};
}