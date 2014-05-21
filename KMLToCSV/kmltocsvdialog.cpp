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
    m_mapquestRadioButton->setChecked(true);
    QLabel* mapTypeLabel = new QLabel(tr("Type:"), mapOptionsGroupBox);
    m_mapTypeComboBox = new QComboBox(mapOptionsGroupBox);
    m_mapTypeComboBox->addItem(tr("Map"), NetworkDownloader::Map);
    m_mapTypeComboBox->addItem(tr("Satellite"), NetworkDownloader::Satellite);
    m_mapTypeComboBox->addItem(tr("Hybrid"), NetworkDownloader::Hybrid);

    mapOptionsLayout->addWidget(m_mapquestRadioButton);
    mapOptionsLayout->addWidget(m_googleRadioButton);
    mapOptionsLayout->addStretch(1);
    mapOptionsLayout->addWidget(mapTypeLabel);
    mapOptionsLayout->addWidget(m_mapTypeComboBox);

    mapOptionsGroupBox->setLayout(mapOptionsLayout);

    //Google Maps Download is not implemented yet so keep the option disabled
    m_googleRadioButton->setEnabled(false);

    verticalLayout->addWidget(mapOptionsGroupBox);
	
	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	m_optionsButton = new QPushButton(tr("Options"), this);
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

void KMLToCSVDialog::accept() {

    if (ValidateInput()) {

        SynGlyphX::Application::setOverrideCursor(Qt::WaitCursor);

        QString tempKMLFilename;
        QString dataInFilename = QDir::toNativeSeparators(QDir::currentPath() + "/data_in.csv");

        try {

            QFileInfo kmlFileInfo(GetKMLFilename());
            QStringList args;

            QString outputDirectory = m_outputDirectory->GetText() + QDir::separator();

            if (kmlFileInfo.suffix() == "kml") {
                args.append("kml_parser.py");
                tempKMLFilename = QDir::toNativeSeparators(QDir::currentPath() + "/doc.kml");
            }
            else {
                args.append("kmz_parser.py");
                tempKMLFilename = QDir::toNativeSeparators(QDir::currentPath() + "/test.kmz");
            }
            QFile::copy(kmlFileInfo.canonicalFilePath(), tempKMLFilename);

            RunCommand("python.exe", args, dataInFilename);

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
            DownloadedMap map(pointsFromCSV, mapfilename.toStdString(), QSize(2048, 1024), GetSource(), static_cast<NetworkDownloader::MapType>(m_mapTypeComboBox->currentData().toInt()));

            args.clear();
            args.append("-k");
            args.append("-G");
            args.append(GetCSVFilename());

            args.append("-c");
            const GeographicBoundingBox& imageBoundingBox = map.GetImageBoundingBox();
            args.append(QString::number(imageBoundingBox.GetSWCorner().get<1>()));
            args.append(QString::number(imageBoundingBox.GetSWCorner().get<0>()));
            args.append(QString::number(imageBoundingBox.GetNECorner().get<1>()));
            args.append(QString::number(imageBoundingBox.GetNECorner().get<0>()));

            RunCommand(SynGlyphX::Application::applicationDirPath() + "/gps2csv.exe", args, outputDirectory + kmlFileInfo.baseName() + ".csv");

            //QFile::copy(QDir::currentPath() + "/corners.txt", outputDirectory + kmlFileInfo.baseName() + "_corners.txt");
            //QFile::copy(QDir::currentPath() + "/corners.kml", outputDirectory + kmlFileInfo.baseName() + "_corners.kml");

            imageBoundingBox.WriteToKMLFile(outputDirectory.toStdString() + kmlFileInfo.baseName().toStdString() + "_image_bounding_box.kml");
            map.GetPointsBoundingBox().WriteToKMLFile(outputDirectory.toStdString() + kmlFileInfo.baseName().toStdString() + "_points_bounding_box.kml");

            QDesktopServices::openUrl(QUrl("file:///" + outputDirectory));
            //QMessageBox::information(this, tr("Success"), tr("File successfully converted"));

        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Conversion Failed", e.what());
        }

        SynGlyphX::Application::restoreOverrideCursor();

        //Cleanup
        QFile::remove(tempKMLFilename);
        QFile::remove(dataInFilename);
        //QFile::remove(QDir::currentPath() + "/corners.txt");
        //QFile::remove(QDir::currentPath() + "/corners.kml");
        QFile::remove(QDir::currentPath() + "/output.csv");
        QFile::remove(QDir::currentPath() + "/test.csv");
    }
}

void KMLToCSVDialog::RunCommand(const QString& program, const QStringList& args, const QString& stdOutFile) {

    QProcess process;
    process.setProgram(program);
    process.setArguments(args);
    process.setStandardOutputFile(stdOutFile);
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
}

void KMLToCSVDialog::reject() {

    WriteSettings();
    QDialog::reject();
}

void KMLToCSVDialog::UpdateOutputDirectory(const QString& inputFile) {
    
    QFileInfo fileInfo(inputFile);
    m_outputDirectory->SetText(fileInfo.dir().canonicalPath() + "/Converted");
}

//void KMLToCSVDialog::OnDownloadMapClicked() {
//	
//}

void KMLToCSVDialog::OnOptionsClicked() {

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