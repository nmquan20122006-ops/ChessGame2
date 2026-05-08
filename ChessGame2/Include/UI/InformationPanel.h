#pragma once
#include<TGUI/TGUI.hpp>
#include<TGUI/Backends/SFML.hpp>
#include"Piece.h"
#include<vector>

class PieceRenderer;
struct MoveRecord;

class InformationPanel {
public:
    InformationPanel(tgui::GuiSFML& gui, PieceRenderer& p);

    void setFont(const std::string& path);
    void update(const std::vector<MoveRecord>& moves);
    void clear();
    void updateLayout(float ratio);

    void initCapturedPanel(tgui::GuiSFML& gui);

private:
    struct Row {
        tgui::Panel::Ptr panel;
        tgui::Label::Ptr num;
        tgui::Label::Ptr white;
        tgui::Label::Ptr black;
        tgui::Panel::Ptr whiteBg;
        tgui::Panel::Ptr blackBg;
    };

    PieceRenderer& pieceRenderer;

    tgui::Panel::Ptr root;
    tgui::ScrollablePanel::Ptr scroll;
    tgui::Font tguiFont;

    tgui::Panel::Ptr w_capturedPanel;
    tgui::Panel::Ptr b_capturedPanel;

    std::vector<Row> rows;

    float rowHeight = 30.f;
    float startY = 5.f;

    Row createRow(float y);

public:
    void updateCapturedList(const std::vector<Piece>& w_Piece, const std::vector<Piece>& b_Piece);
};