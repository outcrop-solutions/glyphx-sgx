#include "ElasticListSetupDialog.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QCoreApplication>

namespace SynGlyphX {

	ElasticListSetupDialog::ElasticListSetupDialog(QWidget *parent)
		: QDialog(parent)
	{
		setWindowTitle("Elastic List Setup");
		setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		m_tableTabs = new QTabWidget(this);
		
		mainLayout->addWidget(m_tableTabs);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		mainLayout->addWidget(buttonBox);

		setLayout(mainLayout);

		QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	}

	ElasticListSetupDialog::~ElasticListSetupDialog()
	{
	}

	void ElasticListSetupDialog::PopulateTabs(std::map<std::wstring, std::vector<std::wstring>> elastic, std::map<std::wstring, std::vector<std::wstring>> available){

		for (const auto& table : available){
			ElasticListSetupTab* tab = new ElasticListSetupTab(table.first);
			std::vector<std::wstring> availableFields;
			if (elastic.find(table.first) == elastic.end()){
				std::vector<std::wstring> empty;
				elastic[table.first] = empty;
			}
			for (const auto& field : table.second){
				if (std::find(elastic[table.first].begin(), elastic[table.first].end(), field) == elastic[table.first].end()){
					availableFields.push_back(field);
				}
			}
			
			tab->PopulateAvailableFields(availableFields);
			tab->PopulateElasticList(elastic[table.first]);
			m_elasticTabs[table.first] = tab;

			m_tableTabs->addTab(tab, QString::fromStdWString(m_formattedNames[table.first]));
		}
	}

	std::map<std::wstring, std::vector<std::wstring>> ElasticListSetupDialog::SaveElasticListSelections(){
		
		std::map<std::wstring, std::vector<std::wstring>> toReturn;
		for (const auto& tab : m_elasticTabs){
			toReturn[tab.first] = tab.second->GetElasticList();
		}
		return toReturn;
	}

} //namespace SynGlyphX
