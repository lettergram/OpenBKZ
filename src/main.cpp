#include "openbkz.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenBKZ w;
    w.show();

    return a.exec();
}
