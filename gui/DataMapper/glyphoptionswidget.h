#pragma once
#ifndef GLYPHOPTIONSWIDGET_H
#define GLYPHOPTIONSWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>

class GlyphOptionsWidget : public QWidget
{
	Q_OBJECT

public:
	GlyphOptionsWidget(QWidget *parent);
	~GlyphOptionsWidget();

	bool GetMergeRoots() const;
	void SetMergeRoots(bool mergeRoots);

private:
	QCheckBox* m_mergeRootsCheckBox;
};

#endif // GLYPHOPTIONSWIDGET_H
