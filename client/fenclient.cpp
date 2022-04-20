#include "fenclient.h"
#include "ui_fenclient.h"

FenClient::FenClient() : ui(new Ui::FenClient)
{
    setupUi(this);

    message->setEnabled(false);
    logo->setPixmap(QPixmap("next.png"));

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSocket(QAbstractSocket::SocketError)));

    messageSize = 0;
}

FenClient::~FenClient()
{
    delete ui;
}

void FenClient::on_connectionButton_clicked()
{
    textBrowser->append(tr("<em>Tentative de connexion en cours...</em>"));
    connectionButton->setEnabled(false);

    socket->abort(); //on désactive les connexions précédentes
    socket->connectToHost(ipServer->text(), portServer->value());//On se connecte
}

void FenClient::on_sendButton_clicked()
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    //On prépare le paquet à envoyer
    QString messageToSend = tr("<strong>") + pseudo->text() + tr("</strong> : ") + message->text();

    out << (quint16) 0;
    out << messageToSend;
    out.device()->seek(0);
    out << (quint16) (sizeof(messageToSend));

    socket->write(paquet);

    message->clear();
    message->setFocus();
}

void FenClient::on_message_returnPressed()
{
    on_sendButton_clicked();
}

void FenClient::dataReceived()
{
    //Même principe que lorsque le serveur reçoit le paquat

    QDataStream in(socket);

    if(messageSize == 0)
    {
        if(socket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> messageSize;
    }

    if(socket->bytesAvailable() < messageSize)
        return;

    QString messageReceived;
    in >> messageReceived;

    textBrowser->append(messageReceived);

    messageSize = 0;
}

void FenClient::connected()
{
    textBrowser->append(tr("<em>Connexion réussie !</em>"));
    message->setEnabled(true);
}

void FenClient::disconnected()
{
    textBrowser->append(tr("<em>Vous êtes déconnecté du serveur !</em>"));
}

// Ce slot est appelé lorsqu'il y a une erreur
void FenClient::errorSocket(QAbstractSocket::SocketError erreur)
{
    switch(erreur) // On affiche un message différent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            textBrowser->append(tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            textBrowser->append(tr("<em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            textBrowser->append(tr("<em>ERREUR : le serveur a coupé la connexion.</em>"));
            break;
        default:
            textBrowser->append(tr("<em>ERREUR : ") + socket->errorString() + tr("</em>"));
    }

    connectionButton->setEnabled(true);
}


