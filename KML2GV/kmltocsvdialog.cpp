#include "kmltocsvdialog.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QMessageBox>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopServices>
#include "application.h"
#include "geographicboundingbox.h"
#include <fstream>
#include <boost/tokenizer.hpp>
#include "downloadedmap.h"
#include "downloadexception.h"
#include <QtWidgets/QGroupBox>
#include "downloadoptionsdialog.h"
#include "ogrconvert.h"

KMLToCSVDialog::KMLToCSVDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout(this);

    m_inputKML = new SynGlyphX::FileLineEdit("KML & KMZ files (*.kmz *.kml)", this);
    formLayout->addRow(tr("KML/KMZ file:"), m_inputKML);

    m_inputGlyph = new SynGlyphX::FileLineEdit("CSV files (*.csv)", this);
    formLayout->addRow(tr("Glyph file:"), m_inputGlyph);

    m_outputDirectory = new SynGlyphX::DirectoryLineEdit(this);
    formLayout->addRow(tr("Output Directory:"), m_outputDirectory);

    verticalLayout->addLayout(formLayout);
    verticalLayout->addStretch();

    QGroupBox* mapOptionsGroupBox = new QGroupBox(tr("Map Options"), this);

    QHBoxLayout* mapOptionsLayout = new QHBoxLayout(this);

    m_mapquestRadioButton = new QRadioButton("MapQuest Open (OpenStreetMap)", mapOptionsGroupBox);
    m_googleRadioButton = new QRadioButton("Google Maps", mapOptionsGroupBox);
    QObject::connect(m_mapquestRadioButton, SIGNAL(clicked()), this, SLOT(OnMapSourceChanged()));
    QObject::connect(m_googleRadioButton, SIGNAL(clicked()), this, SLOT(OnMapSourceChanged()));

    m_imageSizeWidget = new SynGlyphX::SizeWidget(this);

    //Disabling size widget until image size limitations in our viewer are removed
    m_imageSizeWidget->setEnabled(false);

    QLabel* mapTypeLabel = new QLabel(tr("Type:"), mapOptionsGroupBox);
    m_mapTypeComboBox = new QComboBox(mapOptionsGroupBox);
    m_mapTypeComboBox->addItem(tr("Map"), NetworkDownloader::Map);
    m_mapTypeComboBox->addItem(tr("Satellite"), NetworkDownloader::Satellite);
    m_mapTypeComboBox->addItem(tr("Hybrid"), NetworkDownloader::Hybrid);

    mapOptionsLayout->addWidget(m_mapquestRadioButton);
    mapOptionsLayout->addWidget(m_googleRadioButton);
    mapOptionsLayout->addStretch(1);
    mapOptionsLayout->addWidget(m_imageSizeWidget);
    mapOptionsLayout->addWidget(mapTypeLabel);
    mapOptionsLayout->addWidget(m_mapTypeComboBox);

    mapOptionsGroupBox->setLayout(mapOptionsLayout);

    //Google Maps Download is not implemented yet so keep the option disabled
    m_googleRadioButton->setEnabled(false);

    verticalLayout->addWidget(mapOptionsGroupBox);
	
	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	m_optionsButton = new QPushButton(tr("Settings"), this);
	QObject::connect(m_optionsButton, SIGNAL(clicked()), this, SLOT(OnOptionsClicked()));
	buttonsLayout->addWidget(m_optionsButton);

	/*m_downloadMapButton = new QPushButton(tr("Download Map"), this);
	QObject::connect(m_downloadMapButton, SIGNAL(clicked()), this, SLOT(OnDownloadMapClicked()));
	buttonsLayout->addWidget(m_downloadMapButton);*/

    QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, this);

	buttonsLayout->addWidget(dialogButtonBox);

	verticalLayout->addLayout(buttonsLayout);

    setLayout(verticalLayout);

    QObject::connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));

    QObject::connect(m_inputKML, SIGNAL(TextChanged(QString)), this, SLOT(UpdateOutputDirectory(QString)));

    ReadSettings();

    setWindowTitle(tr("KML To CSV Converter"));
}

KMLToCSVDialog::~KMLToCSVDialog()
{

}

QString KMLToCSVDialog::GetKMLFilename() const {

    QFileInfo fileInfo(m_inputKML->GetText());
    return fileInfo.canonicalFilePath();
}

QString KMLToCSVDialog::GetCSVFilename() const {

    QFileInfo fileInfo(m_inputGlyph->GetText());
    return fileInfo.canonicalFilePath();
}

bool KMLToCSVDialog::ValidateInput() {

    QString kmlFilename = m_inputKML->GetText();
    QString glyphFilename = m_inputGlyph->GetText();
    QString outputDirectory = m_outputDirectory->GetText();

    if (kmlFilename.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file field is empty"));
        return false;
    }

    if (glyphFilename.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Glyph file field is empty"));
        return false;
    }

    if ((kmlFilename.right(4) != ".kml") && (kmlFilename.right(4) != ".kmz")) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file is not in either kml or kmz format"));
        return false;
    }

    if (glyphFilename.right(4) != ".csv") {
        QMessageBox::warning(this, tr("Error"), tr("CSV file is not in csv format"));
        return false;
    }

    if (!QFileInfo::exists(kmlFilename)) {
        QMessageBox::warning(this, tr("Error"), tr("KML/KMZ file does not exist"));
        return false;
    }

    if (!QFileInfo::exists(glyphFilename)) {
        QMessageBox::warning(this, tr("Error"), tr("CSV file does not exist"));
        return false;
    }

    QDir dir;
    if (!dir.mkpath(outputDirectory)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not create output directory"));
        return false;
    }

    return true;
}

bool KMLToCSVDialog::VerifyKeys() {

    NetworkDownloader& downloader = NetworkDownloader::Instance();
    NetworkDownloader::MapSource source = GetSource();
    if (source == NetworkDownloader::MapQuestOpen) {

        if (downloader.GetMapQuestOpenKey().isEmpty()) {
            
            QMessageBox messageBox(QMessageBox::Critical, tr("MapQuest Key Missing"), "MapQuest needs a key to download imagery.  To get a key, go to:<br/><A HREF=\"http://developer.mapquest.com/\">http://developer.mapquest.com/</A><br/><br/>You are responsible for obeying MapQuest licensing when downloading imagery.<br/><A HREF=\"http://developer.mapquest.com/web/tools/getting-started/terms-overview\">http://developer.mapquest.com/web/tools/getting-started/terms-overview</A>", QMessageBox::Ok);
            messageBox.setTextFormat(Qt::RichText);
            messageBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
            messageBox.exec();
            
            return false;
        }
        else {
            return true;
        }
    }
    else if (source == NetworkDownloader::GoogleMaps) {

        return false;
    }

    return false;
}

void KMLToCSVDialog::accept() {

    if (VerifyKeys() && ValidateInput()) {

        QFileInfo kmlFileInfo(GetKMLFilename());
        QString outputDirectory = m_outputDirectory->GetText() + QDir::separator();
        QString outputCSVFilename = outputDirectory + kmlFileInfo.baseName() + ".csv";

        if (QFile::exists(outputCSVFilename)) {
            if (QMessageBox::question(this, "Output Directory Already Exists", "There is already output in the output directory.  Do you wish to overwrite it? If not, then change the name of the output directory.") == QMessageBox::No) {
                return;
            }
        }

        SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);

        QString dataInFilename = QDir::toNativeSeparators(QDir::currentPath() + "/data_in.csv");

        try {
            
            QStringList args;

            //Cleanup - Do it at the beginning for the last run so that intermediate files can be investigated if part of the process failed
            QFile::remove(dataInFilename);
            QFile::remove(QDir::currentPath() + "/output.csv");
            QFile::remove(QDir::currentPath() + "/test.csv");
            QFile::remove(QDir::currentPath() + "/doc.csv");

            SynGlyphX::OGRConvert& ogrConvert = SynGlyphX::OGRConvert::Instance();
            ogrConvert.ConvertKMLToCSV(QDir::toNativeSeparators(kmlFileInfo.canonicalFilePath()).toStdString(), QDir::toNativeSeparators(dataInFilename).toStdString());

            std::vector<GeographicPoint> pointsFromCSV;
            if (!ReadPointsFromCSV(dataInFilename, pointsFromCSV)) {
                QMessageBox::critical(this, "Error", tr("Failed to read intermediate CSV so base map couldn't be downloaded"));
                return;
            }
            if (pointsFromCSV.empty()) {
                QMessageBox::critical(this, "Error", tr("No points found in intermediate CSV so base map couldn't be downloaded"));
                return;
            }

            QString mapfilename = outputDirectory + kmlFileInfo.baseName() + ".png";
            DownloadedMap map(pointsFromCSV, mapfilename.toStdString(), m_imageSizeWidget->GetSize(), GetSource(), static_cast<NetworkDownloader::MapType>(m_mapTypeComboBox->currentData().toInt()));

            args.clear();
            args.append("-G");
            args.append(GetCSVFilename());

            args.append("-c");
            const GeographicBoundingBox& imageBoundingBox = map.GetImageBoundingBox();
            args.append(QString::number(imageBoundingBox.GetSWCorner().get<1>()));
            args.append(QString::number(imageBoundingBox.GetSWCorner().get<0>()));
            args.append(QString::number(imageBoundingBox.GetNECorner().get<1>()));
            args.append(QString::number(imageBoundingBox.GetNECorner().get<0>()));

            args.append("-p");

            QString tagsFilename = outputDirectory + kmlFileInfo.baseName() + ".tags.csv";

            args.append("-T");
            args.append(tagsFilename);

            RunCommand(SynGlyphX::Application::applicationDirPath() + "/gps2csv.exe", args, outputCSVFilename);

            imageBoundingBox.WriteToKMLFile(outputDirectory.toStdString() + kmlFileInfo.baseName().toStdString() + "_image_bounding_box.kml");
            map.GetPointsBoundingBox().WriteToKMLFile(outputDirectory.toStdString() + kmlFileInfo.baseName().toStdString() + "_points_bounding_box.kml");

            args.clear();
            args.append(QDir::toNativeSeparators(QDir::currentPath() + "/ANTzTemplate/*"));
            args.append(outputDirectory);
#ifdef WIN32
            args.append("/E");
            args.append("/Y");
            args.append("/Q");
            RunCommand("xcopy", args);
#else
            args.append("-R";)
            RunCommand("cp", args);
#endif
            QString antzMapFilename = outputDirectory + "/usr/images/map00001.jpg";
            if (QFile::exists(antzMapFilename)) {
                QFile::remove(antzMapFilename);
            }
            QFile::copy(mapfilename, antzMapFilename);

            QString antzCSVFilename = outputDirectory + "/usr/csv/ANTz0001.csv";
            if (QFile::exists(antzCSVFilename)) {
                QFile::remove(antzCSVFilename);
            }
            QFile::copy(outputCSVFilename, antzCSVFilename);

            QString antzTagsFilename = outputDirectory + "/usr/csv/ANTzTag0001.csv";
            if (QFile::exists(antzTagsFilename)) {
                QFile::remove(antzTagsFilename);
            }
            QFile::copy(tagsFilename, antzTagsFilename);

            //Copy aux.xml file
            QString cornersXMLFilename = QDir::toNativeSeparators(QDir::currentPath() + "/corners.xml");
            QFile::copy(cornersXMLFilename, outputCSVFilename + ".aux.xml");
            QFile::remove(cornersXMLFilename);

            QDesktopServices::openUrl(QUrl("file:///" + outputDirectory));
            //QMessageBox::information(this, tr("Success"), tr("File successfully converted"));

        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Conversion Failed", e.what());
        }

        SynGlyphX::Application::restoreOverrideCursor();
    }
}

void KMLToCSVDialog::RunCommand(const QString& program, const QStringList& args, const QString& stdOutFile) {

    QProcess process;
    process.setProgram(program);
    process.setArguments(args);
    if (!stdOutFile.isEmpty()) {
        process.setStandardOutputFile(stdOutFile);
    }
    process.start();

    if (!process.waitForFinished()) {
        throw std::exception((program.toStdString() + " failed to finish").c_str());
    }

    if (process.exitStatus() != QProcess::NormalExit) {
        throw std::exception((program.toStdString() + " crashed and failed").c_str());
    }
}

void KMLToCSVDialog::ReadSettings() {

    QSettings settings;

    settings.beginGroup("Dialog");
    resize(settings.value("size", QSize(800, 120)).toSize());
    settings.endGroup();

    settings.beginGroup("params");
    m_inputGlyph->SetText(settings.value("glyph", QDir::currentPath() + "/pin.csv").toString());
    m_inputKML->SetInitalBrowseDirectory(settings.value("inputDir", "").toString());
    SetRadioButtonsFromMapSource(static_cast<NetworkDownloader::MapSource>(settings.value("MapSource", NetworkDownloader::Hybrid).toInt()));
    m_mapTypeComboBox->setCurrentIndex(settings.value("MapType").toInt());
    m_imageSizeWidget->SetSize(settings.value("MapSize", QSize(2048, 1024)).toSize());
    settings.endGroup();
}

void KMLToCSVDialog::WriteSettings() {

    QSettings settings;

    settings.beginGroup("Dialog");
    settings.setValue("size", size());
    settings.endGroup();

    settings.beginGroup("params");
    settings.setValue("glyph", m_inputGlyph->GetText());
    QFileInfo fileInfo(m_inputKML->GetText());
    settings.setValue("inputDir", fileInfo.canonicalPath());
    settings.setValue("MapSource", GetSource());
    settings.setValue("MapType", m_mapTypeComboBox->currentIndex());
    settings.setValue("MapSize", m_imageSizeWidget->GetSize());
    settings.endGroup();
}

NetworkDownloader::MapSource KMLToCSVDialog::GetSource() {

    if (m_googleRadioButton->isChecked()) {
        return NetworkDownloader::GoogleMaps;
    }
    else {
        return NetworkDownloader::MapQuestOpen;
    }
}

void KMLToCSVDialog::SetRadioButtonsFromMapSource(NetworkDownloader::MapSource source) {

    if (source == NetworkDownloader::GoogleMaps) {
        m_googleRadioButton->setChecked(true);
    }
    else {
        m_mapquestRadioButton->setChecked(true);
    }
    OnMapSourceChanged();
}

void KMLToCSVDialog::OnMapSourceChanged() {

    if (m_googleRadioButton->isChecked()) {
        m_imageSizeWidget->SetRange(1, 2048);
    }
    else {
        m_imageSizeWidget->SetRange(1, 3840);
    }
}

void KMLToCSVDialog::reject() {

    WriteSettings();
    QDialog::reject();
}

void KMLToCSVDialog::UpdateOutputDirectory(const QString& inputFile) {
    
    QFileInfo fileInfo(inputFile);
    m_outputDirectory->SetText(fileInfo.dir().canonicalPath() + "/" + fileInfo.baseName() + "_Converted");
}

//void KMLToCSVDialog::OnDownloadMapClicked() {
//	
//}

void KMLToCSVDialog::OnOptionsClicked() {

    DownloadOptionsDialog dialog(this);
    dialog.exec();
}

bool KMLToCSVDialog::ReadPointsFromCSV(const QString& csvfilename, std::vector<GeographicPoint>& points) {

	std::ifstream csvfile; 
	csvfile.open(csvfilename.toStdString().c_str());

	if (!csvfile.is_open()) {
		return false;
	}

	boost::char_separator<char> commaSeparator(",");
	
	std::string line;
	while (std::getline(csvfile, line)) {

		boost::tokenizer<boost::char_separator<char>> tokenizer(line, commaSeparator);
		boost::tokenizer<boost::char_separator<char>>::iterator iterator = tokenizer.begin();
		
		//There should be at least 3 tokens.  Otherwise skip the line
		if (iterator == tokenizer.end()) {
			continue;
		}
		std::string lonStr(*iterator);
		++iterator;

		if (iterator == tokenizer.end()) {
			continue;
		}
		std::string latStr(*iterator);
		++iterator;

		if (iterator != tokenizer.end()) {
			try {
				GeographicPoint point(boost::lexical_cast<double>(lonStr), boost::lexical_cast<double>(latStr));
				points.push_back(point);
			}
			catch (const boost::bad_lexical_cast &e) {
				//if there is a problem with the conversion than the line was bad so skip it
				continue;
			}
		}
	}

	csvfile.close();
	return true;
}