#pragma once
#include <QtWidgets/QWidget>

class QTableWidget;
class QHeaderView;
class QPushButton;
class QTableWidget;

namespace SynGlyphX {
	class DataTransformModel;
}

class AliasesWidget : public QWidget
{
	Q_OBJECT

public:
	AliasesWidget(QWidget *parent = 0);
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
	//GlyphRolesTableModel* m_model;
};
