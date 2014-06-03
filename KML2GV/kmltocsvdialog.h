#ifndef KMLTOCSVDIALOG_H
#define KMLTOCSVDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include "browselineedit.h"
#include "geographicboundingbox.h" 
#include <vector>
#include "networkdownloader.h"
#include "mapoptionswidget.h"

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
	void OnOptionsClicked();

private:
    void ReadSettings();
    void WriteSettings();
    void RunCommand(const QString& program, const QStringList& args, const QString& stdOutFile = "");
    bool ValidateInput();
    bool VerifyKeys();

	bool ReadPointsFromCSV(const QString& csvfilename, std::vector<GeographicPoint>& points);

    SynGlyphX::BrowseLineEdit* m_inputKML;
    SynGlyphX::BrowseLineEdit* m_inputGlyph;
    SynGlyphX::BrowseLineEdit* m_outputDirectory;
	QPushButton* m_optionsButton;
	QPushButton* m_downloadMapButton;
    MapOptionsWidget* m_mapOptionsWidget;
};

#endif // KMLTOCSVDIALOG_H
