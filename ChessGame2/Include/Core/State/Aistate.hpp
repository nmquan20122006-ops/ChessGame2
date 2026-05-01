#pragma once
#include"Type.hpp"
#include<string>

struct AiState {

	bool isAiEnabled = false;

	Color aiTurn = Color::black;

	std::string bestMoveSuggestion;
};