#include <QApplication>
#include <fenserver.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    FenServer fenetre;

    fenetre.show();

    return app.exec();
}
