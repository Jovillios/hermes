#include <QApplication>
#include <fenclient.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    FenClient fenetre;
    fenetre.show();

    return app.exec();
}
