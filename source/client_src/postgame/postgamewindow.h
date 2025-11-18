#ifndef POSTGAMEWINDOW_H
#define POSTGAMEWINDOW_H

#include <QWidget>
#include <QMainWindow>

#include <QString>
#include <QPixmap>
#include "../../common_src/constants.h"
#include "../../common_src/srv_msg/playerstats.h"

namespace Ui {
class PostGameWindow;
}

class PostGameWindow: public QWidget {
    Q_OBJECT

public:
    explicit PostGameWindow(PlayerStats& stats, QWidget* parent = nullptr);
    ~PostGameWindow();

    void setStats(const QString& pos, const QString& time, const QString& bestLap);
    void applyStyles();

private slots:
    void on_closeButton_clicked();

private:
    void setStats(const QString& pos, const QString& time);
    Ui::PostGameWindow* ui;
    PlayerStats& stats;
};

#endif  // POSTGAMEWINDOW_H
