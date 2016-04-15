#include "loadingscreenwidget.h"
#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>

QString LoadingScreenWidget::s_glyphEdDir;

LoadingScreenWidget::LoadingScreenWidget(GlyphViewerWindow* mainWindow, QWidget *parent)
	: QFrame(parent),
	m_mainWindow(mainWindow)
{
	setFrameShape(QFrame::Shape::Box);
	setFrameShadow(QFrame::Shadow::Sunken);
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* vizAndFilterLayout = new QHBoxLayout(this);

	QStringList visualizationNames;
	visualizationNames << "Global Admissions View" << "Admissions Officer View" << "High School View";
	m_viewListWidget = new QListWidget(this);
	m_viewListWidget->setSortingEnabled(false);
	m_viewListWidget->addItems(visualizationNames);
	QObject::connect(m_viewListWidget, &QListWidget::itemSelectionChanged, this, &LoadingScreenWidget::OnNewViewSelected);

	SynGlyphX::GroupBoxSingleWidget* visualizationGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("View(s)"), m_viewListWidget, this);
	vizAndFilterLayout->addWidget(visualizationGroupBox);

	m_filterWidgets.push_back(new QListWidget(this));

	m_filterGroupBoxWidgets.push_back(new SynGlyphX::GroupBoxSingleWidget(tr("Admissions Counselor"), m_filterWidgets[0], this));
	vizAndFilterLayout->addWidget(m_filterGroupBoxWidgets[0]);

	m_filterWidgets.push_back(new QListWidget(this));

	m_filterGroupBoxWidgets.push_back(new SynGlyphX::GroupBoxSingleWidget(tr("Year(s)"), m_filterWidgets[1], this));
	vizAndFilterLayout->addWidget(m_filterGroupBoxWidgets[1]);

	vizAndFilterLayout->addStretch(1);

	mainLayout->addLayout(vizAndFilterLayout, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);

	QPushButton* loadButton = new QPushButton(tr("Load Visualization"), this);
	buttonLayout->addWidget(loadButton, 1);
	QObject::connect(loadButton, &QPushButton::clicked, this, &LoadingScreenWidget::OnLoadVisualization);

	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);

	m_viewListWidget->selectionModel()->select(m_viewListWidget->model()->index(0, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect);

	m_displayNameToSDTPathMap.push_back(GetGlyphEdDir() + QDir::toNativeSeparators("/Global Admissions/View 1 Datamap.sdt"));
	m_displayNameToSDTPathMap.push_back(GetGlyphEdDir() + QDir::toNativeSeparators("/Admissions Officer/View 2 Datamap.sdt"));
	m_displayNameToSDTPathMap.push_back(GetGlyphEdDir() + QDir::toNativeSeparators("/High School/View 3 Datamap.sdt"));
}

LoadingScreenWidget::~LoadingScreenWidget()
{

}

bool LoadingScreenWidget::DoesGlyphEdDirExist() {

	return QDir(GetGlyphEdDir()).exists();
}

QString LoadingScreenWidget::GetGlyphEdDir() {

	if (s_glyphEdDir.isEmpty()) {

		s_glyphEdDir = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::applicationDirPath()) + QDir::separator() + "GlyphEd");
	}

	return s_glyphEdDir;
}

void LoadingScreenWidget::OnLoadVisualization() {

	if (m_filterWidgets[0]->selectionModel()->selectedIndexes().empty() || m_filterWidgets[1]->selectionModel()->selectedIndexes().empty()) {

		QMessageBox::information(this, tr("Did not load visualization"), tr("Visualization can not be loaded until at least one value has been selected from each filter."));
	}
	else {

		FilteringParameters filters;

		QString firstColumnName;
		QString secondColumnName;

		SynGlyphX::DataTransformMapping mapping;

		unsigned int selectedView = m_viewListWidget->selectionModel()->selectedIndexes().front().row();
		if (selectedView == 2) {

			firstColumnName = "HSNameUnique";
			secondColumnName = "Year";
		}
		else if (selectedView == 1) {

			firstColumnName = "StaffAssigned";
			secondColumnName = "Last_Decision_Cluster";

			QSet<QString> years;
			years.insert("2016");
			filters.SetDistinctValueFilter("Year", years);
		}
		else {

			firstColumnName = "StaffAssigned";
			secondColumnName = "Year";
		}

		mapping.ReadFromFile(m_displayNameToSDTPathMap[selectedView].toStdString());

		QSet<QString> firstFilterData;
		for (const auto& selectedItem : m_filterWidgets[0]->selectedItems()) {

			firstFilterData.insert(selectedItem->text());
		}
		filters.SetDistinctValueFilter(firstColumnName, firstFilterData);

		QSet<QString> secondFilterData;
		for (const auto& selectedItem : m_filterWidgets[1]->selectedItems()) {

			secondFilterData.insert(selectedItem->text());
		}
		filters.SetDistinctValueFilter(secondColumnName, secondFilterData);

		SynGlyphX::InputTable inputTable(mapping.GetDatasources().begin()->first, mapping.GetDatasources().begin()->second->GetTableNames().at(0));

		DataMappingLoadingFilterModel::Table2LoadingFiltersMap table2FiltersMap;
		table2FiltersMap[inputTable] = filters;

		m_mainWindow->LoadNewVisualization(m_displayNameToSDTPathMap[m_viewListWidget->selectionModel()->selectedIndexes().at(0).row()], table2FiltersMap);
	}
}

void LoadingScreenWidget::OnNewViewSelected() {

	m_filterWidgets[0]->selectionModel()->clearSelection();
	m_filterWidgets[1]->selectionModel()->clearSelection();

	m_filterWidgets[0]->clear();
	m_filterWidgets[1]->clear();

	unsigned int row = m_viewListWidget->selectionModel()->selectedIndexes().front().row();
	if (row == 2) {

		m_filterGroupBoxWidgets[0]->setTitle(tr("High School"));
		m_filterGroupBoxWidgets[1]->setTitle(tr("Years(s)"));

		m_filterWidgets[0]->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		m_filterWidgets[1]->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

		QStringList highSchools;
		highSchools << "Addison Trail High School - 140003"
			<< "Adlai E Stevenson High School - 143550"
			<< "Airforce Academy High School - 140698"
			<< "Al Raby HS for Comm & Env - 140662"
			<< "Alan B Shepard High School - 143356"
			<< "Amos Alonzo Stagg High School - 143357"
			<< "Amundsen High School - 140655"
			<< "Antioch Community High School - 140070"
			<< "Argo Community High School - 140090"
			<< "Austin Polytechnical Academy - 141318"
			<< "Barrington Community High Sch - 140205"
			<< "Bartlett High School - 140213"
			<< "Benet Academy - 142635"
			<< "Benito Juarez Community Acad - 140677"
			<< "Bloom High School - 141390"
			<< "Bloom-Trail High School - 141391"
			<< "Bogan High School - 141387"
			<< "Bolingbrook High School - 140367"
			<< "Brother Rice High School - 140682"
			<< "Buffalo Grove High School - 140447"
			<< "Carl Sandburg High School - 143325"
			<< "Carmel Catholic High School - 143096"
			<< "Carver Military Academy - 140840"
			<< "Catalyst-Maria High School - 140654"
			<< "Charles P. Steinmetz Prep HS - 141305"
			<< "Chicago Academy High School - 140661"
			<< "Chicago Bulls College Prep - 140693"
			<< "Chicago Christian High School - 140720"
			<< "Chicago HS Agricultural Sci - 140723"
			<< "Chicago Hope Academy - 141008"
			<< "Chicago Math & Science Academy - 140721"
			<< "Chicago Mil Acad Bronzeville - 140689"
			<< "Chicagoland Jewish High School - 143009"
			<< "Christ King Jesuit Coll Prep - 140641"
			<< "Christian Heritage Acdemy - 143211"
			<< "Cics Northtown Academy - 140691"
			<< "Community HS Dist 99 North HS - 141630"
			<< "Community High Sch District 94 - 144350"
			<< "Crete-Monee High School - 141505"
			<< "Cristo Rey Jesuit High School - 140668"
			<< "Curie Metropolitan High School - 141016"
			<< "De La Salle Institute - 140765"
			<< "De La Salle Institute West Cam - 140741"
			<< "DePaul College Prep - 140850"
			<< "Deerfield High School - 141570"
			<< "Devry Advantage Academy - 140562"
			<< "Drw Trading College Prep - 141104"
			<< "Dwight D Eisenhower High Sch - 140345"
			<< "East Leyden High School - 141960"
			<< "Edwin G Foreman High School - 140795"
			<< "Elk Grove High School - 141778"
			<< "Epic Academy Charter High Sch - 140111"
			<< "Eric Solorio Academy High Sch - 141042"
			<< "Evanston Township High School - 141860"
			<< "Evergreen Park Comm High Sch - 141865"
			<< "Fenton High School - 140290"
			<< "Fenwick High School - 143240"
			<< "Francis W Parker School - 140830"
			<< "Gary Comer College Prep - 141347"
			<< "George Washington High School - 140842"
			<< "George Westinghouse High Sch - 141001"
			<< "Glenbard East High School - 142662"
			<< "Glenbard North High School - 140534"
			<< "Glenbard South High School - 142073"
			<< "Glenbard West High School - 142075"
			<< "Glenbrook North High School - 143215"
			<< "Glenbrook South High School - 142078"
			<< "Golder Coll Prep-Cmps of Noble - 141143"
			<< "Grant Community High School - 141945"
			<< "Grayslake Central High School - 142135"
			<< "Grayslake North High School - 142126"
			<< "Gwendolyn Brooks Coll Prep Aca - 141297"
			<< "Hansberry College Prep - 141103"
			<< "Harold L Richards High School - 143228"
			<< "Highland Park High School - 142275"
			<< "Hillcrest High School - 141493"
			<< "Hinsdale Central High School - 142300"
			<< "Hinsdale South High School - 141428"
			<< "Hoffman Estates High School - 142301"
			<< "Holy Trinity High School - 140875"
			<< "Homewood-Flossmoor Comm HS - 142307"
			<< "Hubbard High School - 140877"
			<< "Hyde Park Academy - 140880"
			<< "Hyman G Rickover Academy - 141007"
			<< "IL Home Schoolers - 144598"
			<< "Ic Catholic Prep - 141800"
			<< "Illiana Christian High School - 142545"
			<< "J S Morton High School East - 141410"
			<< "J S Morton High School West - 140303"
			<< "James B Conant High School - 143343"
			<< "John F Kennedy High School - 140897"
			<< "John Hancock High School - 140899"
			<< "John Hersey High School - 140097"
			<< "Johnson College Prep - 140827"
			<< "Jones College Prep - 140905"
			<< "Kenwood Academy - 140917"
			<< "King College Prep - 140825"
			<< "Lake Forest Academy - 142525"
			<< "Lake Forest High School - 142520"
			<< "Lake Park Hs-District 108 - 142852"
			<< "Lake View High School - 140930"
			<< "Lake Zurich Senior High School - 142530"
			<< "Lakes Community High School - 142524"
			<< "Lane Technical High School - 140640"
			<< "Latin School of Chicago - 140935"
			<< "Legal Prep Charter Academy - 141332"
			<< "Lemont Township High School - 142590"
			<< "Libertyville High School - 142620"
			<< "Lincoln Park High School - 141100"
			<< "Lindblom Math and Science Acad - 141122"
			<< "Loyola Academy - 144403"
			<< "Lyons Township High School - 142483"
			<< "Maine Township High Sch East - 141595"
			<< "Maine Township High Sch South - 143377"
			<< "Maine Township High Sch West - 141597"
			<< "Maria High School - 141015"
			<< "Marian Catholic High School - 141393"
			<< "Marine Leadership Academy - 140704"
			<< "Marist High School - 141017"
			<< "Michele Clark Academic Prep HS - 141301"
			<< "Montini Catholic High School - 142663"
			<< "Morgan Park Academy - 141040"
			<< "Morgan Park High School - 141035"
			<< "Mother Mcauley High School - 141044"
			<< "Mount Carmel High School - 141045"
			<< "Muchin College Prep - 141083"
			<< "Mundelein High School - 143097"
			<< "Naperville Central High School - 143105"
			<< "Naperville North High School - 143106"
			<< "Nazareth Academy - 142485"
			<< "Neuqua Valley High School - 143108"
			<< "New Trier High School - 144430"
			<< "Niles North High School - 143927"
			<< "Niles Township West High Sch - 143926"
			<< "Noble Street College Prep - 141009"
			<< "North Grand High School - 141084"
			<< "North Lawndale Coll Prep Ch - 141057"
			<< "North Shore Country Day School - 144435"
			<< "Northridge Preparatory School - 143061"
			<< "Northside College Prep HS - 141342"
			<< "Notre Dame College Prep - 141063"
			<< "Oak Park & River Forest HS - 143245"
			<< "Ogden Intl Sch Chicago - 140748"
			<< "Our Lady of Tepeyac High Sch - 141132"
			<< "Palatine High School - 143345"
			<< "Perspectives Calumet HS Tech - 140944"
			<< "Perspectives Charter-Joslin - 141022"
			<< "Perspectives Leadership Acad - 140708"
			<< "Perspectives/Iit Math/Sci Acad - 141367"
			<< "Phoenix Military Academy - 141078"
			<< "Pritzker College Prep - 141239"
			<< "Prospect High School - 143062"
			<< "Prosser Career Acaemy - 141077"
			<< "Providence-St Mel High School - 141080"
			<< "Proviso West High School - 142286"
			<< "Queen of Peace High School - 143234"
			<< "Rauner College Prep - 140709"
			<< "Reavis High School - 143235"
			<< "Regina Dominican High School - 144407"
			<< "Resurrection College Prep High - 141090"
			<< "Rich Central High School - 143293"
			<< "Rich East High School - 143375"
			<< "Rich South High School - 143374"
			<< "Ridgewood High School - 141097"
			<< "Riverside-Brookfield High Sch - 143655"
			<< "Roger Sullivan High School - 141310"
			<< "Rolling Meadows High School - 143736"
			<< "Round Lake Senior High School - 143760"
			<< "Rowe-Clark Math Science Acad - 141108"
			<< "Roycemore School - 141850"
			<< "Saint Benedict High School - 141130"
			<< "Saint Francis High School - 144383"
			<< "Saint Ignatius College Prep - 141170"
			<< "Saint Joseph High School - 144348"
			<< "Saint Laurence High School - 143237"
			<< "Saint Patrick High School - 141230"
			<< "Saint Rita of Cascia High Sch - 141255"
			<< "Saint Viator High School - 140098"
			<< "Sarah E Goode Stem Academy - 140636"
			<< "Schaumburg High School - 143857"
			<< "Schurz Evening High School - 140695"
			<< "Schurz High School - 140700"
			<< "Senn High School - 141055"
			<< "Seton Academy - 143937"
			<< "Simeon Career Academy - 141380"
			<< "South High School District 99 - 141631"
			<< "South Shore International - 140699"
			<< "Southland Coll Prep Charter HS - 143634"
			<< "Stephen T Mather High School - 141307"
			<< "Thomas Kelly High School - 141315"
			<< "Thornridge High School - 141618"
			<< "Thornton Fractional North HS - 140475"
			<< "Thornton Fractional South HS - 142547"
			<< "Thornton Township High School - 142215"
			<< "Thornwood High School - 143939"
			<< "Tilden Career Community Acad - 141320"
			<< "Timothy Christian High School - 141415"
			<< "Tinley Park High School - 144118"
			<< "Trinity High School - 143650"
			<< "Univ IL-Chicago Coll Prep - 140658"
			<< "Univ of Chicago Laboratory HS - 141335"
			<< "Universal School - 140404"
			<< "University Chicago Charter Sch - 140869"
			<< "Urban Prep Academy Bronzeville - 140150"
			<< "Urban Prep Charter Acdmy-West - 141329"
			<< "Urban Prep Chrt Acad Young Men - 141006"
			<< "Vernon Hills High School - 144214"
			<< "Victor J Andrew High School - 144117"
			<< "Von Steuben Metro Science Ctr - 141345"
			<< "Walter Payton College Prep Sch - 141382"
			<< "Warren Township High School - 142175"
			<< "Wauconda High School - 144310"
			<< "Waukegan High School - 144322"
			<< "Wells Community Academy - 141370"
			<< "West Leyden High School - 143223"
			<< "Westmont High School - 144363"
			<< "Wheaton Academy - 144385"
			<< "Wheaton North High School - 144386"
			<< "Wheaton-Warrenville South HS - 144380"
			<< "Wheeling High School - 144387"
			<< "Whitney M Young Magnet HS - 141383"
			<< "William Fremd High School - 143347"
			<< "William H Taft High School - 141385"
			<< "Willowbrook High School - 144232"
			<< "Willows Academy - 142074"
			<< "Woodlands Academy Sacred Heart - 142510"
			<< "York Community High School - 141805"
			<< "Zion Benton Township High Sch - 144515";

		m_filterWidgets[0]->addItems(highSchools);
		m_filterWidgets[0]->sortItems();

		QStringList years;
		years << "2016" << "2015" << "2014" << "2013";
		m_filterWidgets[1]->addItems(years);

	}
	else if (row == 1) {

		m_filterGroupBoxWidgets[0]->setTitle(tr("Admissions Counselor"));
		m_filterGroupBoxWidgets[1]->setTitle(tr("Decision Status"));
		m_filterWidgets[0]->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		m_filterWidgets[1]->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

		QStringList admissionsOfficers;
		admissionsOfficers << "Alisa M. Fisher";

		m_filterWidgets[0]->addItems(admissionsOfficers);
		m_filterWidgets[0]->sortItems();

		QStringList decisionStatuses;
		decisionStatuses << "Admit"
			<< "Admit and Cancel"
			<< "Confirm Admit"
			<< "Pending Action"
			<< "Reject"
			<< "Waitlist"
			<< "Withdrawn";

		m_filterWidgets[1]->addItems(decisionStatuses);
		m_filterWidgets[1]->sortItems();
	}
	else {

		m_filterGroupBoxWidgets[0]->setTitle(tr("Admissions Counselor"));
		m_filterGroupBoxWidgets[1]->setTitle(tr("Years(s)"));
		m_filterWidgets[0]->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		m_filterWidgets[1]->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

		QStringList admissionsOfficers;
		admissionsOfficers << "Alisa M. Fisher"
			<< "Alyssa N. Moya"
			<< "Alyssa O'Rourke"
			<< "Anna Taul"
			<< "Brendan M. Corsones"
			<< "Eileen M. Zander"
			<< "Eileen Surprenant"
			<< "Erin K. Camilleri"
			<< "Gabriel H. Brown"
			<< "Jordan H. Schank"
			<< "Joyce Lantz"
			<< "Julie A. Moloney"
			<< "Kathryn Andersen"
			<< "LeShane O. Saddler"
			<< "Marisa S. Villano"
			<< "Mary Goodman"
			<< "Mary deVilliers"
			<< "Monica J. Hernandez"
			<< "R. Christopher Perkins"
			<< "Shannon C. Kelly";

		m_filterWidgets[0]->addItems(admissionsOfficers);
		m_filterWidgets[0]->sortItems();

		QStringList years;
		years << "2016" << "2015" << "2014" << "2013";
		m_filterWidgets[1]->addItems(years);
	}
}