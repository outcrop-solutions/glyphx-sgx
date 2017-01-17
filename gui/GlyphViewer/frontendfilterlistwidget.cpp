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
	setMinimumWidth(240);
}

FrontEndFilterListWidget::~FrontEndFilterListWidget()
{
}

void FrontEndFilterListWidget::update(const std::pair<MultiTableDistinctValueFilteringParameters, std::vector<std::wstring>>& filters)
{
	for (auto w : findChildren<QWidget*>()) delete w;

	auto add_panel = [this](const QString& title) -> QVBoxLayout* {
		auto collapse = new QPushButton(title);
		m_mainLayout->addWidget(collapse);
		QGroupBox* listframe = new QGroupBox(this);
		QVBoxLayout* buttonsLayout = new QVBoxLayout();
		listframe->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
		listframe->setLayout(buttonsLayout);
		m_mainLayout->addWidget(listframe);

		QObject::connect(collapse, &QPushButton::pressed, this, [listframe, collapse]() {
			if (listframe->height() != 0) listframe->setMaximumHeight(0);
			else listframe->setMaximumHeight(9999999);	// @todo
		});

		return buttonsLayout;
	};

	for (auto& f : filters.first)
	{
		auto& fe = f.second;
		auto ffilters = fe.GetDistinctValueFilters();
		for (auto& fef : ffilters)
		{
			auto layout = add_panel(fef.first);
			auto& fefe = fef.second;
			for (auto& s : fefe) layout->addWidget(new QLabel(s));
		}
	}

	for (auto& f : filters.second)
	{
		auto layout = add_panel(QString::fromStdWString(f));
		layout->addWidget(new QLabel("no selection"));
	}

	m_mainLayout->addStretch();
}
