/*#ifndef TEST_UNDO_H
#define TEST_UNDO_H

#include <string>

class UndoTest {
public:
    // Chạy tất cả test
    static void run_all_tests();

private:
    static void test_undo_single_move();
    static void test_undo_multiple_moves();
    static void test_undo_when_no_moves();
    static void test_undo_after_game_over();
    static void test_undo_redo_sequence();
    static void test_undo_preserves_turn();
    static void test_undo_capture_restore();
    static void test_undo_castling();
    static void test_undo_en_passant();

    static void assert_equal(const std::string& actual, const std::string& expected, const std::string& test_name);
    static void assert_true(bool condition, const std::string& test_name);
};

#endif*/