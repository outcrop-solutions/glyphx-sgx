#ifndef DATASOURCEFIELDTYPESDIALOG_H
#define DATASOURCEFIELDTYPESDIALOG_H

#include "sgxgui_global.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QTableWidget>

namespace SynGlyphX {

	class SGXGUI_EXPORT DatasourceFieldTypesDialog : public QDialog
	{
		Q_OBJECT

	public:
		DatasourceFieldTypesDialog(QStringList fields, const QString& fileFormat = "CSV", QWidget *parent = 0);
		~DatasourceFieldTypesDialog();

		QStringList GetFieldTypes() const;

	private:
		QTableWidget* m_fieldTypeTable;
		QList<QButtonGroup*> m_radioButtonGroups;
	};

} //namespace SynGlyphX

#endif // DATASOURCEFIELDTYPESDIALOG_H
