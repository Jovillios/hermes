#include <client.h>
#include <fenserver.h>


Client::Client(QTcpSocket *paquet) : m_paquet(paquet)
{
    connect(m_paquet, SIGNAL(readyRead()), this, SLOT(recevoirMessage()));
}

void Client::envoyerMessage(QByteArray message)
{
    m_paquet->write(message);
}

Client::~Client()
{

}

void Client::recevoirMessage()
{

}
