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
        enum FileDialogType {
            FileOpen,
            FileSave,
            Directory
        };

        BrowseLineEdit(FileDialogType type, bool convertToNativeSeparators = true, QWidget *parent = 0);
        ~BrowseLineEdit();

        void SetFilters(const QString& filters);
        void SetInitalBrowseDirectory(const QString& dir);

        QString GetText() const;

    public slots:
        void SetText(const QString& text);

    signals:
        void TextChanged(QString text);

    private slots:
        void BrowseButtonActivated();
        void OnEditingFinished();

    private:
        FileDialogType m_dialogType;
        QLineEdit* m_lineEdit;
        QPushButton* m_browseButton;
        bool m_convertToNativeSeparators;
        QString m_filters;
        QString m_initialBrowseDirectory;
    };

} //namespace SynGlyphX

#endif // BROWSELINEEDIT_H
