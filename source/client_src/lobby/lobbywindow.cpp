#include "lobbywindow.h"
#include "ui_lobbywindow.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

#include "../../common_src/requestgame.h"
#include "../../common_src/joingame.h"

LobbyWindow::LobbyWindow(ClientProtocol& protocol, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::LobbyWindow),
      protocol(protocol) {
    ui->setupUi(this);

    applyBackgroundSkin();
    applyLobbySelectorStyles();

    ui->stackedPages->setCurrentWidget(ui->MainMenu);

    ui->gamesTable->setColumnCount(3);
    QStringList headers;
    headers << "ID" << "Jugadores" << "Estado";
    ui->gamesTable->setHorizontalHeaderLabels(headers);
    ui->gamesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->gamesTable->setSelectionMode(QAbstractItemView::SingleSelection);

    this->setFixedSize(this->size());
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

}

LobbyWindow::~LobbyWindow() {
    delete ui;
}

void LobbyWindow::populateGames(const std::vector<GameMetadata>& games) {
    ui->gamesTable->clearContents();
    ui->gamesTable->setRowCount(static_cast<int>(games.size()));

    for (int row = 0; row < static_cast<int>(games.size()); ++row) {
        const auto& g = games[row];

        // Columna 0: ID (visible y guardado en UserRole)
        auto* idItem = new QTableWidgetItem(
            QString::number(static_cast<quint32>(g.game_id)));
        idItem->setData(Qt::UserRole, static_cast<quint32>(g.game_id));

        // Columna 1: cantidad de jugadores
        auto* playersItem = new QTableWidgetItem(QString::number(g.players));

        // Columna 2: estado
        const QString estado = g.started ? "En curso" : "Esperando";
        auto* statusItem = new QTableWidgetItem(estado);

        ui->gamesTable->setItem(row, 0, idItem);
        ui->gamesTable->setItem(row, 1, playersItem);
        ui->gamesTable->setItem(row, 2, statusItem);
    }

    ui->gamesTable->resizeColumnsToContents();
}

/* Menú principal  */

void LobbyWindow::on_playButton_clicked() {
    // cambia a la página LobbySelector
    ui->stackedPages->setCurrentWidget(ui->LobbySelector);
    applyBackgroundLobbySelector();

    // pedir partidas al servidor
    protocol.requestGames();
    Op op = protocol.readActionByte();

    if (op == REQUEST_GAMES) {
        MetadataGames games = protocol.getMetadata();
        populateGames(games.getMetadata());
    } else {
        QMessageBox::warning(this, "Error",
                             "Respuesta inesperada al solicitar partidas.");
    }
}

void LobbyWindow::on_quitButton_clicked() {
    close();
}

/*  LobbySelector  */

void LobbyWindow::on_refreshButton_clicked() {
    protocol.requestGames();
    Op op = protocol.readActionByte();

    if (op == REQUEST_GAMES) {
        MetadataGames games = protocol.getMetadata();
        populateGames(games.getMetadata());
    } else {
        QMessageBox::warning(this, "Error",
                             "Respuesta inesperada al actualizar partidas.");
    }
}

void LobbyWindow::on_joinButton_clicked() {
    int row = ui->gamesTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Unirse a partida",
                                 "Seleccioná una partida de la lista primero.");
        return;
    }

    QTableWidgetItem* idItem = ui->gamesTable->item(row, 0);
    if (!idItem) {
        return;
    }

    quint32 idValue = idItem->data(Qt::UserRole).toUInt();
    ID game_id = static_cast<ID>(idValue);

    RequestGame req(game_id);
    protocol.sendRequestGame(req);

    Op op = protocol.readActionByte();
    if (op != JOIN_GAME) {
        QMessageBox::warning(this, "Unirse a partida",
                             "Respuesta inesperada del servidor.");
        return;
    }

    JoinGame info = protocol.recvGameInfo();

    if (info.couldJoin()) {
        QMessageBox::information(this, "Unirse a partida",
                                 "Te uniste correctamente a la partida.");
        close();
    } else {
        err_code code = info.getExitStatus();
        if (code == FULL_GAME) {
            QMessageBox::warning(this, "Unirse a partida",
                                 "La partida está llena.");
        } else if (code == INEXISTENT_GAME) {
            QMessageBox::warning(this, "Unirse a partida",
                                 "La partida no existe.");
        } else {
            QMessageBox::warning(this, "Unirse a partida",
                                 "No se pudo unir a la partida.");
        }
    }
}

void LobbyWindow::on_createButton_clicked() {
    ID game_id = 0;

    RequestGame req(game_id);
    protocol.sendRequestGame(req);

    Op op = protocol.readActionByte();
    if (op != JOIN_GAME) {
        QMessageBox::warning(this, "Crear partida",
                             "Respuesta inesperada del servidor.");
        return;
    }

    JoinGame info = protocol.recvGameInfo();

    if (info.couldJoin()) {
        QMessageBox::information(this, "Crear partida",
                                 "Partida creada y unida correctamente.");
        close();
    } else {
        err_code code = info.getExitStatus();
        if (code == FULL_GAME) {
            QMessageBox::warning(this, "Crear partida",
                                 "No se pudo crear la partida (lleno).");
        } else {
            QMessageBox::warning(this, "Crear partida",
                                 "No se pudo crear la partida.");
        }
    }
}


void LobbyWindow::on_backButton_clicked() {
    ui->stackedPages->setCurrentWidget(ui->MainMenu);
    applyBackgroundSkin();
}

void LobbyWindow::applyLobbySelectorStyles() {
    ui->panel->setStyleSheet(
        "QFrame#panel {"
        "  background: rgba(8,12,20,220);"
        "  border: 2px solid #0f2236;"
        "  border-radius: 14px;"
        "  padding: 16px;"
        "}"
        "QLabel#titleLabel {"
        "  color: #f3f0d0;"
        "  font-size: 28px;"
        "  font-weight: 900;"
        "  letter-spacing: 1px;"
        "}"
        "QLabel#subtitleLabel {"
        "  color: #c8d0d8;"
        "  font-size: 14px;"
        "  padding-bottom: 8px;"
        "  border-bottom: 1px solid rgba(255,255,255,0.08);"
        "}"
        "QTableWidget#gamesTable {"
        "  background: transparent;"
        "  color: #e6edf3;"
        "  gridline-color: rgba(255,255,255,0.05);"
        "  selection-background-color: rgba(61,139,253,120);"
        "  selection-color: white;"
        "  alternate-background-color: rgba(255,255,255,0.03);"
        "}"
        "QTableWidget#gamesTable::item { padding: 6px; }"
        "QHeaderView::section {"
        "  background: rgba(255,255,255,0.06);"
        "  color: #e6e2c8;"
        "  border: none;"
        "  border-bottom: 1px solid rgba(255,255,255,0.07);"
        "  padding: 8px 10px;"
        "  font-weight: 700;"
        "}"
        "QTableCornerButton::section {"
        "  background: rgba(255,255,255,0.06);"
        "  border: none;"
        "}"
        // Botones dentro del panel
        "QPushButton {"
        "  min-height: 44px;"
        "  border-radius: 10px;"
        "  font-weight: 800;"
        "  font-size: 18px;"
        "  padding: 6px 20px;"
        "  color: white;"
        "}"
        "QPushButton#createButton { background: #2ecc40; border: 3px solid #0d5f1a; }"
        "QPushButton#createButton:hover { background: #3fe24f; }"
        "QPushButton#createButton:pressed { background: #1fa72f; }"
        "QPushButton#joinButton { background: #3a6ea7; border: 3px solid #1f3e61; }"
        "QPushButton#joinButton:hover { background: #497fbb; }"
        "QPushButton#joinButton:pressed { background: #2f5c8b; }"
        "QPushButton#refreshButton { background: #6b6f78; border: 3px solid #3a3d44; }"
        "QPushButton#refreshButton:hover { background: #7a7f88; }"
        "QPushButton#refreshButton:pressed { background: #5a5e66; }"
        "QFrame#panel {"
        "  background: rgba(8,12,20,220);"
        "  border: 2px solid #0f2236;"
        "  border-radius: 14px;"
        "  padding: 16px;"
        "}"
    );

    auto *shadow = new QGraphicsDropShadowEffect(ui->panel);
    shadow->setBlurRadius(28);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0,0,0,160));
    ui->panel->setGraphicsEffect(shadow);

    ui->backButton->setStyleSheet(
        "QPushButton#backButton {"
        "  background: #E74C3C;"
        "  border: 3px solid #7A1E18;"
        "  color: white;"
        "  font-weight: 800;"
        "  font-size: 22px;"
        "  border-radius: 12px;"
        "  padding: 10px 36px;"
        "}"
        "QPushButton#backButton:hover   { background: #FF5E4F; }"
        "QPushButton#backButton:pressed { background: #C83A2C; }"
        "QPushButton#backButton:disabled{ background:#9E9E9E; border-color:#5E5E5E; color:#E6E6E6; }"
    );
}

void LobbyWindow::applyBackgroundSkin() {
    ui->MainMenu->setAttribute(Qt::WA_StyledBackground, true);
    ui->LobbySelector->setAttribute(Qt::WA_StyledBackground, true);

    ui->stackedPages->setStyleSheet(QString());

    ui->MainMenu->setObjectName("MainMenu");
    ui->LobbySelector->setObjectName("LobbySelector");

    const QString css = R"CSS(
        /* Fondo por defecto del QMainWindow */
        QMainWindow#LobbyWindow {
            background-image: url(:/images/menu_bg_clean.png);
            background-repeat: no-repeat;
            background-position: center;
            background-color: black;
        }

        QWidget#MainMenu {
            background-image: url(:/images/menu_bg_clean.png);
            background-repeat: no-repeat;
            background-position: center;
            background-color: transparent;
        }

        QWidget#LobbySelector {
            background-image: url(:/images/lobby_bg.png);
            background-repeat: no-repeat;
            background-position: center;
            background-color: transparent;
        }
    )CSS";

    this->setStyleSheet(css);
}

void LobbyWindow::applyBackgroundLobbySelector() {
    ui->MainMenu->setAttribute(Qt::WA_StyledBackground, true);
    ui->LobbySelector->setAttribute(Qt::WA_StyledBackground, true);

    ui->stackedPages->setStyleSheet(QString());

    ui->LobbySelector->setObjectName("LobbySelector");
    ui->MainMenu->setObjectName("MainMenu");

    const QString css = R"CSS(
        /* Fondo por defecto del QMainWindow */
        QMainWindow#LobbyWindow {
            background-image: url(:/images/lobby_bg.png);
            background-repeat: no-repeat;
            background-position: center;
            background-color: black;
        }
        QWidget#MainMenu {
            background-image: url(:/images/menu_bg_clean.png);
            background-repeat: no-repeat;
            background-position: center;
            background-color: transparent;
        }
    )CSS";

    this->setStyleSheet(css);
}