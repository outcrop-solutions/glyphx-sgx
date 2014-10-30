#ifndef GLYPHDEFAULTSWIDGET_H
#define GLYPHDEFAULTSWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include "datamappingdefaults.h"

class GlyphDefaultsWidget : public QTabWidget
{
	Q_OBJECT

public:
	GlyphDefaultsWidget(const SynGlyphX::DataMappingDefaults& defaults, QWidget *parent);
	~GlyphDefaultsWidget();

	SynGlyphX::DataMappingDefaults GetDefaults() const;

private:
	void CreateTagTab(const SynGlyphX::DataMappingDefaults& defaults);

	QComboBox* m_tagFieldDefaultComboBox;
	QLineEdit* m_tagValueDefaultLineEdit;
};

#endif // GLYPHDEFAULTSWIDGET_H
