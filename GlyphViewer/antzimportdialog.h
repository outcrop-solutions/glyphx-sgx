#ifndef ANTZIMPORTDIALOG_H
#define ANTZIMPORTDIALOG_H

#include <QtWidgets/QDialog>
#include "browselineedit.h"
#include <QtWidgets/QCheckBox>

class ANTzImportDialog : public QDialog
{
	Q_OBJECT

public:
	ANTzImportDialog(QWidget *parent = 0);
	~ANTzImportDialog();

	QString GetANTzCSVFilename() const;
	QString GetANTzTagFilename() const;
	QString GetANTzChannelFilename() const;
	QString GetBaseImageFilename() const;
	QString GetDataTransformFilename() const;

private slots:
	void OnDefaultBaseImageCheckboxChanged(int newState);
	void OnCSVFileChanged(const QString& newText);

private:
	SynGlyphX::BrowseLineEdit* m_antzCSVFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzTagFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzChannelFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_baseImageFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_dataTransformFileLineEdit;
	QCheckBox* m_defaultBaseImageCheckBox;
};

#endif // ANTZIMPORTDIALOG_H
