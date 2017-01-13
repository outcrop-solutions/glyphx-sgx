#include "FrontEndFilterListWidget.h"
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QAbstractItemView>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include "groupboxsinglewidget.h"
#include "application.h"
#include "glyphviewerwindow.h"

FrontEndFilterListWidget::FrontEndFilterListWidget(QWidget *parent)
	: QWidget(parent)
{
	m_mainLayout = new QVBoxLayout(this);
	setLayout(m_mainLayout);
}

FrontEndFilterListWidget::~FrontEndFilterListWidget()
{
}

void FrontEndFilterListWidget::update(const MultiTableDistinctValueFilteringParameters& filters)
{
	for (auto w : findChildren<QWidget*>()) delete w;

	for (auto& f : filters)
	{
		auto& fe = f.second;
		auto ffilters = fe.GetDistinctValueFilters();
		for (auto& fef : ffilters)
		{
			qDebug() << fef.first;
			QVBoxLayout* buttonsLayout = new QVBoxLayout();
			QGroupBox* listframe = new QGroupBox(fef.first, this);
			listframe->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
			m_mainLayout->addWidget(listframe);
			auto& fefe = fef.second;
			for (auto& s : fefe)
				buttonsLayout->addWidget(new QLabel(s));
			listframe->setLayout(buttonsLayout);
		}
	}

	m_mainLayout->addStretch();
}
