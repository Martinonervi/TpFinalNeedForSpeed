#ifndef POSTGAMEWINDOW_H
#define POSTGAMEWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <vector>

#include "../client_protocol.h"
#include <QVector>
#include <QString>
#include <QPixmap>

namespace Ui {
class postgamewindow;
}

class postgamewindow: public QWidget {
    Q_OBJECT

public:
    explicit postgamewindow(ClientProtocol& protocol, QWidget* parent = nullptr);
    ~postgamewindow();

private:
    Ui::postgamewindow* ui;
    ClientProtocol& protocol;
};

#endif  // POSTGAMEWINDOW_H
