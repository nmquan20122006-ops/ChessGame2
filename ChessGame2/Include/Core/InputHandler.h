#pragma once
#include<functional>
#include<SFML/System/Vector2.hpp>

#include"Utility.h"
#include"State/GameState.hpp"

class MoveService;

class InputHandler {
public:
	using	MoveRequestFn		= std::function<void(Position, Position)>;
	using	BlockCheckFn		= std::function<bool()>;

	void	setOnClickMove(MoveRequestFn fn)			{ onClickMove = fn; }
	void	setOnDragMove(MoveRequestFn fn)				{ onDragMove = fn; }
	void    setIsBlock(BlockCheckFn fn)					{ isBlocking = fn; }

private:
	std::shared_ptr<MoveService> moveService;
	std::shared_ptr<GameState>  gameState;
	std::shared_ptr<Board>		board;

	MoveRequestFn				onClickMove;
	MoveRequestFn				onDragMove;
	std::function<void()>		onDeselect;
	BlockCheckFn				isBlocking;

	bool	IsCurrentTurnPiece(Piece piece)const;
	bool	IsBlocking()const							 { return isBlocking && isBlocking(); }
	void	Select(Position pos);
	void	Deselect();
	void	RequestClickMove(Position from, Position to)const { if (onClickMove)onClickMove(from, to); }
	void	RequestDragMove(Position from, Position to)const { if (onDragMove)onDragMove(from, to); }
public:
	InputHandler(std::shared_ptr<Board>b, std::shared_ptr<GameState>s, std::shared_ptr<MoveService> m);

	void	HandleSquareSelection(Position pos);
	void	HandlePress(Position pos, sf::Vector2f mousePos);
	void	HandleMove(sf::Vector2f mousePos);
	void	HandleRelease(Position to);
	void	HandleClickPromotionPanel(sf::Vector2f mousePos);
};