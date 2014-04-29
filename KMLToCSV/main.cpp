#include "kmltocsvdialog.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KMLToCSVDialog w;
    w.resize(800, 100);

    if (w.exec() == QDialog::Accepted) {
        
    }

    return 0;
}
