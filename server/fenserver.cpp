#include <fenserver.h>
#include <QtWidgets>
#include <QtNetwork>
#include <QMessageBox>

FenServer::FenServer()
{
    //Mise en page de la fenêtre
    serverStatus = new QLabel;
    clientNumber = new QLCDNumber;
    quit = new QPushButton(tr("Quitter"));
    connect(quit, SIGNAL(pressed()), qApp, SLOT(quit()));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(serverStatus);
    vbox->addWidget(clientNumber);
    vbox->addWidget(quit);

    setLayout(vbox);

    //Création du serveur
    server = new QTcpServer(this);
    if(!server->listen(QHostAddress::Any, 2048))
    {
        serverStatus->setText(tr("Le serveur n'a pas pu être démarré. Raison :<br />")+ server->errorString());
    }
    else
    {
        serverStatus->setText(tr("Le serveur a été démarré sur le port <strong>") + QString::number(server->serverPort()) + tr("</strong>.<br /> Des clients peuvent à présent se connecter. <br />Nombre de clients connectés :"));
        connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }

    messageSize = 0;



}

void FenServer::sendToAll(const QString &message)
{
    QByteArray paquet;//liste de tous les paquets à envoyer
    QDataStream out(&paquet, QIODevice::WriteOnly);//On va pouvoir utiliser <<

    out << (quint16) 0; //on réserve l'emplacement d'un quint16 (messageSize) on met un 0 à la place pour retrouver l'emplacement
    out << message;//On ajoute le message à la suite
    out.device()->seek(0);//On met le curseur sur le 0 du début
    out << (quint16)(paquet.size() - sizeof(quint16));//On écrase le 0 du début et on met la taille du message


    //Initialisation du fichier de sauvegarde :

    QFile history("history.txt");
    QDir::setCurrent("C:/Users/user/Documents/Programmes C++/nextchat-server/");

    if(!history.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this, "Sauvegarde", "Fichier de sauvegarde non trouvé");
        return;
    }

    QTextStream fluxHistory(&history);
    fluxHistory.setCodec("UTF-8");

    //Ecrire le message dans un fichier de sauvegarde

    QString historicMessage(message);

    historicMessage.remove("<strong>");
    historicMessage.remove("</strong>");
    historicMessage.remove("<em>");
    historicMessage.remove("</em>");

    fluxHistory << historicMessage << endl;
    fluxHistory.flush();
    history.close();

    //Envoie le message à tous les clients
    for(int i= 0; i < clients.size(); i++)
    {
        clients[i]->write(paquet);
    }
}

FenServer::~FenServer()
{

}

void FenServer::newConnection()
{
    sendToAll(tr("<em>Un nouveau client vient de se connecter !</em>"));
    QTcpSocket *newClient = server->nextPendingConnection();
    clients << newClient; // Rajouter un objet de type client dans notre liste

    clientNumber->display(clients.size());  //Mettre à  jour le nombre de client
    connect(newClient, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnection()));
}

void FenServer::disconnection()
{
    sendToAll(tr("<em>Un client vient de se déconnecter !</em>"));

    //On recherche l'origine du signal
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == nullptr)
        return;

    //On supprime alors le client
    clients.removeOne(socket);
    clientNumber->display(clients.size());
    socket->deleteLater(); //on supprime le signal après que le slot soit finis
}

void FenServer::dataReceived()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == nullptr)//Si jamais sender() ne trouve pas le client d'origine on arrête la méthode
        return;

    QDataStream in(socket);

    if(messageSize == 0) //Au début on aura pas la taille du message donc on va appeler cet fonction plusieurs fois
    {
        if(socket->bytesAvailable() < (int)sizeof(quint16))//On attend que le paquet soit composer de 16 bytes qui est la taille du message
            return;

        in >> messageSize;//Lorsqu'on a les 16 premiers bytes, il s'agit de la taille du message et on l'a récupère
    }

    if(socket->bytesAvailable() < messageSize) //On attend d'avoir reçu tout le message pour continuer la méthode
        return;

    QString message;
    in >> message;//on veut un QString donc on transfert le stream dans un QString

    sendToAll(message);//on envoie alors le paquer en entier
    messageSize = 0; //réinitialisation
}

