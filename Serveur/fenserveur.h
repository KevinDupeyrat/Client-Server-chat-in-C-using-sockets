#ifndef FENSERVEUR_H
#define FENSERVEUR_H

//#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QString>
#include <QtNetwork>
#include <QStringListModel>
#include <QStringList>
#include "client.h"

class FenServeur : public QWidget{
    Q_OBJECT

private:

    QLabel *m_etatServeur;
    QPushButton *m_boutonQuitter;

    QTcpServer *m_serveur;
    QList<Client *> m_listClient;
    quint16 m_tailleMessage;
    QString m_PseudoListClient;

private slots:

    void nouvelleConnexion();
    void donneesRecues();
    void deconnexionClient();
    QString miseAJourListCli();

public:

    FenServeur();
    void envoyerATous(const QString &message);



};


#endif // FENSERVEUR_H
