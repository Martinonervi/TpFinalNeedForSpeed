#pragma once

#include <QMainWindow>
#include "../client_protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LobbyWindow;
}
QT_END_NAMESPACE

class LobbyWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LobbyWindow(ClientProtocol& protocol, QWidget *parent = nullptr);
    ~LobbyWindow() override;

private slots:
    void on_playButton_clicked();
    void on_quitButton_clicked();

private:
    Ui::LobbyWindow *ui;
    ClientProtocol& protocol;
    void printGames(const std::vector<GameMetadata>& v);
};
