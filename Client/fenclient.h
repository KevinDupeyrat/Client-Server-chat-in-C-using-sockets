#ifndef FENCLIENT_H
#define FENCLIENT_H

//#include <QtWidgets>

#include <QtNetwork>
#include <QStringListModel>
#include <QStringList>
#include "ui_fenclient.h"




class fenClient : public QWidget, private Ui::FenClient{
    Q_OBJECT


private:

    QStringList m_listeClient;
    QStringListModel *m_model;
    QTcpSocket *m_socket;
    quint16 m_tailleMessage;



private slots:

    void on_boutonDeconnexion_clicked();
    void on_boutonConnexion_clicked();
    void on_boutonEnvoyer_clicked();
    void on_message_returnPressed();
    void donneesRecues();
    void donneesEnvoyes(QString messageEnvoyer);
    void connecte();
    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError erreur);


public:
    fenClient();


};

#endif // FENCLIENT_H
