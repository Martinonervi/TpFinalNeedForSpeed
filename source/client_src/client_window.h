#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QPushButton>
#include <QTextEdit>
#include "client_protocol.h"

class ClientWindow final: public QWidget {
    // Macro obligatoria en clases Qt que usan signals, slots o el sistema de meta-objetos de Qt.
    Q_OBJECT

public:
    explicit ClientWindow(Queue<std::string>& recv_queue);

private:
    QTextEdit *logArea;
    Queue<std::string>& recv_queue;
    QPushButton *readButton;
    void onReadClicked() const;


};

#endif // CLIENTWINDOW_H
