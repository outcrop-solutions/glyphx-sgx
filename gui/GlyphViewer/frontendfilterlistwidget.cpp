#include "FrontEndFilterListWidget.h"
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QAbstractItemView>
#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include "groupboxsinglewidget.h"
#include "application.h"
#include "glyphviewerwindow.h"
#include "datatransformmapping.h"
#include "frontendfilter.h"

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

void FrontEndFilterListWidget::clear()
{
	QLayoutItem *child;
	while ((child = m_mainLayout->takeAt(0)) != 0) {
		delete child->widget();
		delete child;
	}
}

void FrontEndFilterListWidget::update(const char* filename, const MultiTableDistinctValueFilteringParameters& filters)
{
	SynGlyphX::DataTransformMapping mapping;
	mapping.ReadFromFile(filename);

	clear();

	auto add_panel = [this](const QString& title) -> QVBoxLayout* {
		auto collapse = new QPushButton(title);
		m_mainLayout->addWidget(collapse);
		QGroupBox* listframe = new QGroupBox(this);
		QVBoxLayout* buttonsLayout = new QVBoxLayout();
		listframe->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
		listframe->setLayout(buttonsLayout);
		m_mainLayout->addWidget(listframe);

		QObject::connect(collapse, &QPushButton::pressed, this, [listframe, collapse]() {
			int goal_height = 0;
			if (listframe->height() == 0) goal_height = 99999;
			listframe->setMaximumHeight(goal_height);
		});

		return buttonsLayout;
	};

	for (auto& table : filters)
	{
		auto fieldToAliasMap = mapping.GetFieldToAliasMapForTable(table.first);
		auto& filterparams = table.second;
		auto ffilters = filterparams.GetDistinctValueFilters();
		for (auto& fef : ffilters)
		{
			auto field = fef.first.toStdWString();
			auto title = fef.first;
			if (fieldToAliasMap.count(field) == 0) {

				std::string cleanedField = fef.first.toStdString();
				std::replace(cleanedField.begin(), cleanedField.end(), '_', ' ');
				title = QString::fromStdString(cleanedField);
			}
			else {

				title = QString::fromStdWString(fieldToAliasMap.at(field));
			}

			auto layout = add_panel(title);
			auto& fefe = fef.second;

			if (fefe.size() > 0)
				for (auto& s : fefe) layout->addWidget(new QLabel(s));
			else
				layout->addWidget(new QLabel("no selection"));

		}
	}

	m_mainLayout->addStretch();
}
