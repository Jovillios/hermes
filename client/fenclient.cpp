#include "fenclient.h"
#include "ui_fenclient.h"

FenClient::FenClient() :
    ui(new Ui::FenClient)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
        connect(socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
        connect(socket, SIGNAL(connected()), this, SLOT(connecte()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

        tailleMessage = 0;
}

FenClient::~FenClient()
{
    delete ui;
}

void FenClient::on_boutonConnexion_clicked() //se connecter
{
    //On annonce que la fenêtre est en train de se connecter
    ui->text->append(tr("<em>Tentative de connexion en cours...</em>"));
    ui->boutonConnexion->setEnabled(false);

    socket->abort();
    socket->connectToHost(ui->ipServeur->text(), ui->portServeur->value());//On se connecte avec les inputs
}

void FenClient::on_boutonEnvoyer_clicked()
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    //On prépare le paquet à envoyer
    QString messageAEnvoyer = tr("<strong>") + ui->pseudo->text() + tr("</strong> : ") + ui->message->text();

    out << (quint16) 0;
    out << messageAEnvoyer;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    socket->write(paquet); //On envoie le paquet

    ui->message->clear(); //On enlève le message du lineEdit
    ui->message->setFocus(); //On me le curseur à l'intéreur
}
//Appuyer sur la touche Entrée à le même effet qu'appuyer sur le bouton envoyer
void FenClient::on_message_returnPressed()
{
    on_boutonEnvoyer_clicked();
}

void FenClient::donneesRecues()
{
    /*
     * On récupère la taille de message pour savoir quand on a reçu le message en entier
     */

    QMessageBox::information(this, "Données Reçues", "Données");

    QDataStream in(socket);

    if(tailleMessage == 0)
    {
        if(socket->bytesAvailable() < (int)sizeof(quint16))
        return;

        in >> tailleMessage;
    }

    if(socket->bytesAvailable() < tailleMessage)
        return;

    //Si on arrive a cette ligne on a récupéré le message en entier
    QString messageRecu;
    in >> messageRecu;

    //On affiche le message dans la zone de texte

    ui->text->append(messageRecu);

    tailleMessage = 0; //On réinitialise
}

void FenClient::connecte()
{
    ui->text->append(tr("<em>Connexion réussie !</em>"));
    ui->boutonConnexion->setEnabled(true); //On réactive le bouton connexion
}

void FenClient::deconnecte()
{
    ui->text->append(tr("<em>Déconnecté du serveur</em>"));
}

void FenClient::erreurSocket(QAbstractSocket::SocketError erreur)
{
    switch(erreur)
    {
            case QAbstractSocket::HostNotFoundError:
                ui->text->append(tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
                break;
            case QAbstractSocket::ConnectionRefusedError:
                ui->text->append(tr("<em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.</em>"));
                break;
            case QAbstractSocket::RemoteHostClosedError:
                ui->text->append(tr("<em>ERREUR : le serveur a coupé la connexion.</em>"));
                break;
            default:
                ui->text->append(tr("<em>ERREUR : ") + socket->errorString() + tr("</em>"));
        }

        ui->boutonConnexion->setEnabled(true);
}


