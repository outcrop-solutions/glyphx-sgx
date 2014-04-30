#ifndef FILELINEEDIT_H
#define FILELINEEDIT_H

#include "sgxgui_global.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

namespace SynGlyphX {

    class SGXGUI_EXPORT FileLineEdit : public QWidget
    {
        Q_OBJECT

    public:
        FileLineEdit(const QString& filters, QWidget *parent = 0);
        ~FileLineEdit();

        void SetFilename(const QString& filename);
        QString GetFilename() const;

        private slots:
        void FileBrowserButtonActivated();

    private:
        QLineEdit* m_lineEdit;
        QPushButton* m_fileBrowserButton;
        QString m_filters;
    };

} //namespace SynGlyphX

#endif // FILELINEEDIT_H
