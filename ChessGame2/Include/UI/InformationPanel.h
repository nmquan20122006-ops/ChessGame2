#pragma once
#include<TGUI/TGUI.hpp>
#include<TGUI/Backends/SFML.hpp>

class InformationPanel {

private:

	tgui::Panel::Ptr panel;

public:

	InformationPanel(tgui::GuiSFML& gui);

	void updateLayout(float boardLeftEdgeRatio);

};