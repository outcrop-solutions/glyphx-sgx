#ifndef FILELINEEDIT_H
#define FILELINEEDIT_H

#include "sgxgui_global.h"

#include "browselineedit.h"

namespace SynGlyphX {

    class SGXGUI_EXPORT FileLineEdit : public BrowseLineEdit
    {
        Q_OBJECT

    public:
        FileLineEdit(const QString& filters, QWidget *parent = 0);
        ~FileLineEdit();

        void SetInitalBrowseDirectory(const QString& dir);

    protected:
        virtual QString GetFromDialog();

    private:
        QString m_filters;
        QString m_initialBrowseDirectory;
    };

} //namespace SynGlyphX

#endif // FILELINEEDIT_H
