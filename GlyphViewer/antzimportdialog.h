#ifndef ANTZIMPORTDIALOG_H
#define ANTZIMPORTDIALOG_H

#include <QtWidgets/QDialog>
#include "browselineedit.h"
#include <QtWidgets/QCheckBox>
#include "csvfilereader.h"

class ANTzImportDialog : public QDialog
{
	Q_OBJECT

public:
	ANTzImportDialog(QWidget *parent = 0);
	~ANTzImportDialog();

	QString GetANTzNodeFilename() const;
	QString GetANTzTagFilename() const;
	QString GetANTzChannelFilename() const;
	QString GetANTzChannelMapFilename() const;
	QString GetBaseImageFilename() const;
	QString GetOutputFilename() const;

public slots:
	virtual void accept();

private slots:
	void OnDefaultBaseImageCheckboxChanged(int newState);
	void OnNodeFileChanged(const QString& newText);

private:
	bool ValidateANTzCSVFile(const QString& filename, const SynGlyphX::CSVFileReader::CSVValues& headers, const QString& fieldName, bool filenameCanBeEmpty = false);
	bool ValidateBaseImageFile(const QString& filename);
	bool ValidateDataTransformFile(const QString& filename);

	SynGlyphX::BrowseLineEdit* m_antzNodeFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzTagFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzChannelFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzChannelMapFileLineEdit;
	QCheckBox* m_defaultBaseImageCheckBox;
	SynGlyphX::BrowseLineEdit* m_baseImageFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_outputFileLineEdit;
};

#endif // ANTZIMPORTDIALOG_H
