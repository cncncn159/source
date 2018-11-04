#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w(0,new CBridge);
    w.show();
    //w.showFullScreen();
    return a.exec();
}
