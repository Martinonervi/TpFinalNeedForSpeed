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

    // más adelante: pedir partidas al backend, etc.
}

void LobbyWindow::on_quitButton_clicked() {
    close();
}
