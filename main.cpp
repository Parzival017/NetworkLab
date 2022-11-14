#include <QApplication>
#include "clientwidget.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ClientWidget cw;
    cw.show();
    return QApplication::exec();
}
