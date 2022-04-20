#ifndef FENSERVER_H
#define FENSERVER_H

#include <QtWidgets>
#include <QtNetwork>
#include <QMessageBox>

class FenServer : public QWidget
{
    Q_OBJECT

    public:
    FenServer();
    void sendToAll(const QString &message);
    ~FenServer();

    private slots:
    void newConnection();
    void disconnection();
    void dataReceived();

    private:
    QLabel *serverStatus;
    QLCDNumber *clientNumber;
    QPushButton *quit;

    QTcpServer *server;
    QList<QTcpSocket *> clients;
    quint16 messageSize;
};

#endif // FENSERVER_H
