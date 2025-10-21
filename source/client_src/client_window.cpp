#include "client_window.h"
#include <QMessageBox>
#include <QVBoxLayout>

ClientWindow::ClientWindow(Queue<std::string>& recv_queue)
    : QWidget(nullptr), recv_queue(recv_queue)
{
    logArea = new QTextEdit(this);
    logArea->setReadOnly(true);
    readButton = new QPushButton("Read Message", this);

    auto *layout = new QVBoxLayout; // Leak de mem, ya veremos como lo resolvemos
    layout->addWidget(readButton);
    layout->addWidget(logArea);
    setLayout(layout);

    connect(readButton, &QPushButton::clicked, this, &ClientWindow::onReadClicked);

    setWindowTitle("Cliente - Protocolo");
}

void ClientWindow::onReadClicked() const {
    std::string message;
    if (recv_queue.try_pop(message)) {
        logArea->append(message.data());
    } else {
        logArea->append("No message recieved");
    }
}
