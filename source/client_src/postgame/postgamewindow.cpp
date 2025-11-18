#include "postgamewindow.h"

#include "ui_postgamewindow.h"
#include <QGraphicsDropShadowEffect>

PostGameWindow::PostGameWindow(PlayerStats& stats,QWidget* parent)
    :QWidget(parent), stats(stats), ui(new Ui::PostGameWindow) {
    ui->setupUi(this);
    applyStyles();

    connect(ui->closeButton, &QPushButton::clicked,
            this, &PostGameWindow::closeRequested);

    auto* sh = new QGraphicsDropShadowEffect(ui->panel);
    sh->setBlurRadius(28);
    sh->setOffset(0, 6);
    sh->setColor(QColor(0,0,0,160));
    ui->panel->setGraphicsEffect(sh);
}

PostGameWindow::~PostGameWindow() { delete ui; }

void PostGameWindow::setStats(const QString& pos, const QString& time, const QString& bestLap) {
    ui->posEdit->setText(pos);
    ui->timeEdit->setText(time);
}

void PostGameWindow::applyStyles() {
    // fondo transparente (tu fondo global lo pone LobbyWindow)
    this->setAttribute(Qt::WA_StyledBackground, true);

    this->setObjectName("PostGameWindow");

    const QString css = R"CSS(
        QWidget#PostGamePanel {
            background-image: url(:/postgame/lobby_bg.png);
            background-repeat: no-repeat;
            background-position: center;
            background-color: black;
        }

        QFrame#panel {
          background: rgba(10,14,18,230);
          border: 2px solid #0f2236;
          border-radius: 16px;
        }

        QLabel#titleLabel {
          color: #F3F0D0;
          font-size: 32px; font-weight: 900; letter-spacing: 1px;
        }
        QLabel#subtitleLabel {
          color: #C8D0D8;
          font-size: 16px; margin-bottom: 6px;
        }

        QFrame#divider {
          background: rgba(255,255,255,0.10);
          max-height: 1px; min-height: 1px;
        }

        QLabel { color: #E6EDF3; font-size: 20px; }
        QLineEdit {
          background: rgba(255,255,255,0.06);
          border: 2px solid rgba(255,255,255,0.10);
          border-radius: 8px;
          color: #E6EDF3; padding: 6px 10px;
        }
        QLineEdit:read-only { color: #E6EDF3; }

        QPushButton#closeButton {
          min-height: 48px; min-width: 220px;
          background: #E74C3C;
          border: 3px solid #7A1E18;
          color: white; font-weight: 900; font-size: 20px;
          border-radius: 12px; padding: 10px 24px;
        }
        QPushButton#closeButton:hover   { background: #FF5E4F; }
        QPushButton#closeButton:pressed { background: #C83A2C; }
    )CSS";
    this->setStyleSheet(css);
}