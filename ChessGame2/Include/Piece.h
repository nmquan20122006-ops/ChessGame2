#pragma once



enum class Piece {
	Empty = 0,
	W_Pawn = 1,
	W_Rook = 2,
	W_Knight = 3,
	W_Bishop = 4,
	W_Queen = 5,
	W_King = 6,

	B_Pawn = 7,
	B_Rook = 8,
	B_Knight = 9,
	B_Bishop = 10,
	B_Queen = 11,
	B_King = 12
};



//Logic truy van Piece


inline bool isWhite(Piece p) {

	int value = static_cast<int>(p);
	return value >= 1 && value <= 6;
}

inline bool isBlack(Piece p) {

	int value = static_cast<int>(p);
	return value >= 7 && value <= 12;
}

inline Piece getType(Piece p) {

	int value = static_cast<int>(p);

	if (value == 0)return Piece::Empty;

	int typeValue = (value > 6) ? (value - 6) : value;

	return static_cast<Piece>(typeValue);
}

inline bool isSameColor(Piece p1, Piece p2) {

	if (p1 == Piece::Empty || p2 == Piece::Empty)return false;

	return (isWhite(p1) && isWhite(p2)) || (isBlack(p1) && isBlack(p2));
}

inline bool isOpponent(Piece p1, Piece p2) {

	if (p1 == Piece::Empty || p2 == Piece::Empty)return false;

	return (isWhite(p1) && isBlack(p2)) || (isBlack(p1) && isWhite(p2));

}

inline Piece getBlackPiece(Piece whitePiece) {

	int value = static_cast<int>(whitePiece);
	if (value >= 1 && value <= 6) {
		return static_cast<Piece>(value + 6);  // W_Pawn (1) -> B_Pawn (7)
	}
	return Piece::Empty;
}