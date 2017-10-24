#include <QtNetwork>
#include "client.h"

Client::Client(QTcpSocket *socket){

    m_socket = socket;

}

void Client::setPseudo(QString const p){
    m_pseudo = p;
}

QString Client::getPseudo() const{
    return m_pseudo;
}

QTcpSocket* Client::getSocket() const{
    return m_socket;
}
