#pragma once
#include <QtWidgets/QWidget>

class QTableWidget;
class QHeaderView;
class QPushButton;
class AliasTableWidget;

namespace SynGlyphX {
	class DataTransformModel;
}

class AliasesWidget : public QWidget
{
	Q_OBJECT

public:
	AliasesWidget(SynGlyphX::DataTransformModel* model, QWidget *parent = 0);
	~AliasesWidget();

	QSize sizeHint() const;
	void removeAlias();
	void Clear();
	void Refresh();
	QString customStyleSheet();
	QHeaderView* SelectableHeaderView();

	void dragEnterEvent(QDragEnterEvent *event) override;

public slots:
	void sectionClicked(int index);
	void OnCellChanged(int row, int column);

private:
	QString GenerateUniqueName(QString hint = QString());
	QPushButton* m_addAliasButton;
	AliasTableWidget* m_aliasesTableWidget;
	QHeaderView* m_headers;

	QAction* m_removeSelectedContextMenuAction;

	//std::map<QDataWidgetMapper*, unsigned int> m_dataWidgetMappersAndRows;
	SynGlyphX::DataTransformModel* m_model;
};
