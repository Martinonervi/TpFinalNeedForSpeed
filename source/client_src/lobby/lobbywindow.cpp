#include "lobbywindow.h"
#include "ui_lobbywindow.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>

#include "../../common_src/requestgame.h"
#include "../../common_src/joingame.h"

LobbyWindow::LobbyWindow(ClientProtocol& protocol, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::LobbyWindow),
      protocol(protocol) {
    ui->setupUi(this);

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
}
