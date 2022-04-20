#include <QApplication>
#include <fenserveur.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    FenServeur fenêtre;
    fenêtre.show();

    return app.exec();
}
