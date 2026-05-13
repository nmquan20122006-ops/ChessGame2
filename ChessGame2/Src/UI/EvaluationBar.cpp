#include "EvaluationBar.h"
#include <cmath>

EvalBar::EvalBar(tgui::GuiSFML& gui) {
    // Nền đen
    m_background = tgui::Panel::create();
    m_background->getRenderer()->setBackgroundColor({ 30, 30, 30 });
    m_background->getRenderer()->setRoundedBorderRadius(5);
    gui.add(m_background);
    
    m_whitePart = tgui::Panel::create();
    m_whitePart->getRenderer()->setBackgroundColor({ 240, 240, 240 });
    m_whitePart->getRenderer()->setRoundedBorderRadius(6);
    gui.add(m_whitePart);  // add vào gui, không phải m_background

    m_blackPart = tgui::Panel::create();
    m_blackPart->getRenderer()->setBackgroundColor({ 30, 30, 30 });
    m_blackPart->getRenderer()->setRoundedBorderRadius(6);
    gui.add(m_blackPart);

    m_scoreLabel = tgui::Label::create();
    m_scoreLabel->setTextSize(11);
    m_scoreLabel->getRenderer()->setTextColor({ 200, 200, 200 });
    m_scoreLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    gui.add(m_scoreLabel);
}

void EvalBar::updateLayout(float boardRightEdgeRatio,
    const sf::View& boardView,
    unsigned int windowWidth,
    unsigned int windowHeight) {

    float viewportRight = (boardView.getViewport().left
        + boardView.getViewport().width) * windowWidth;

    m_barX = viewportRight + 8.f;
    m_barY = windowHeight * 0.05f;
    m_barWidth = 12.f;
    m_barHeight = windowHeight * 0.90f;

    // Background — toàn bộ thanh
    m_background->setPosition(m_barX, m_barY);
    m_background->setSize(m_barWidth, m_barHeight);

    // Score label — phía dưới
    m_scoreLabel->setPosition(m_barX, m_barY + m_barHeight + 4.f);
    m_scoreLabel->setSize(m_barWidth, 20.f);

    updateBar();
}

float EvalBar::EvalToRatio(const EngineEvaluation& eval) const {
    if (eval.isCheckMate) {
        return eval.checkMateIn > 0 ? 1.0f : 0.0f;
    }
    float x = eval.score / 400.0f;
    return 1.0f / (1.0f + std::exp(-x));
}

void EvalBar::updateBar() {
    float whiteRatio = 1.f - m_currentRatio;
    float blackRatio = m_currentRatio;

    // White — trên
    m_whitePart->setPosition(m_barX, m_barY);
    m_whitePart->setSize(m_barWidth, whiteRatio * m_barHeight);

    // Black — dưới
    m_blackPart->setPosition(m_barX, m_barY + whiteRatio * m_barHeight);
    m_blackPart->setSize(m_barWidth, blackRatio * m_barHeight);
}


void EvalBar::updateEval(const EngineEvaluation& eval) {
    // Smooth lerp
    float target = EvalToRatio(eval);
    m_currentRatio += (target - m_currentRatio) * 0.1f;
    updateBar();

    // Update score text
    std::string text;
    if (eval.isCheckMate) {
        text = "M" + std::to_string(std::abs(eval.checkMateIn));
    }
    else {
        int   abs = std::abs(eval.score);
        float disp = abs / 100.f;
        text = (eval.score >= 0 ? "+" : "-")
            + std::to_string((int)disp) + "."
            + std::to_string(abs % 100 / 10);
    }
    m_scoreLabel->setText(text);
}