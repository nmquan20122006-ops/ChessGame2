#pragma once
#include<TGUI/TGUI.hpp>
#include<TGUI/Backends/SFML.hpp>


class TextureManager;
struct MoveRecord;

class InformationPanel {

private:

	tgui::Panel::Ptr			panel;
	tgui::Grid::Ptr				grid;
	tgui::Label::Ptr            titleLabel;
	tgui::ScrollablePanel::Ptr	scrollPanel;
	tgui::Font					tguiFont;
	tgui::Panel::Ptr            separator;

public:
	InformationPanel(tgui::GuiSFML& gui);

	void setFont(const std::string& filePath);

	void updateLayout(float boardLeftEdgeRatio);

	void updateMoveHistory(const std::vector<MoveRecord>& records);

	void clear();

};