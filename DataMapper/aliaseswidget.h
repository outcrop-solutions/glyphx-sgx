#pragma once
#ifndef ALIASESWIDGET_H
#define ALIASESWIDGET_H

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QAction>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QDataWidgetMapper>
#include "glyphrolestablemodel.h"

namespace SynGlyphX {
	class DataTransformModel;
}

class AliasesWidget : public QWidget
{
	Q_OBJECT

public:
	AliasesWidget(GlyphRolesTableModel* model, QWidget *parent = 0);
	~AliasesWidget();

	QSize sizeHint() const;
	void removeAlias();
	void Clear();
	QString customStyleSheet();
	QHeaderView* SelectableHeaderView();

public slots:
	void sectionClicked(int index);

private:
	QPushButton* m_addAliasButton;
	QTableWidget* m_aliasesTableWidget;
	QHeaderView* m_headers;

	QAction* m_removeSelectedContextMenuAction;

	//std::map<QDataWidgetMapper*, unsigned int> m_dataWidgetMappersAndRows;
	GlyphRolesTableModel* m_model;
};

#endif //ALIASESWIDGET_H