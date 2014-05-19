#ifndef KMLTOCSVDIALOG_H
#define KMLTOCSVDIALOG_H

#include <QtWidgets/QDialog>
#include "filelineedit.h"
#include "directorylineedit.h"

class KMLToCSVDialog : public QDialog
{
    Q_OBJECT

public:
    KMLToCSVDialog(QWidget *parent = 0);
    ~KMLToCSVDialog();

    QString GetKMLFilename() const;
    QString GetCSVFilename() const;

private slots:
    virtual void accept();
    virtual void reject();

    void UpdateOutputDirectory(const QString& inputFile);

private:
    void ReadSettings();
    void WriteSettings();
    bool RunCommand(const QString& program, const QStringList& args, const QString& stdOutFile);
    bool ValidateInput();

    SynGlyphX::FileLineEdit* m_inputKML;
    SynGlyphX::FileLineEdit* m_inputGlyph;
    SynGlyphX::DirectoryLineEdit* m_outputDirectory;
};

#endif // KMLTOCSVDIALOG_H
