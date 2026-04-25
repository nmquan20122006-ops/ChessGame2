#include"InformationPanel.h"

InformationPanel::InformationPanel(tgui::GuiSFML& gui){
	
	panel = tgui::Panel::create();

	panel->getRenderer()->setBackgroundColor({ 49,46,43 });

	panel->getRenderer()->setRoundedBorderRadius(10);

	gui.add(panel);

}

void InformationPanel::updateLayout(float boardLeftEdgeRatio) {

	if (!panel)return;

	if (boardLeftEdgeRatio < 0.15f) {
		panel->setVisible(false);
		return;
	}

	panel->setVisible(true);

	int margin = 15;

	std::string posX = std::to_string(margin);
	std::string width = std::to_string(boardLeftEdgeRatio * 100.f) + "% - " + std::to_string(margin * 2);

	std::string posY = std::to_string(margin);
	std::string height = "100% - " + std::to_string(margin * 2);

	panel->setPosition(posX.c_str(), posY.c_str());
	panel->setSize(width.c_str(), height.c_str());

}