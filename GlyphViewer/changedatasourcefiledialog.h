#ifndef CHANGEDATASOURCEFILEDIALOG_H
#define CHANGEDATASOURCEFILEDIALOG_H

#include <QtWidgets/QDialog>
#include "browselineedit.h"

class ChangeDatasourceFileDialog : public QDialog
{
	Q_OBJECT

public:
	ChangeDatasourceFileDialog(const QString& oldDatasourceFile, QWidget *parent = 0);
	~ChangeDatasourceFileDialog();

	QString GetNewDatasourceFile() const;

public slots:
	virtual void accept();

private:
	bool ValidateNewDatasourceFile() const;
	SynGlyphX::BrowseLineEdit* m_newDatasourceFileLineEdit;
};

#endif // CHANGEDATASOURCEFILEDIALOG_H
