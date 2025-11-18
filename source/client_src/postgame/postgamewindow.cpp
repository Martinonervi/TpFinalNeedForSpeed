#include "postgamewindow.h"

#include "ui_postgamewindow.h"

postgamewindow::postgamewindow(PlayerStats& stats,QWidget* parent)
    :stats(stats), QWidget(parent), ui(new Ui::postgamewindow) {
    ui->setupUi(this);
}

postgamewindow::~postgamewindow() { delete ui; }
