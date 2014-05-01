#include "kmltocsvdialog.h"
#include "application.h"

int main(int argc, char *argv[])
{
    SynGlyphX::Application::Setup("KMLToCSV", "1.0");
    SynGlyphX::Application a(argc, argv);
    
    KMLToCSVDialog w;

    w.show();

    return a.exec();
}
