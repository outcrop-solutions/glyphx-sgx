#ifndef CHANGEDATASOURCEFILEDIALOG_H
#define CHANGEDATASOURCEFILEDIALOG_H

#include <QtWidgets/QDialog>
#include "browselineedit.h"
#include "datasource.h"

class ChangeDatasourceFileDialog : public QDialog
{
	Q_OBJECT

public:
	ChangeDatasourceFileDialog(const SynGlyphX::Datasource& oldDatasourceFile, const QString& acceptButtonText, QWidget *parent = 0);
	~ChangeDatasourceFileDialog();

	QString GetNewDatasourceFile() const;

public slots:
	virtual void accept();

private:
	bool ValidateNewDatasourceFile() const;
	SynGlyphX::BrowseLineEdit* m_newDatasourceFileLineEdit;
	QStringList m_oldDatasourceTables;
};

#endif // CHANGEDATASOURCEFILEDIALOG_H
