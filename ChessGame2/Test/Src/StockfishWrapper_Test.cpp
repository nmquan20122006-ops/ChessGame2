#include"StockfishWrapper_Test.h"
#include <iostream>
#include <vector>
#include <string>

// ============= Mô phỏng bàn cờ và nước đi (thay bằng game thật của bạn) =============
struct Move {
    std::string from;
    std::string to;
    std::string piece;
    std::string captured;
    bool is_castling;
    bool is_en_passant;

    Move() : from(""), to(""), piece(""), captured(""), is_castling(false), is_en_passant(false) {}
    Move(std::string f, std::string t, std::string p) : from(f), to(t), piece(p), captured(""), is_castling(false), is_en_passant(false) {}
};

class ChessGameMock {
private:
    std::vector<std::string> move_history;  // Lưu các nước đã đi
    std::vector<std::string> undo_stack;    // Stack undo
    std::vector<std::string> redo_stack;    // Stack redo
    std::string current_turn;
    bool is_game_over;

public:
    ChessGameMock() : current_turn("white"), is_game_over(false) {}

    bool make_move(const std::string& move) {
        if (is_game_over) return false;

        // Lưu nước đi vào history
        move_history.push_back(move);
        undo_stack.push_back(move);

        // Xóa redo stack khi có nước đi mới
        redo_stack.clear();

        // Đổi lượt
        current_turn = (current_turn == "white") ? "black" : "white";

        return true;
    }

    bool undo() {
        if (is_game_over) return false;
        if (undo_stack.empty()) return false;

        // Lấy nước cuối từ undo_stack
        std::string last_move = undo_stack.back();
        undo_stack.pop_back();

        // Đưa vào redo_stack
        redo_stack.push_back(last_move);

        // Xóa khỏi history
        if (!move_history.empty()) {
            move_history.pop_back();
        }

        // Đổi lượt ngược lại
        current_turn = (current_turn == "white") ? "black" : "white";

        return true;
    }

    bool redo() {
        if (is_game_over) return false;
        if (redo_stack.empty()) return false;

        // Lấy nước từ redo_stack
        std::string move = redo_stack.back();
        redo_stack.pop_back();

        // Đưa lại vào undo_stack
        undo_stack.push_back(move);
        move_history.push_back(move);

        // Đổi lượt
        current_turn = (current_turn == "white") ? "black" : "white";

        return true;
    }

    void set_game_over(bool over) { is_game_over = over; }
    void clear_history() { move_history.clear(); undo_stack.clear(); redo_stack.clear(); }

    int get_history_size() { return move_history.size(); }
    int get_undo_size() { return undo_stack.size(); }
    int get_redo_size() { return redo_stack.size(); }
    std::string get_current_turn() { return current_turn; }
    std::string get_last_move() { return move_history.empty() ? "" : move_history.back(); }
};

// ============= Unit Tests =============

void UndoTest::assert_true(bool condition, const std::string& test_name) {
    if (condition) {
        std::cout << "✅ PASS: " << test_name << std::endl;
    }
    else {
        std::cout << "❌ FAIL: " << test_name << std::endl;
    }
}

void UndoTest::assert_equal(const std::string& actual, const std::string& expected, const std::string& test_name) {
    if (actual == expected) {
        std::cout << "✅ PASS: " << test_name << " (expected: " << expected << ")" << std::endl;
    }
    else {
        std::cout << "❌ FAIL: " << test_name << " (actual: " << actual << ", expected: " << expected << ")" << std::endl;
    }
}

void UndoTest::test_undo_single_move() {
    ChessGameMock game;

    game.make_move("e2e4");
    game.undo();

    assert_equal(game.get_current_turn(), "white", "test_undo_single_move - turn reverts to white");
    assert_true(game.get_history_size() == 0, "test_undo_single_move - history empty");
}

void UndoTest::test_undo_multiple_moves() {
    ChessGameMock game;

    game.make_move("e2e4");
    game.make_move("e7e5");
    game.make_move("g1f3");

    game.undo();  // undo g1f3
    assert_equal(game.get_current_turn(), "black", "test_undo_multiple_moves - after first undo");
    assert_true(game.get_history_size() == 2, "test_undo_multiple_moves - history size 2");

    game.undo();  // undo e7e5
    assert_equal(game.get_current_turn(), "white", "test_undo_multiple_moves - after second undo");
    assert_true(game.get_history_size() == 1, "test_undo_multiple_moves - history size 1");
}

void UndoTest::test_undo_when_no_moves() {
    ChessGameMock game;

    bool result = game.undo();
    assert_true(!result, "test_undo_when_no_moves - undo returns false");
    assert_true(game.get_history_size() == 0, "test_undo_when_no_moves - history remains empty");
}

void UndoTest::test_undo_after_game_over() {
    ChessGameMock game;

    game.make_move("e2e4");
    game.set_game_over(true);

    bool result = game.undo();
    assert_true(!result, "test_undo_after_game_over - undo not allowed after game over");
    assert_true(game.get_history_size() == 1, "test_undo_after_game_over - history unchanged");
}

void UndoTest::test_undo_redo_sequence() {
    ChessGameMock game;

    game.make_move("e2e4");
    game.make_move("e7e5");

    // Undo 2 lần
    game.undo();  // undo e7e5
    game.undo();  // undo e2e4

    assert_true(game.get_undo_size() == 0, "test_undo_redo_sequence - undo stack empty");
    assert_true(game.get_redo_size() == 2, "test_undo_redo_sequence - redo stack has 2 moves");

    // Redo 1 lần
    game.redo();
    assert_true(game.get_history_size() == 1, "test_undo_redo_sequence - after redo history size 1");
    assert_true(game.get_undo_size() == 1, "test_undo_redo_sequence - after redo undo size 1");
    assert_true(game.get_redo_size() == 1, "test_undo_redo_sequence - after redo redo size 1");

    // Redo lần 2
    game.redo();
    assert_true(game.get_history_size() == 2, "test_undo_redo_sequence - after second redo history size 2");
    assert_true(game.get_redo_size() == 0, "test_undo_redo_sequence - redo stack empty");
}

void UndoTest::test_undo_preserves_turn() {
    ChessGameMock game;

    assert_equal(game.get_current_turn(), "white", "test_undo_preserves_turn - initial turn white");

    game.make_move("e2e4");
    assert_equal(game.get_current_turn(), "black", "test_undo_preserves_turn - after move turn black");

    game.undo();
    assert_equal(game.get_current_turn(), "white", "test_undo_preserves_turn - after undo turn back to white");
}

void UndoTest::test_undo_capture_restore() {
    // Test này cần bàn cờ thật để kiểm tra quân bị ăn được phục hồi
    // Với mock, chỉ test cơ bản
    ChessGameMock game;

    game.make_move("e2e4");
    game.make_move("d7d5");
    game.make_move("e4d5");  // Bắt tốt

    game.undo();  // Undo nước bắt
    assert_true(game.get_history_size() == 2, "test_undo_capture_restore - after undo capture");
    // Trong game thật, cần kiểm tra quân bị ăn đã được đặt lại
}

void UndoTest::test_undo_castling() {
    // Test cho nước nhập thành
    ChessGameMock game;

    // Mô phỏng nhập thành (trong game thật cần FEN đặc biệt)
    game.make_move("e1g1");  // Nhập thành ngắn
    game.undo();

    assert_true(game.get_history_size() == 0, "test_undo_castling - after undo castling");
}

void UndoTest::test_undo_en_passant() {
    // Test cho nước bắt tốt qua đường
    ChessGameMock game;

    game.make_move("e2e4");
    game.make_move("d7d5");
    game.make_move("e4d5");  // Bắt en passant

    game.undo();
    assert_true(game.get_history_size() == 2, "test_undo_en_passant - after undo en passant");
}

void UndoTest::run_all_tests() {
    std::cout << "\n========== RUNNING UNDO UNIT TESTS ==========\n" << std::endl;

    test_undo_single_move();
    test_undo_multiple_moves();
    test_undo_when_no_moves();
    test_undo_after_game_over();
    test_undo_redo_sequence();
    test_undo_preserves_turn();
    test_undo_capture_restore();
    test_undo_castling();
    test_undo_en_passant();

    std::cout << "\n========== ALL TESTS COMPLETED ==========\n" << std::endl;
}