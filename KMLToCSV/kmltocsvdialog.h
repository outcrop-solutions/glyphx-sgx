#ifndef KMLTOCSVDIALOG_H
#define KMLTOCSVDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include "filelineedit.h"
#include "directorylineedit.h"
#include "geographicboundingbox.h" 
#include <vector>
#include "networkdownloader.h"
#include "sizewidget.h"

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
    void OnMapSourceChanged();

private:
    NetworkDownloader::MapSource GetSource();
    void SetRadioButtonsFromMapSource(NetworkDownloader::MapSource source);
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
    QRadioButton* m_mapquestRadioButton;
    QRadioButton* m_googleRadioButton;
    QComboBox* m_mapTypeComboBox;
    SynGlyphX::SizeWidget* m_imageSizeWidget;
};

#endif // KMLTOCSVDIALOG_H
