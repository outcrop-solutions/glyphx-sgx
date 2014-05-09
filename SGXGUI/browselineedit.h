#ifndef BROWSELINEEDIT_H
#define BROWSELINEEDIT_H

#include "sgxgui_global.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

namespace SynGlyphX {

    class SGXGUI_EXPORT BrowseLineEdit : public QWidget
    {
        Q_OBJECT

    public:
        BrowseLineEdit(bool convertToNativeSeparators = true, QWidget *parent = 0);
        ~BrowseLineEdit();

        QString GetText() const;

    public slots:
        void SetText(const QString& text);

    signals:
        void TextChanged(QString text);

    protected slots:
        void BrowseButtonActivated();

    protected:
        virtual QString GetFromDialog() = 0;

        QLineEdit* m_lineEdit;
        QPushButton* m_browseButton;
        bool m_convertToNativeSeparators;
    };

} //namespace SynGlyphX

#endif // BROWSELINEEDIT_H
