#pragma once
#include<functional>
#include<SFML/System/Vector2.hpp>
#include"Utility.h"
#include"State/GameState.hpp"

class MoveService;
class Board;

class InputHandler {
public:
	using	MoveRequestFn			= std::function<void(Position, Position)>;
	using	BlockCheckFn			= std::function<bool()>;
	using	PromotionRequestFn		= std::function<void(Piece)>;
	using	PendingToFn				= std::function<Position()>;

	void							setOnClickMove(MoveRequestFn fn) { m_onClickMove = fn; }
	void							setOnDragMove(MoveRequestFn fn)	{ m_onDragMove = fn; }
	void							setIsBlock(BlockCheckFn fn)	{ m_isBlocking = fn; }
	void							setOnPromotionSelected(PromotionRequestFn fn) { m_onPromotionSelected = fn; }
	void							setOnGetPendingTo(PendingToFn fn) { m_onGetPendingTo = fn; }
private:
	std::shared_ptr<MoveService>	m_moveService;
	std::shared_ptr<GameState>		m_gameState;
	std::shared_ptr<Board>			m_board;
	MoveRequestFn					m_onClickMove;
	MoveRequestFn					m_onDragMove;
	std::function<void()>			m_onDeselect;
	BlockCheckFn					m_isBlocking;
	PromotionRequestFn				m_onPromotionSelected;
	PendingToFn						m_onGetPendingTo;
	bool							isCurrentTurnPiece(Piece piece)const;
	bool							isBlocking()const { return m_isBlocking && m_isBlocking(); }
	void							select(Position pos);
	void							deselect();
	void							requestClickMove(Position from, Position to)const { if (m_onClickMove)m_onClickMove(from, to); }
	void							requestDragMove(Position from, Position to)const { if (m_onDragMove)m_onDragMove(from, to); }
public:
	InputHandler(std::shared_ptr<Board>b, std::shared_ptr<GameState>s, std::shared_ptr<MoveService> m);
	void							handleSquareSelection(Position pos);
	void							handlePress(Position pos, sf::Vector2f mousePos);
	void							handleMove(sf::Vector2f mousePos);
	void							handleRelease(Position to);
	void							handleClickPromotionPanel(sf::Vector2f mousePos);
};