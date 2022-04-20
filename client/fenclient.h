#ifndef FENCLIENT_H
#define FENCLIENT_H

#include <QtWidgets>
#include <QtNetwork>
#include <ui_fenclient.h>

namespace Ui {
class FenClient;
}

class FenClient : public QWidget, private  Ui::FenClient
{
    Q_OBJECT

public:
    FenClient();
    ~FenClient();

private slots:
    void on_connectionButton_clicked();
    void on_sendButton_clicked();
    void on_message_returnPressed();
    void dataReceived();
    void connected();
    void disconnected();
    void errorSocket(QAbstractSocket::SocketError error);

private:
    Ui::FenClient *ui;

    QTcpSocket *socket;
    quint16 messageSize;
};

#endif // FENCLIENT_H
