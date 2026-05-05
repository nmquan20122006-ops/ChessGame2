#include"InformationPanel.h"
#include"MoveLog.h"
#include"TextureManager.h"

InformationPanel::InformationPanel(tgui::GuiSFML& gui) {

    panel = tgui::Panel::create();
    panel->getRenderer()->setBackgroundColor({ 40, 36, 33 });
    panel->getRenderer()->setRoundedBorderRadius(12);
    gui.add(panel);

    titleLabel = tgui::Label::create();
    titleLabel->setText("");
    titleLabel->setTextSize(25);
    titleLabel->setPosition("50%", "6");
    titleLabel->getRenderer()->setTextColor({ 200, 170, 110,100 });
    titleLabel->setSize("100%", "50");
    titleLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    titleLabel->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    titleLabel->setPosition(0, 0);
    panel->add(titleLabel);

    scrollPanel = tgui::ScrollablePanel::create();
    scrollPanel->setPosition("0%", "40");
    scrollPanel->setSize("100%", "50%");
    scrollPanel->getRenderer()->setScrollbarWidth(8);

    auto renderer = scrollPanel->getRenderer();
    renderer->setBackgroundColor({ 30, 27, 24 }); 
    renderer->setPadding({ 10,10,10,10 });
    renderer->setScrollbarWidth(8);

    tgui::ScrollbarRenderer scrollbarRenderer(scrollPanel->getRenderer()->getScrollbar());
    scrollbarRenderer.setThumbColor({ 80, 70, 60 });
    scrollbarRenderer.setTrackColor({ 40, 36, 33 });
    scrollbarRenderer.setArrowBackgroundColor({ 40, 36, 33 });
    scrollbarRenderer.setArrowColor({ 80, 70, 60 });
    panel->add(scrollPanel);

    grid = tgui::Grid::create();
    grid->setPosition(0, 0);
    scrollPanel->add(grid);
}

void InformationPanel::setFont(const std::string& filePath) {
    tguiFont = tgui::Font(filePath);
}

void InformationPanel::updateMoveHistory(const std::vector<MoveRecord>& records) {

    grid->removeAllWidgets();

    float panelW    = scrollPanel->getSize().x;
    float numW      = 40.f;
    float moveW = (panelW - numW) / 2.f - 10.f;

    auto makeNumLabel = [&](const std::string& text) {
        auto label = tgui::Label::create();
        label->setText(text);
        label->setTextSize(14);
        label->getRenderer()->setFont(tguiFont);
        label->getRenderer()->setTextColor({ 100, 100, 100 });
        label->getRenderer()->setPadding({ 0, 0, 0, 0 });
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
        label->setSize(numW,30);
        return label;
    };

    auto makeMoveLabel = [&,this](const std::string& text, bool isLast) {
        auto label = tgui::Label::create();
        label->setText(text);
        label->setTextSize(14);
        label->getRenderer()->setFont(tguiFont);
        label->getRenderer()->setPadding({ 14, 0, 14, 0 });
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center); 
        
        label->setSize(moveW,30);

        if (isLast) {
            label->getRenderer()->setBackgroundColor({ 219, 196, 68, 20 });
            label->getRenderer()->setTextColor({ 255, 255, 255 });
        } else {
            
            label->getRenderer()->setTextColor({ 180, 180, 180 });
        }
        return label;
    };

    for (int i = 0; i < (int)records.size(); i += 2) {
        int row          = (i / 2);
        std::string num  = std::to_string(row + 1) + ".";
        std::string wSAN = records[i].san;
        std::string bSAN = (i + 1 < (int)records.size()) ? records[i + 1].san : "";

        int  lastIdx = (int)records.size() - 1;
        bool wIsLast = (i == lastIdx);
        bool bIsLast = (i + 1 == lastIdx);

        tgui::Color rowBg = (row % 2 == 0)
            ? tgui::Color{ 255, 255, 255, 10 }
            : tgui::Color{ 0, 0, 0, 10 };

        auto numLabel   = makeNumLabel(num);
        auto whiteLabel = makeMoveLabel(wSAN, wIsLast);
        auto blackLabel = makeMoveLabel(bSAN, bIsLast);

        numLabel->getRenderer()->setBackgroundColor(rowBg);
        if (!wIsLast) whiteLabel->getRenderer()->setBackgroundColor(rowBg);
        if (!bIsLast) blackLabel->getRenderer()->setBackgroundColor(rowBg);

        grid->addWidget(numLabel,   row, 0);
        grid->addWidget(whiteLabel, row, 1);
        grid->addWidget(blackLabel, row, 2);
    }

    scrollPanel->setVerticalScrollbarValue(100);

}
void InformationPanel::clear() {
    grid->removeAllWidgets();
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

