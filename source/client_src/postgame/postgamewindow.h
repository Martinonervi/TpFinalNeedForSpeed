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
class postgamewindow;
}

class postgamewindow: public QWidget {
    Q_OBJECT

public:
    explicit postgamewindow(PlayerStats& stats, QWidget* parent = nullptr);
    ~postgamewindow();

private:
    Ui::postgamewindow* ui;
    PlayerStats& stats;
};

#endif  // POSTGAMEWINDOW_H
