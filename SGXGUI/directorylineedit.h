#ifndef DIRECTORYLINEEDIT_H
#define DIRECTORYLINEEDIT_H

#include "sgxgui_global.h"
#include "browselineedit.h"

namespace SynGlyphX {

    class SGXGUI_EXPORT DirectoryLineEdit : public BrowseLineEdit
    {
        Q_OBJECT

    public:
        DirectoryLineEdit(QWidget *parent = 0);
        ~DirectoryLineEdit();

    protected:
        virtual QString GetFromDialog();

    };

} //namespace SynGlyphX

#endif // DIRECTORYLINEEDIT_H
