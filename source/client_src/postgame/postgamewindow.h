#ifndef POSTGAMEWINDOW_H
#define POSTGAMEWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <vector>

#include <QVector>
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

signals:
    void closeRequested();

private:
    Ui::PostGameWindow* ui;
    PlayerStats& stats;
};

#endif  // POSTGAMEWINDOW_H
