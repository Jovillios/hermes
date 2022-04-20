#ifndef CLIENT_H
#define CLIENT_H

#include <QtWidgets>
#include <QtNetwork>

class Client
{
    Q_OBJECT
public:
    Client(QTcpSocket *paquet);
    void envoyerMessage(QByteArray message);
    ~Client();

private slots:
    void recevoirMessage();

private:
    QTcpSocket *m_paquet;
    QString *m_pseudo;
    QPixmap *m_icon;
};

#endif // CLIENT_H
