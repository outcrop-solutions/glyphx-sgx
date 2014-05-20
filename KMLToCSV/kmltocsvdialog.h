#ifndef KMLTOCSVDIALOG_H
#define KMLTOCSVDIALOG_H

#include <QtWidgets/QDialog>
#include "filelineedit.h"
#include "directorylineedit.h"
#include "geographicboundingbox.h" 
#include <vector>

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
	//void OnDownloadMapClicked();
	void OnOptionsClicked();

private:
    void ReadSettings();
    void WriteSettings();
    void RunCommand(const QString& program, const QStringList& args, const QString& stdOutFile);
    bool ValidateInput();

	bool ReadPointsFromCSV(const QString& csvfilename, std::vector<GeographicPoint>& points);

    SynGlyphX::FileLineEdit* m_inputKML;
    SynGlyphX::FileLineEdit* m_inputGlyph;
    SynGlyphX::DirectoryLineEdit* m_outputDirectory;
	QPushButton* m_optionsButton;
	QPushButton* m_downloadMapButton;
};

#endif // KMLTOCSVDIALOG_H
