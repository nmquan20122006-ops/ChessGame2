#pragma once
#define NOMINMAX
#include<Windows.h>

constexpr int			BOARD_SIZE							= 8;
int const				SQUARE_SIZE							= 80;
int const				WINDOW_WIDTH						= 900;
int const				WINDOW_HEIGHT						= 800;
int const				OFFSET								= 30;
int const				BOARD_PIXEL_SIZE					= 640;
constexpr double		PI									= 3.14159265358979323846;
constexpr DWORD			PROCESS_TERMINATE_WAIT_MS			= 5000;
constexpr DWORD			PROCESS_TERMINATE_TIMEOUT_MS		= 5000;
const std::string		SOUND_PATH							= "Assets/Sound/";