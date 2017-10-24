#include <QMessageBox>
#include "ui_fenclient.h"
#include "fenclient.h"

fenClient::fenClient(){
    setupUi(this);

    // cration du socket
    m_socket = new QTcpSocket(this);


    // on connecte le socket avec les SLOTS personnels
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(m_socket, SIGNAL(connected()), this, SLOT(connecte()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

    // On désactive les boutons
    message->setEnabled(false);
    boutonDeconnexion->setEnabled(false);
    boutonEnvoyer->setEnabled(false);
    m_tailleMessage = 0;



}

void fenClient::on_boutonConnexion_clicked(){

    // Contrôle  de la saisie d'un pseudo
    if(pseudo->text().isEmpty()){

        QMessageBox::critical(this,"Erreur", "Vous devez avoir un Pseudo pour vous connecter");
        return;

    }else{

        // On indique la tentative de connexion
        listeMessages->append("<em>Tentative de connexion en cour ...</em>");

        // On désactive les connexion si il y en a d'autres
        m_socket->abort();

        // On effectue la connexion au serveur
        m_socket->connectToHost(serveurIP->text(), serveurPort->value());

    }

}

void fenClient::on_boutonDeconnexion_clicked(){

    // On désactive la connexion
    m_socket->abort();


}

void fenClient::on_boutonEnvoyer_clicked(){

    //donneesEnvoyes("<strong>" + m_client->getPseudo() + "</strong> : " + message->text());
    donneesEnvoyes(message->text());


}

void fenClient::on_message_returnPressed(){

    on_boutonEnvoyer_clicked();

}

void fenClient::donneesRecues(){


    // Flux d'entrée avec l'objet QDataStream
    QDataStream in(m_socket);

    while((m_tailleMessage == 0 && m_socket ->bytesAvailable() >= (int)sizeof(quint32)) || (m_tailleMessage != 0 && m_socket->bytesAvailable() >= m_tailleMessage)){
        if(m_tailleMessage == 0){
            in >> m_tailleMessage;
        }else{
            QString messageRecu;
            in >> messageRecu;

            // Si le message commence par //LISTECLI
            if(messageRecu.startsWith("//LISTECLI")){

                // On met à jour la liste des clients connecté au serveur
                // dans la ListView
                messageRecu.remove("//LISTECLI");
                m_listeClient = messageRecu.split(".");
                m_model = new QStringListModel(m_listeClient);
                listeClient->setModel(m_model);

            }else{

                // On affiche  le message sur la liste des messages
                listeMessages->append(messageRecu);

            }


            // On remet la taille du message à 0
            m_tailleMessage = 0;
        }
    }
}


void fenClient::donneesEnvoyes(QString messageEnvoyer){

    // Création du paquet avec l'objet QByteArray
    QByteArray paquet;
    // Création du flux d'envoie (WriteOnly)
    QDataStream out(&paquet, QIODevice::WriteOnly);



    // On crée le pacquet qui contient la taille du paquet
    // et le message
    out << (quint16) 0;
    out << messageEnvoyer;
    out.device()->seek(0);
    out <<(quint16)(paquet.size() - sizeof(quint16));

    // On envoie le paquet sur le réseau au serveur
    m_socket->write(paquet);
    // On efface la zone d'écriture du message
    message->clear();
    // On remet le curseur à l'interieur de la zone
    message->setFocus();

}

void fenClient::connecte(){

    // Si la connection a marché on l'indique
    listeMessages->append("<em>Connexion réussie !</em>");
    // On change les statue des boutons
    boutonEnvoyer->setEnabled(true);
    message->setEnabled(true);
    boutonDeconnexion->setEnabled(true);

    boutonConnexion->setEnabled(false);
    serveurIP->setEnabled(false);
    serveurPort->setEnabled(false);
    pseudo->setEnabled(false);


    // A la connection on envoie le pseudo du client au serveur
    // pour qu'il soit enregistrer
    donneesEnvoyes("//pseudo=" + pseudo->text());

}

void fenClient::deconnecte(){

    listeMessages->append("<em>Déconnecté du serveur</em>");
    pseudo->setEnabled(true);
    message->setEnabled(false);
    boutonDeconnexion->setEnabled(false);
    boutonEnvoyer->setEnabled(false);

    boutonConnexion->setEnabled(true);
    serveurIP->setEnabled(true);
    serveurPort->setEnabled(true);

    //for(int i = 0; i < listeClient->model()->rowCount(); i++)
        //listeClient->model()->removeRow(i);
    listeClient->model()->removeRows(0,listeClient->model()->rowCount());

}

void fenClient::erreurSocket(QAbstractSocket::SocketError erreur){

    switch(erreur){
        case QAbstractSocket::HostNotFoundError:
            listeMessages->append("<em>ERREUR : Le serveur n'a pas pu être trouvé. Vérifier l'IP et le port.</em>");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            listeMessages->append("<em>ERREUR : Le serveur a refusé la connexion. Vérifiez si le serveur a bien été lancé");
        break;
        case QAbstractSocket::RemoteHostClosedError:
            listeMessages->append("<em>ERREUR : Le serveur a coupé la connexion");
        break;
        default:
            listeMessages->append("<em>ERREUR : " + m_socket->errorString() + "</em>");
    }

    boutonConnexion->setEnabled(true);

}
