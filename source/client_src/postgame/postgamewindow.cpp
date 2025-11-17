#include "postgamewindow.h"

#include "ui_postgamewindow.h"

postgamewindow::postgamewindow(ClientProtocol& protocol,QWidget* parent)
    :protocol(protocol), QWidget(parent), ui(new Ui::postgamewindow) {
    ui->setupUi(this);
}

postgamewindow::~postgamewindow() { delete ui; }
