#pragma once
#include<string>
#include<memory>
#include<functional>
#include<queue>

#include"Utility.h"
#include"State/Aistate.hpp"

class IChessEngine;
class Board;
class MoveService;

class ChessEngineController {
public:
	ChessEngineController(
		std::shared_ptr<Board>			board,
		std::shared_ptr<GameState>		gameState,
		std::shared_ptr<MoveService>	moveService
		);
	using MoveReadyFn =					std::function<void(Move&)>;
	using HintReadyFn =					std::function<void(Position,Position)>;
	using EnemyBestMoveReadyFn =		std::function<void(Position, Position)>;
	void								init(EngineConfig& config);
	void								newGame();
	void								stop();
	void								syncPosition(const std::string& fen);
	void								setOnMoveReady(MoveReadyFn fn) { m_onMoveReady = fn; }
	void								setOnHintReady(HintReadyFn fn) { m_onHintReady = fn; }
	void								setOnEnemyBestMoveReady(EnemyBestMoveReadyFn fn) { m_onEnemyBestMoveReady = fn; }
	void								updateEngineMove();
	void								updateEngineHint();
	void								updateEngineEnemyBestMove();
	void								update();
	void								requestHint();
	void								requestEnemyBestMove();
	void								setEngine(std::unique_ptr<IChessEngine> chessEngine);
	bool								isThinking()const;
	bool								isEnable()const;
	EngineEvaluation					getEngineEvaluation()const;
private:
	void								processMove(const std::string& uciMove);
	void								processHint(const std::string& uciMove);
	void								processEnemyBestMove(const std::string& uciMove);
	std::shared_ptr<Board>				m_board;
	std::shared_ptr<MoveService>		m_moveService;
	std::shared_ptr<GameState>			m_gameState;
	std::unique_ptr<IChessEngine>		m_chessEngine;
	std::queue<EngineTaskQueue>			m_engineTaskQueue;
	MoveReadyFn							m_onMoveReady;
	EnemyBestMoveReadyFn				m_onEnemyBestMoveReady;
	HintReadyFn							m_onHintReady;
	EngineConfig						m_engineConfig;
	EngineThinkingMode					m_engineThinkingMode;
};