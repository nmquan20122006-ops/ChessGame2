#pragma once
#include <TGUI/TGUI.hpp>
#include <TGUI/Backends/SFML.hpp>
#include "State/Aistate.hpp"
class EvalBar {
public:
    EvalBar(tgui::GuiSFML& gui);

    void updateLayout(float boardRightEdgeRatio, const sf::View& boardView, unsigned int windowWidth, unsigned int windowHeight);
    void updateEval(const EngineEvaluation& eval);

private:
    tgui::Panel::Ptr    m_background;
    tgui::Panel::Ptr    m_whitePart;
    tgui::Panel::Ptr    m_blackPart;
    tgui::Label::Ptr    m_scoreLabel;

    float m_currentRatio = 0.5f;

    static constexpr float BarWidth = 15.f;
    static constexpr int   Margin = 10;
    static constexpr int   Gap = 5;

    float m_barX = 0.f;
    float m_barY = 0.f;
    float m_barWidth = 12.f;
    float m_barHeight = 0.f;

    float EvalToRatio(const EngineEvaluation& eval) const;
    void  updateBar();
};