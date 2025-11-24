#include "lobbywindow.h"

#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFontDatabase>

#include "../../common_src/cli_msg/disconnect_request.h"
#include "../../common_src/cli_msg/requestgame.h"
#include "../../common_src/srv_msg/joingame.h"

#include "ui_lobbywindow.h"

static QString ensurePixelFontLoaded() {
    static QString family;
    if (!family.isEmpty()) return family;

    const int id = QFontDatabase::addApplicationFont(":/fonts/pressstart2p.ttf");
    if (id >= 0) {
        const QStringList fams = QFontDatabase::applicationFontFamilies(id);
        if (!fams.isEmpty()) family = fams.first();
    }
    return family;
}

LobbyWindow::LobbyWindow(ClientProtocol& protocol, bool& was_closed, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LobbyWindow),
    protocol(protocol),
    was_closed(was_closed){
    ui->setupUi(this);

    if (const QString pix = ensurePixelFontLoaded(); !pix.isEmpty()) {
        qApp->setFont(QFont(pix));
    }
    applyBackgroundSkin();
    applyLobbySelectorStyles();
    applyCarSelectorStyles();
    initCarSelector({
        ":/cars/car0.png",
        ":/cars/car1.png",
        ":/cars/car2.png",
        ":/cars/car3.png",
        ":/cars/car4.png",
        ":/cars/car5.png",
        ":/cars/car6.png",
    });

    ui->stackedPages->setCurrentWidget(ui->MainMenu);

    ui->gamesTable->setColumnCount(3);
    ui->gamesTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Jugadores" << "Estado");
    ui->gamesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // ocupa todo el ancho
    ui->gamesTable->verticalHeader()->setVisible(false);
    ui->gamesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->gamesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->gamesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->gamesTable->setShowGrid(false);
    ui->gamesTable->setAlternatingRowColors(true);
    ui->gamesTable->setFocusPolicy(Qt::NoFocus);

    this->setFixedSize(this->size());
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    this->setFixedSize(this->size());
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

}

LobbyWindow::~LobbyWindow() {
    delete ui;
}

void LobbyWindow::populateGames(const std::vector<GameMetadata>& games) {
    auto* table = ui->gamesTable;

    table->clearContents();
    table->setRowCount(static_cast<int>(games.size()));

    for (int row = 0; row < static_cast<int>(games.size()); ++row) {
        const auto& g = games[row];

        auto* idItem = new QTableWidgetItem(QString::number(static_cast<quint32>(g.game_id)));
        idItem->setData(Qt::UserRole, static_cast<quint32>(g.game_id));

        auto* playersItem = new QTableWidgetItem(QString::number(g.players));
        auto* statusItem  = new QTableWidgetItem(g.started ? "En curso" : "Esperando");

        table->setItem(row, 0, idItem);
        table->setItem(row, 1, playersItem);
        table->setItem(row, 2, statusItem);
    }

    const int viewportH = table->viewport()->height();
    int rowH = (table->rowCount() > 0 ? table->rowHeight(0)
                                      : table->fontMetrics().height() + 12);
    if (rowH <= 0) rowH = 12;

    const int minRowsToFill = qMax(0, viewportH / rowH);
    if (table->rowCount() < minRowsToFill) {
        const int extra = minRowsToFill - table->rowCount();
        const int start = table->rowCount();
        table->setRowCount(start + extra);
        for (int r = start; r < start + extra; ++r) {
            for (int c = 0; c < table->columnCount(); ++c) {
                auto* it = new QTableWidgetItem(QString());
                it->setFlags(Qt::NoItemFlags);
                it->setBackground(Qt::NoBrush);
                table->setItem(r, c, it);
            }
        }
    }
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
    was_closed = true;
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
        std::cout << "Opcode: " <<static_cast<int>(op) << static_cast<Opcode>(op) <<std::endl;
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
        ui->stackedPages->setCurrentWidget(ui->CarSelector);
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
        joined_id = info.getGameID();
        QMessageBox::information(this, "Crear partida",
                                 "Partida creada y unida correctamente.");
        ui->stackedPages->setCurrentWidget(ui->CarSelector);
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
        "  font-size: 20px;"
        "  font-weight: 900;"
        "  letter-spacing: 1px;"
        "}"
        "QLabel#subtitleLabel {"
        "  color: #c8d0d8;"
        "  font-size: 9px;"
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
        "  font-size: 11px;"
        "  padding: 6px 17px;"
        "  color: white;"
        "}"
        "QPushButton#createButton { background: #2ecc40; border: 3px solid #0d5f1a; padding: 6px 16px; }"
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
        "  min-height: 25px;"
        "  border: 3px solid #7A1E18;"
        "  color: white;"
        "  font-weight: 800;"
        "  font-size: 12px;"
        "  border-radius: 12px;"
        "  padding: 10px 26px;"
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


// ------------ CAR SELECTOR ------------ //

void LobbyWindow::applyCarSelectorStyles() {
    ui->CarSelector->setStyleSheet("background: transparent;");

    ui->carPanel->setStyleSheet(
        "QFrame#carPanel {"
        "  background: rgba(8,12,20,220);"
        "  border: 2px solid #0f2236;"
        "  border-radius: 14px;"
        "}"
        "QLabel#carTitleLabel {"
        "  color: #f3f0d0;"
        "  font-size: 28px;"
        "  font-weight: 900;"
        "  letter-spacing: 1px;"
        "}"
        "QLabel#carInfoLabel {"
        "  color: #f3f0d0;"
        "  font-size: 20px;"
        "  font-weight: 700;"
        "  padding: 6px 0 10px 0;"
        "  border-bottom: 1px solid rgba(255,255,255,0.08);"
        "  qproperty-alignment: AlignHCenter;"
        "}"
        /* flechas */
        "QPushButton#prevCarButton, QPushButton#nextCarButton {"
        "  min-width: 64px; min-height: 64px;"
        "  border: 3px solid #1f3e61;"
        "  background: #2f5c8b;"
        "  color: white; font-size: 26px; font-weight: 900;"
        "  border-radius: 10px;"
        "}"
        "QPushButton#prevCarButton:hover, QPushButton#nextCarButton:hover {"
        "  background: #3a6ea7;"
        "}"
        /* botón principal */
        "QPushButton#selectCarButton {"
        "  min-height: 48px;"
        "  background: #2ecc40;"
        "  border: 3px solid #0d5f1a;"
        "  color: white; font-weight: 900; font-size: 14px;"
        "  border-radius: 12px;"
        "}"
        "QPushButton#selectCarButton:hover { background: #3fe24f; }"
        "QPushButton#selectCarButton:pressed { background: #1fa72f; }"
    );

    ui->backButtonCar->setStyleSheet(
        "QPushButton#backButtonCar {"
        "  background: #E74C3C;"
        "  min-height: 25px;"
        "  border: 3px solid #7A1E18;"
        "  color: white;"
        "  font-weight: 800;"
        "  font-size: 12px;"
        "  border-radius: 12px;"
        "  padding: 10px 26px;"
        "}"
        "QPushButton#backButtonCar:hover   { background: #FF5E4F; }"
        "QPushButton#backButtonCar:pressed { background: #C83A2C; }"
        "QPushButton#backButtonCar:disabled{ background:#9E9E9E; border-color:#5E5E5E; color:#E6E6E6; }"
    );

    auto *shadow = new QGraphicsDropShadowEffect(ui->carPanel);
    shadow->setBlurRadius(28);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0,0,0,160));
    ui->carPanel->setGraphicsEffect(shadow);
}

void LobbyWindow::initCarSelector(const QVector<QString>& sprites) {
    m_carSprites = sprites;
    if (m_carSprites.isEmpty()) { // necesario (?)
        ui->carInfoLabel->setText("No hay autos");
        ui->carImage->clear();
        ui->prevCarButton->setEnabled(false);
        ui->nextCarButton->setEnabled(false);
        ui->selectCarButton->setEnabled(false);
        return;
    }
    m_currentCar = 0;
    ui->prevCarButton->setEnabled(true);
    ui->nextCarButton->setEnabled(true);
    ui->selectCarButton->setEnabled(true);
    updateCarView();
}

void LobbyWindow::updateCarView() {
    const int total = m_carSprites.size();
    if (total == 0) return;

    // texto “Auto X / N”
    ui->carInfoLabel->setText(QString("Auto %1 / %2").arg(m_currentCar + 1).arg(total));

    QPixmap pm(m_carSprites[m_currentCar]);
    if (!pm.isNull()) {
        QSize target = ui->carImage->size();
        ui->carImage->setPixmap(pm.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        ui->carImage->setText("Sin imagen");
    }

    ui->prevCarButton->setEnabled(m_currentCar > 0);
    ui->nextCarButton->setEnabled(m_currentCar < total - 1);
}

void LobbyWindow::on_backButtonCar_clicked() {
    int n = 0;
    DisconnectReq dr(joined_id);
    CliMsgPtr msg = std::make_shared<DisconnectReq>(dr);
    protocol.sendDisconnectReq(dr);
    joined_id = 0;

    /*
    Op opcode = protocol.readActionByte();

    if (opcode == INIT_PLAYER) {
        std::cout << "recibí init player" << std::endl;
        protocol.recvSendPlayer();
    }
    */

    ui->stackedPages->setCurrentWidget(ui->LobbySelector);
    on_refreshButton_clicked();
}

void LobbyWindow::on_prevCarButton_clicked() {
    if (m_currentCar >= 0) { --m_currentCar; updateCarView(); }
}

void LobbyWindow::on_nextCarButton_clicked() {
    if (m_currentCar + 1 < m_carSprites.size()) { ++m_currentCar; updateCarView(); }
}

void LobbyWindow::on_selectCarButton_clicked() {
    int n = 0;
    auto type = static_cast<CarType>(m_currentCar);
    InitPlayer ip("jugador", type);
    CliMsgPtr msg = std::make_shared<InitPlayer>(ip);
    try {
        n = protocol.sendInitPlayer(dynamic_cast<const InitPlayer&>(*msg));
    }catch (const std::exception& e) {
        QMessageBox::warning(this, "Seleccionar auto",
                     "Respuesta inesperada del servidor.");
        return;
    }
    if (n == 0) {
        QMessageBox::warning(this, "Seleccionar auto",
                     "Respuesta inesperada del servidor.");
        return;
    }
    close();
}