#include "lobbywindow.h"
#include "ui_lobbywindow.h"

LobbyWindow::LobbyWindow(ClientProtocol& protocol, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::LobbyWindow),
      protocol(protocol) {
    ui->setupUi(this);
    // arrancar en la página de menú
    ui->stackedPages->setCurrentWidget(ui->MainMenu);
}

LobbyWindow::~LobbyWindow() {
    delete ui;
}

void LobbyWindow::on_playButton_clicked() {
    // cambiar de MainMenu a LobbySelector
    ui->stackedPages->setCurrentWidget(ui->LobbySelector);
    protocol.requestGames();
    Op op = protocol.readActionByte();
    if (op == REQUEST_GAMES) {
        MetadataGames games = protocol.getMetadata();
        printGames(games.getMetadata());
    }

    // más adelante: pedir partidas al backend, etc.
}

void LobbyWindow::on_quitButton_clicked() {
    close();
}

void LobbyWindow::printGames(const std::vector<GameMetadata>& v) {
    if (v.empty()) { std::cout << "sin partidas\n"; return; }
    for (const auto& g : v) {
        std::cout << "id: " << static_cast<uint32_t>(g.game_id) << " cant jugadores: " << g.players << std::endl;
    }
}