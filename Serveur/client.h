#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>

class Client{


private:

    QTcpSocket *m_socket;
    QString m_pseudo;


public:

    Client(QTcpSocket *socket);
    void setPseudo(QString const p);
    QString getPseudo() const;
    QTcpSocket* getSocket() const;


};

#endif // CLIENT_H
