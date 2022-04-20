#include <fenserveur.h>

FenServeur::FenServeur()
{
    //Creation et disposition des widgets
    setWindowTitle("Hermès - Serveur");

    QVBoxLayout *Vbox = new QVBoxLayout();
    setLayout(Vbox);
    etatServeur = new QLabel("etatServeur...");
    boutonQuitter = new QPushButton("Quitter");
    connect(boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));

    Vbox->addWidget(etatServeur);
    Vbox->addWidget(boutonQuitter);

    //Gestion du serveur

    serveur = new QTcpServer(this);
    if(!serveur->listen(QHostAddress::Any, 42067))
    {
    //Démarrage incorrecte
        etatServeur->setText(tr("Le serveur n'a pas pu être démarré. Raison:</br>") + serveur->errorString());
    }
    else
    {
       etatServeur->setText(tr("Le serveur a été démarré sur le port <strong>") +
       QString::number(serveur->serverPort()) + tr("</strong>.<br />Des clients peuvent maintenant se connecter"));

       connect(serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
    }

    tailleMessage = 0;
}

void FenServeur::envoyerATous(const QString &message)
{
    //préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0; //0 pour réserver la place pour écrire la taille
    out << message; //on ajoute le message à la suite
    out.device()->seek(0);//on se replace au début du packet;
    out << (quint16) (paquet.size() - sizeof(quint16)); //on écrase le 0 qu'on avait réservé

    //envoie le paquet à tous les clients
    for(int i=0; i < clients.size(); i++)
    {
        clients[i]->write(paquet);
    }
}

void FenServeur::nouvelleConnexion()
{
    envoyerATous(tr("<em>Un nouveau client vient de se connecter</em>"));

    QTcpSocket *nouveauClient = serveur->nextPendingConnection();
    clients << nouveauClient;

    connect(nouveauClient, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
}

void FenServeur::donneesRecues()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == 0)
    {
        //si on ne trouve aucun client
        return;
    }
    //sinon on récupère le message
    QDataStream in(socket);

    if(tailleMessage == 0)//si on ne connait pas la taille du message, on essaie de la récuperer
    {

        if(socket->bytesAvailable() < (int)sizeof(quint16))
        {
                //si on a pas recu la taille du message en entier
            return;
        }
    }

    in >> tailleMessage;//si on a recu la taille du message on le récupère

    //Si on connait la taille du message, on s'assure d'avoir le message en entier
    //sinon c'est que tout est reçu
    QString message;
    in >> message;

    //on envoie le message à tous les clients
    envoyerATous(message);

    //on remet la taile du message sur 0 pour les prochains messages
    tailleMessage = 0;
}

void FenServeur::deconnexionClient()
{
    envoyerATous(tr("<em>Un client vient de se déconnecter</em>"));

    //on détermine quel client se déconnecte
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == 0)
        return;

    clients.removeOne(socket);
    socket->deleteLater();
}

