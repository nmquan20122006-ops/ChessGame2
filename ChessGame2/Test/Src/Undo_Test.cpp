#include"Undo_Test.h"

Board_DEBUG::Board_DEBUG(){}

		void Board_DEBUG::printBoard(Board& b) {
			std::cout << "======================" << std::endl;
			std::cout << "BOARD REAL INFORMATION" << std::endl;

			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					int p = (int)b.board[i][j];
					if (p == 0)std::cout << "* ";
					else std::cout << p << " ";
				}
				std::cout << std::endl;
			}
			std::cout << "----------------" << std::endl;
			/*std::cout << "KING INFORMATION" << std::endl;

			std::cout << "WhiteKing: " << "(" << b.whiteKingPos.row << ":" << b.whiteKingPos.col << ")" << std::endl;
			std::cout << "BlackKing: " << "(" << b.blackKingPos.row << ":" << b.blackKingPos.col << ")" << std::endl;*/
			
		}

		void Board_DEBUG::printMoveList() {
			for (int i = 0; i < moveListTo.size(); i++) {
				std::cout << "Move List To: " << "(" << moveListTo[i].row << ":" << moveListTo[i].col << ")" << std::endl;
			}
			for (int i = 0; i < moveListTo.size(); i++) {
				std::cout << "Move List From: " << "(" << moveListFrom[i].row << ":" << moveListFrom[i].col << ")" << std::endl;
			}
		}

		void Board_DEBUG::printUndoStack() {

			std::cout << "Undo stack: " << std::endl;
			for (int i = 0; i < undoStack.size(); i++) {

				std::cout << "(" << undoStack[i].fromPos.row << ":" << undoStack[i].fromPos.col << ")"
					<< "(" << undoStack[i].toPos.row << ":" << undoStack[i].toPos.col << ")" << "Piece: "
					<< (int)undoStack[i].capturedPiece << std::endl;
			}
		}

		void Board_DEBUG::move_Test(Board& b, BoardToFEN_DEBUG& fenDebugger,Position from, Position to) {

			Piece fromPiece = b.getPiece(from);
			Piece targetPiece = b.getPiece(to);

			std::cout << "Current turn: " << fenDebugger.turnToChar(state_Real.currentTurn) << std::endl;

			if (state_Real.getColor(fromPiece) == state_Real.currentTurn) {

				b.movePiece(from, to);



				state_Real.switchTurn();

				std::cout << "switch turn: " << fenDebugger.turnToChar(state_Real.currentTurn) << std::endl;

				updateMoveLog(from, to, targetPiece,fromPiece);

				std::string enPassanString = fenDebugger.enPassantToString(move_Real);

			

				undoStack.push_back(move_Real);

				fenDebugger.toFEN(b,state_Real,move_Real);

				fenDebugger.printBoard(b);
				
			}
		}

		void Board_DEBUG::printMoveLog() {
			std::cout << "MOVE LOG" << std::endl;
			std::cout << "From: " << "(" << move_Real.fromPos.row << ":" << move_Real.fromPos.col << ")" << std::endl;
			std::cout << "To: " << "(" << move_Real.toPos.row << ":" << move_Real.toPos.col << ")" << std::endl;
			std::cout << "CapturePiece: " << (int)move_Real.capturedPiece << std::endl;
		}

		void Board_DEBUG::updateMoveLog(Position from, Position to, Piece capturedPiece,Piece movedPiece) {

			move_Real.fromPos = from;
			move_Real.toPos = to;
			move_Real.capturedPiece = capturedPiece;
			move_Real.movedPiece = movedPiece;
		}

		void Board_DEBUG::undo_Test(Board& b) {

			if (undoStack.empty())return;

			BoardToFEN_DEBUG fenDebugger;

			Move lastLog = undoStack.back();
			undoStack.pop_back();

			Piece p = b.getPiece(lastLog.toPos);
			Piece fromP = b.getPiece(lastLog.fromPos);
			b.setPiece(lastLog.fromPos, p);

			b.setPiece(lastLog.toPos, lastLog.capturedPiece);

			b.updateKingPosition(lastLog.fromPos, p);

			std::string enPassantString = fenDebugger.enPassantToString(lastLog);

		
			state_Real.switchTurn();

		}

		void Board_DEBUG::runTest(Board& b, BoardToFEN_DEBUG& fenDebugger, int numMoves) {
			undoStack.clear();
			std::string startFen = fenDebugger.toFEN(b, state_Real,move_Real);
			

			for (int i = 0; i < numMoves; i++) {
				Position from = fenDebugger.getRandom();
				Position to = fenDebugger.getRandom();
				moveListFrom.push_back(from);
				moveListTo.push_back(to);

				if (b.getPiece(from) != Piece::Empty && (from.row != to.row || from.col != to.col)) {
					move_Test(b,fenDebugger,from,to);
				}
			}
			std::cout << "Moved: " << undoStack.size() << " steps. Board is messy now.\n";

			std::cout << "FEN before revert: " << std::endl;

			fenDebugger.toFEN(b, state_Real,move_Real);
			std::cout << "----------------------------------------" << std::endl;
			fenDebugger.printBoard(b);

			std::cout << "UNDO......\n";

			int undoCount = 0;

			while (!undoStack.empty()) {
				
				undo_Test(b);

				fenDebugger.toFEN(b, state_Real, move_Real);

				fenDebugger.printBoard(b);
				
				undoCount++;
			}

			std::string endFen = fenDebugger.toFEN(b, state_Real,move_Real);
			std::cout << "Undo performed: " << undoCount << " times.\n";
			std::cout << "Start FEN: " << startFen << "\n";
			std::cout << "End FEN:   " << endFen << "\n";

			if (startFen == endFen) {
				std::cout << "[ SUCCESS ]: Undo system is bulletproof!\n";
			}
			else {
				std::cout << "[ FAILED ]: Board state drifted! Check your capturedPiece logic.\n";
			}
		}

		void Board_DEBUG::verifyAfterMove(Board& b, Position from, Position to, Piece movedPiece) {

			bool condition1 = b.getPiece(from) == Piece::Empty;
			bool condition2 = b.getPiece(to) == movedPiece;

			if (condition1 && condition2) {
				std::cout << "[PASS]" << std::endl;
			}
			else {
				if (!condition1)std::cout << "Piece still in old Position!" << std::endl;
				if (!condition2)std::cout << "Piece COULD NOT reach the new Position!" << std::endl;
		}
	}

		void Board_DEBUG::verifyAfterPushBack(Board& b, std::vector<Move>v) {

			if (undoStack.empty()) { std::cout << "Undo stack EMPTY!" << std::endl; return; }
			Move lastLog = undoStack.back();

			Piece pieceAtFrom = b.getPiece(lastLog.fromPos);
			Piece pieceAtTo = b.getPiece(lastLog.toPos);

			std::cout << "------Stack Check------" << std::endl;

			bool fromCheck = (pieceAtFrom == Piece::Empty);

			bool toCheck = (pieceAtTo != Piece::Empty);

			if (fromCheck && toCheck) {
				std::cout << "[ SYNC PASS ]\n";
				std::cout << "   + From (" << lastLog.fromPos.row << ":" << lastLog.fromPos.col << ") is now EMPTY.\n";
				std::cout << "   + To   (" << lastLog.toPos.row << ":" << lastLog.toPos.col << ") is now OCCUPIED by Piece " << (int)pieceAtTo << ".\n";
				std::cout << "   + Captured Data in Stack: " << (int)lastLog.capturedPiece << "\n";
			}
			else {
				std::cout << "[ SYNC FAIL ] - Board's data and stack'data are not synced !\n";
				if (!fromCheck) std::cout << "   -> Error: from is still have piece!\n";
				if (!toCheck)   std::cout << "   -> Error: to is still have piece!\n";
			}

		}