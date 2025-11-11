#include "console_view.h"
#include "game.h"
#include "types.h"
#include <iostream>

int main() {
  Game game;
  ConsoleView view;

  while (!game.is_game_over()) {
    GameState current_state = game.get_state();
    view.print_board(current_state);

    std::vector<Move> legal_moves = game.get_all_legal_moves();

    Move user_move = view.get_user_move_input(legal_moves, current_state.active_color_);

      char piece = current_state.get_piece(user_move.from);
    if (std::tolower(piece) == 'p') {
      int promotion_row = (get_piece_color(piece) == Color::WHITE)? 0 : 7;
      if (user_move.to.row == promotion_row) {
        user_move.promotion_piece = view.get_promotion_choice();
      }
    }

    game.make_move(user_move);
  }

  std::cout << "\n游戏结束！" << std::endl;
  view.print_board(game.get_state());
  view.print_game_result(game.get_result());

  return 0;
}
