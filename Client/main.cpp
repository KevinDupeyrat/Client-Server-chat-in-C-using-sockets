#include <QApplication>
#include "fenclient.h"

int main(int argc, char* argv[]){

    QApplication app(argc, argv);

    fenClient fenetre;
    fenetre.show();

    return app.exec();
}
