#pragma once

#include <QMainWindow>
#include <vector>

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
        // Menú principal
        void on_playButton_clicked();
    void on_quitButton_clicked();

    // LobbySelector
    void on_refreshButton_clicked();
    void on_joinButton_clicked();
    void on_backButton_clicked();
    void on_createButton_clicked();

private:
    Ui::LobbyWindow *ui;
    ClientProtocol& protocol;

    void populateGames(const std::vector<GameMetadata>& games);
    void applyLobbySelectorStyles();
    void applyBackgroundSkin();
    void applyBackgroundLobbySelector();
};
