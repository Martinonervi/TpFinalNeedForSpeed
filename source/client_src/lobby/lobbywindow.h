#pragma once

#include <QMainWindow>
#include <vector>

#include "../client_protocol.h"
#include <QVector>
#include <QString>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui {
class LobbyWindow;
}
QT_END_NAMESPACE

class LobbyWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LobbyWindow(ClientProtocol& protocol, bool& was_closed, QWidget *parent = nullptr);
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

    // Car selector
    void on_backButtonCar_clicked();
    void on_prevCarButton_clicked();
    void on_nextCarButton_clicked();
    void on_selectCarButton_clicked();

private:

    // Lobby selector
    void populateGames(const std::vector<GameMetadata>& games);
    void applyLobbySelectorStyles();
    void applyBackgroundSkin();
    void applyBackgroundLobbySelector();

    // Car selector
    void applyCarSelectorStyles();
    void initCarSelector(const QVector<QString>& sprites);
    void updateCarView();

    Ui::LobbyWindow *ui;
    ClientProtocol& protocol;
    QVector<QString> m_carSprites;
    int m_currentCar{0};
    ID joined_id{0};
    bool& was_closed;
};
