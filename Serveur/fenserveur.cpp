#include <QVBoxLayout>
#include "fenserveur.h"

FenServeur::FenServeur(){

    // Création et disposition des widgets de le fenêtre
    m_etatServeur = new QLabel;
    m_boutonQuitter = new QPushButton("Quitter");

    QObject::connect(m_boutonQuitter, SIGNAL(clicked(bool)), qApp,SLOT(quit()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_etatServeur);
    layout->addWidget(m_boutonQuitter);
    setLayout(layout);


    setWindowTitle("TPChat - Serveur");

    // Gestion du serveur

    m_serveur = new QTcpServer(this);
    // Démarrage du serveur sur toute les IP
    // disponibles et sur le port 50885
    if(!m_serveur->listen(QHostAddress::Any, 50885)){
        // Si le serveur n'a pas été démarré correctement
        m_etatServeur->setText("Le serveur n'a pas pu etre demarre. Raison :<br />" + m_serveur->errorString());
    }else{
        // Si le serveur à était démarrer correctement
        m_etatServeur->setText("le serveur a ete demarre sur le port <strong>" + QString::number(m_serveur->serverPort()) + " </strong><br/>  Il y a <strong>" +  QString::number(m_listClient.size()) + "</strong> client(s) de connecte(s).");

        QObject::connect(m_serveur,SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
    }

    m_tailleMessage = 0;

}

void FenServeur::nouvelleConnexion(){

    QTcpSocket *nouveauClient = m_serveur->nextPendingConnection();
    m_listClient.append(new Client(nouveauClient));
    m_etatServeur->setText("le serveur a ete demarre sur le port <strong>" + QString::number(m_serveur->serverPort()) + " </strong><br/>  Il y a <strong>" +  QString::number(m_listClient.size()) + "</strong> client(s) de connecte(s).");

    QObject::connect(nouveauClient, SIGNAL(readyRead()),this, SLOT(donneesRecues()));
    QObject::connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));

}

void FenServeur::donneesRecues(){

    QString message;


    // Etape 1: Quand on reçoi un paquet ou sous-paquet, on
    // détermine d'abort qu'elle client envoie le message
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());

    // Si on n'arrive pas a identifier le client,
    // on arrête le méthode
    if(socket == 0)
        return;

    // Si on le trouve, on récupère le message
    QDataStream in(socket);


    // Si on ne connais pas la taille du message,
    // on essaie de la récupérer
    if(m_tailleMessage == 0){
        // Si on n'a pas reçu la taille du message complet
            if(socket->bytesAvailable() < (int)sizeof(quint16))
                return;

            // Si on a reçu la taille du message, on la récupère
            in >> m_tailleMessage;
    }

    // Si on connais la taille du message, on
    // vérifie si on a reçu tous les octet
    if(socket->bytesAvailable() < m_tailleMessage)
        return;


    // Si on a reçu tout le mesage on le récupère
    in >> message;
    int indCli;
    for(int i = 0; i < m_listClient.size(); i++){
        if(m_listClient[i]->getSocket() == socket)
            indCli = i;
    }

    if(message.startsWith("//pseudo=")){

        message.remove("//pseudo=");
        m_listClient[indCli]->setPseudo(message);
        message = "<em><font color=\"blue\">" + message + " vient de ce connecter</font></em>";

        // On envoie la liste des clients connecté à tous les clients
        envoyerATous(miseAJourListCli());

    }else{

        message = "<strong>" + m_listClient[indCli]->getPseudo() + "</strong> : " + message;

    }

    envoyerATous(message);
    // Etape 2 : On remet à zéro la taille du message
    m_tailleMessage = 0;

}

void FenServeur::deconnexionClient(){


    // On détermine quel client se déconnecte
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());

    if(socket == 0)
        return;

    // On supprime le pointeur dans le tableau du client
    // qui vient de ce déconnecté
    for(int i = 0; i < m_listClient.size(); i++){
       if(m_listClient[i]->getSocket() == socket){
           envoyerATous("<em><font color=\"red\">" + m_listClient[i]->getPseudo() + " vient de se deconnecter</font></em>");
           m_listClient.removeOne(m_listClient[i]);
       }
    }

    // On supprime la socket du client qui vient de ce déconnecter
    socket->deleteLater();
    m_etatServeur->setText("le serveur a ete demarre sur le port <strong>" + QString::number(m_serveur->serverPort()) + " </strong><br/>  Il y a <strong>" +  QString::number(m_listClient.size()) + "</strong> client(s) de connecte(s).");

    // On envoie la liste des clients connecté à tous les clients

    envoyerATous(miseAJourListCli());

}

void FenServeur::envoyerATous(const QString &message){

    // Préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    // La taille du paquet n'est pas la même que la taille du message
    // donc, nous mettons 0 au début du paquet pour réserver
    // la taille
    out << (quint16) 0;
    // On écrit en suite le message
    out << message;
    // On revient au 0
    out.device()->seek(0);
    // On y calcule la taille du paquet et on la met au
    // début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16));



    // On envoie le paquet à tous les clients connectés
    for(int i = 0; i < m_listClient.size(); i++)
        m_listClient[i]->getSocket()->write(paquet);
}

QString FenServeur::miseAJourListCli(){

    m_PseudoListClient = "";
    m_PseudoListClient.append("//LISTECLI");
    for(int i = 0; i < m_listClient.size(); i++){
        m_PseudoListClient.append(m_listClient[i]->getPseudo());
        m_PseudoListClient.append(".");
    }

    return m_PseudoListClient;

}
