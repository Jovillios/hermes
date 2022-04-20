#ifndef FENCLIENT_H
#define FENCLIENT_H

#include <QtWidgets>
#include <QtNetwork>

namespace Ui {
class FenClient;
}

class FenClient : public QWidget
{
    Q_OBJECT

public:
    FenClient();
    ~FenClient();

private slots:
    void on_boutonConnexion_clicked();
    void on_boutonEnvoyer_clicked();
    void on_message_returnPressed();
    void donneesRecues();
    void connecte();
    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError erreur);

private:
    Ui::FenClient *ui;
    QTcpSocket *socket;
    quint16 tailleMessage;
};

#endif // FENCLIENT_H
