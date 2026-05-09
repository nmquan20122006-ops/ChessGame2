#pragma once
#include<string>

class IChessEngine {
public:
	virtual ~IChessEngine() = default;

	virtual bool		init(const std::wstring& enginePath) = 0;
	virtual void		newGame(bool isPlayerWhite = true) = 0;
	virtual void		reset() = 0;
	virtual void		setDifficulty(int level) = 0;
	virtual void		syncPosition(const std::string& FEN) = 0;
	virtual void		startThinking(int thinkingTime) = 0;
	virtual void		stopThinking() = 0;
	virtual bool		isThinking()const = 0;
	virtual std::string getPendingMove() = 0;
	virtual void		goDepth(int depth) = 0;
	virtual bool		isGameOver()const = 0;

};