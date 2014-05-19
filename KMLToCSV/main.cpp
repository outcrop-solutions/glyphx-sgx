#include "kmltocsvdialog.h"
#include "application.h"

int main(int argc, char *argv[])
{
    SynGlyphX::Application::Setup("KMLToCSV", "1.0");
    SynGlyphX::Application a(argc, argv);

    SynGlyphX::Application::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));
    
    KMLToCSVDialog w;

    w.show();

    return a.exec();
}
