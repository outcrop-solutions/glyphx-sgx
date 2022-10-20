#include "LoadingFilterDialog.h"
#include <QtWidgets/QDialogButtonBox>
#include "LoadingFilterWidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMessageBox>
#include <boost/uuid/uuid_io.hpp>

LoadingFilterDialog::LoadingFilterDialog(const QString& filename, QWidget *parent)
	: QDialog(parent),
	m_filename(filename)
{
	setWindowTitle(tr("Select Filters"));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	m_loadingFilterWidget = new LoadingFilterWidget(this);
	mainLayout->addWidget(m_loadingFilterWidget);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &LoadingFilterDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &LoadingFilterDialog::reject);

	setLayout(mainLayout);
}

LoadingFilterDialog::~LoadingFilterDialog()
{

}

void LoadingFilterDialog::SetupFilters(const SynGlyphX::DataTransformMapping& mapping) {

	//ge.initiate(m_dataEngineConnection->getEnv(), m_filename.toStdString(), "", "", "", "GlyphViewer");

	//m_loadingFilterWidget->SetFilters(ge, mapping);
}

MultiTableDistinctValueFilteringParameters LoadingFilterDialog::GetFilterValues() const {

	return m_loadingFilterWidget->GetFilterValues();
}

void LoadingFilterDialog::accept() {

	if (!m_loadingFilterWidget->AreSelectionsValid()) {

		QMessageBox::information(this, tr("Did not load visualization"), tr("Visualization can not be loaded until at least one value has been selected from all necessary filters."));
		return;
	}

	MultiTableDistinctValueFilteringParameters filteringParameters = m_loadingFilterWidget->GetFilterValues();
	for (const auto& filtersForTable : filteringParameters) {

		QString id = QString::fromStdWString(boost::uuids::to_wstring(filtersForTable.first.GetDatasourceID()));
		QString tableName = QString::fromStdWString(filtersForTable.first.GetTable());
		/*if (ge.SizeOfQuery(id, tableName, filtersForTable.second.GenerateQuery(filtersForTable.first)) == 0) {

			QMessageBox::information(this, tr("Did not load visualization"), tr("The selected combination of filters generate zero results.  Please try a different combination of filters."));
			return;
		}*/
	}

	QDialog::accept();
}