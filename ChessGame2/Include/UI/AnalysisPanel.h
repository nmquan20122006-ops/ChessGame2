#pragma once
#include<TGUI/TGUI.hpp>
#include<TGUI/Backends/SFML.hpp>

class AnalysisPanel {

private:
	tgui::Panel::Ptr panel;

public:
	AnalysisPanel(tgui::GuiSFML& gui);

	void updateLayout(float boardRightEdgeRatio);

	void draw();
		
};