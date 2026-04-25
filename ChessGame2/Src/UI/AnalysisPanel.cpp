#include "AnalysisPanel.h"

AnalysisPanel::AnalysisPanel(tgui::GuiSFML& gui) {
    panel = tgui::Panel::create();
    
    panel->getRenderer()->setBackgroundColor({ 49, 46, 43 });

    panel->getRenderer()->setRoundedBorderRadius(10);

    gui.add(panel);
   
    updateLayout(0.7f);
}

void AnalysisPanel::updateLayout(float boardRightEdgeRatio) {
    if (!panel) return;

    if ((1.0f - boardRightEdgeRatio) < 0.15f) {
        panel->setVisible(false);
        return;
    }
    panel->setVisible(true);

    int margin = 15;
    
    std::string posX = std::to_string(boardRightEdgeRatio * 100.f) + "% + " + std::to_string(margin);

    std::string posY = std::to_string(margin);

    std::string width = std::to_string((1.0f - boardRightEdgeRatio) * 100.f) + "% - " + std::to_string(margin * 2);

    std::string height = "100% - " + std::to_string(margin * 2);

    panel->setPosition(posX.c_str(), posY.c_str());
    panel->setSize(width.c_str(), height.c_str());
}