#include "InformationPanel.h"
#include"MoveLog.h"
#include"EventBus.h"
#include"PieceRenderer.h"

InformationPanel::InformationPanel(tgui::GuiSFML& gui, PieceRenderer& t): pieceRenderer(t) { initCapturedPanel(gui);}

void InformationPanel::initCapturedPanel(tgui::GuiSFML& gui) {
    
    w_capturedPanel = tgui::Panel::create();
    b_capturedPanel = tgui::Panel::create();
    root = tgui::Panel::create();

    root->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    gui.add(root);

    auto bg = tgui::Panel::create();
    bg->setPosition(0, "(parent.height) / 4+ 2,5% ");
    bg->setSize("100%", "50% - 40");
    bg->getRenderer()->setBackgroundColor({ 32,30,27 });
    bg->getRenderer()->setRoundedBorderRadius(10);
    root->add(bg);

    scroll = tgui::ScrollablePanel::create();
    scroll->setPosition(2, 0);
    scroll->setSize("100% - 10", "100% - 10");
    scroll->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    scroll->getRenderer()->setScrollbarWidth(0);
    scroll->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);

    bg->add(scroll);

    b_capturedPanel->setPosition(0, 0);
    b_capturedPanel->setSize("100%", "25%");
    b_capturedPanel->getRenderer()->setBackgroundColor({ 32,30,27 });
    b_capturedPanel->getRenderer()->setRoundedBorderRadius(10);

    root->add(b_capturedPanel);

    w_capturedPanel->setPosition(0, "parent.height - height");
    w_capturedPanel->setSize("100%","25%");
    w_capturedPanel->getRenderer()->setBackgroundColor({ 32,30,27 });
    w_capturedPanel->getRenderer()->setRoundedBorderRadius(10);

    root->add(w_capturedPanel);
}

void InformationPanel::setFont(const std::string& filePath) {
    tguiFont = tgui::Font(filePath);
}

InformationPanel::Row InformationPanel::createRow(float y) {
    Row r;
    float W = scroll->getSize().x;

    float wNum = W * 0.15f;
    float wMove = W * 0.40f; 

    float xNum = 5.f;
    float xW = xNum + wNum;
    float xB = xW + wMove + 5.f; 

    r.panel = tgui::Panel::create({ W, rowHeight });
    r.panel->setPosition(0, y);
    r.panel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

    r.whiteBg = tgui::Panel::create({ wMove, rowHeight * 0.8f });
    r.whiteBg->setPosition(xW, rowHeight * 0.05f);
    r.whiteBg->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    r.whiteBg->getRenderer()->setRoundedBorderRadius(6);

    r.blackBg = tgui::Panel::create({ wMove, rowHeight * 0.9f });
    r.blackBg->setPosition(xB, rowHeight * 0.05f);
    r.blackBg->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    r.blackBg->getRenderer()->setRoundedBorderRadius(6);

    // KHỞI TẠO LABEL
    r.num = tgui::Label::create();
    r.white = tgui::Label::create();
    r.black = tgui::Label::create();

    for (auto lbl : { r.num, r.white, r.black }) {
        lbl->setSize("100%", "100%");
        lbl->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        lbl->setTextSize(15);
        if (tguiFont) lbl->getRenderer()->setFont(tguiFont);
    }

    r.num->setPosition(xNum, 0);
    r.num->setSize(wNum, "100%");
    r.num->getRenderer()->setTextColor({ 110,105,100 });

    r.white->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    r.black->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);

    r.panel->add(r.num);
    r.panel->add(r.whiteBg);
    r.panel->add(r.blackBg);
    r.whiteBg->add(r.white);
    r.blackBg->add(r.black);

    auto line = tgui::SeparatorLine::create();
    line->setSize("100%", 1);
    line->setPosition(0, rowHeight - 1);
    line->getRenderer()->setColor({ 45, 42, 38 });
    r.panel->add(line);

    scroll->add(r.panel);
    return r;
}

void InformationPanel::update(const std::vector<MoveRecord>& moves) {

    if (moves.empty()) {
        for (auto& row : rows) {
            row.panel->setVisible(false);
            row.white->setText("");
            row.black->setText("");
            row.num->setText("");
        }
        scroll->setContentSize({ 0, 0 });
        return;
    }
    int neededRows = (moves.size() + 1) / 2;

    while ((int)rows.size() < neededRows) {
        float y = startY + rows.size() * rowHeight;
        rows.push_back(createRow(y));
    }

    for (int i = 0; i < neededRows; i++) {
        int idx = i * 2;

        auto& row = rows[i];

        row.whiteBg->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
        row.blackBg->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

        row.panel->setVisible(true);

        row.num->setText(std::to_string(i + 1) + ".");

        row.white->getRenderer()->setTextColor({ 195,190,185 });
        row.black->getRenderer()->setTextColor({ 195,190,185 });

        // white
        row.white->setText(moves[idx].san);

        // black
        if (idx + 1 < (int)moves.size()) {
            row.black->setText(moves[idx + 1].san);
        }
        else {
            row.black->setText("");
        }
    }

    if (!moves.empty()) {
        int lastIdx = moves.size() - 1;
        int r = lastIdx / 2;
        if (lastIdx % 2 == 0) {
            rows[r].whiteBg->getRenderer()->setBackgroundColor({ 245, 220, 0, 20 });
            rows[r].white->getRenderer()->setTextColor(tgui::Color::White);
        }
        else {
            rows[r].blackBg->getRenderer()->setBackgroundColor({ 245, 220, 0, 20 });
            rows[r].black->getRenderer()->setTextColor(tgui::Color::White);
        }

        for (int i = neededRows; i < (int)rows.size(); i++) {
            rows[i].panel->setVisible(false);
        }

        float totalHeight = startY + neededRows * rowHeight;
        scroll->setContentSize({ 0, totalHeight });
        scroll->setVerticalScrollbarValue(20000);
    }
}

void InformationPanel::updateCapturedList(const std::vector<Piece>& w_Piece, const std::vector<Piece>& b_Piece) {
    if (!w_capturedPanel || !b_capturedPanel) return;

    w_capturedPanel->removeAllWidgets();
    b_capturedPanel->removeAllWidgets();

    auto render = [&](tgui::Panel::Ptr panel, const std::vector<Piece>& list) {
        if (list.empty()) return;

        float xPos = 0.f;
        float pieceSize = 50.f;     
        float spacing = 25.f;
        float panelHeight = panel->getSize().y;
        float yPos = panelHeight - pieceSize - 3.f;

        for (const auto& piece : list) {
            if (piece == Piece::Empty) continue;

            sf::Texture& sfTex = pieceRenderer.getCapturedPieceTexture(piece);
            auto pic = tgui::Picture::create(tgui::Texture(sfTex));
            pic->setSize(pieceSize, pieceSize); 
            pic->setPosition(xPos, yPos);

            panel->add(pic);
            xPos += spacing;
        }
        };

    render(b_capturedPanel, w_Piece);
    render(w_capturedPanel, b_Piece);
}

void InformationPanel::updateLayout(float boardLeftEdgeRatio) {
    if (!root) return;
    root->setVisible(boardLeftEdgeRatio >= 0.15f);
    if (!root->isVisible()) return;
      
    root->setPosition({ 15, 15 });

    float wPercent = boardLeftEdgeRatio * 100.f;
    root->setSize({
        tgui::Layout(std::to_string(wPercent) + "% - 30"),
        tgui::Layout("100% - 30")
        });
}