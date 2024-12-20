#include "pseudotimefilterwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtCore/QSettings>
#include <QtWidgets/QTreeView>
#include "singlewidgetdialog.h"
#include <boost/uuid/uuid_io.hpp>
#include "application.h"

unsigned int PseudoTimeFilterWidget::s_buttonSize = 24;

PseudoTimeFilterWidget::PseudoTimeFilterWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
	: QWidget(parent),
	m_columnsModel(columnsModel),
    m_filterState(FilterState::Inactive),
    m_filteringManager(filteringManager)
{
	m_playTimer.setSingleShot(false);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* sliderValueLayout = new QHBoxLayout();

	m_slider = new QSlider(Qt::Horizontal, this);
	m_slider->setMinimum(0);
	QObject::connect(m_slider, &QSlider::valueChanged, this, &PseudoTimeFilterWidget::OnSliderValueChanged);
	sliderValueLayout->addWidget(m_slider, 1);
	m_currentPositionLabel = new QLineEdit(this);
	m_currentPositionLabel->setReadOnly(true);
	sliderValueLayout->addWidget(m_currentPositionLabel);

	mainLayout->addLayout(sliderValueLayout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();

	QHBoxLayout* buttonsLayoutLeft = new QHBoxLayout();
	buttonsLayoutLeft->setContentsMargins(0, 0, 0, 0);

	m_fieldSelectorButton = new QPushButton(tr("Select Field"), this);
	QObject::connect(m_fieldSelectorButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnFilterSelectionButtonClicked);

	buttonsLayoutLeft->addWidget(m_fieldSelectorButton);

	m_fieldLabel = new QLabel(this);
	buttonsLayoutLeft->addWidget(m_fieldLabel);

	buttonsLayoutLeft->addStretch(1);

	m_repeatButton = new QPushButton(this);
	m_repeatButton->setIcon(QIcon(":SGXGUI/Resources/Video/repeat.png"));
	m_repeatButton->setIconSize(SynGlyphX::Application::DynamicQSize(s_buttonSize, s_buttonSize));
	m_repeatButton->setFixedSize(SynGlyphX::Application::DynamicQSize(s_buttonSize, s_buttonSize));
	m_repeatButton->setCheckable(true);
	m_repeatButton->setToolTip(tr("Repeat"));
	buttonsLayoutLeft->addWidget(m_repeatButton);

	m_stopButton = new QPushButton(this);
	m_stopButton->setIcon(QIcon(":SGXGUI/Resources/Video/stop.png"));
	m_stopButton->setIconSize(SynGlyphX::Application::DynamicQSize(s_buttonSize, s_buttonSize));
	m_stopButton->setFixedSize(SynGlyphX::Application::DynamicQSize(s_buttonSize, s_buttonSize));
	m_stopButton->setToolTip(tr("Stop"));
	QObject::connect(m_stopButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnStop);
	buttonsLayoutLeft->addWidget(m_stopButton);

	m_goToStartButton = new QPushButton(this);
	m_goToStartButton->setIcon(QIcon(":SGXGUI/Resources/Video/start.png"));
	m_goToStartButton->setIconSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goToStartButton->setFixedSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goToStartButton->setToolTip(tr("Go To Start"));
	QObject::connect(m_goToStartButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnGoToStart);
	buttonsLayoutLeft->addWidget(m_goToStartButton);
	
	m_goBackwardButton = new QPushButton(this);
	m_goBackwardButton->setIcon(QIcon(":SGXGUI/Resources/Video/start.png"));
	m_goBackwardButton->setIconSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goBackwardButton->setFixedSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goBackwardButton->setToolTip(tr("Increment Backwards"));
	QObject::connect(m_goBackwardButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnDecrementTimeFilter);
	buttonsLayoutLeft->addWidget(m_goBackwardButton);

	buttonsLayout->addLayout(buttonsLayoutLeft, 1);

	QHBoxLayout* buttonsLayoutCenter = new QHBoxLayout();
	buttonsLayoutCenter->setContentsMargins(0, 0, 0, 0);
	buttonsLayoutCenter->setSizeConstraint(QLayout::SetFixedSize);

	m_playPauseButton = new QPushButton(this);
	m_playPauseButton->setFixedSize(SynGlyphX::Application::DynamicQSize(2 * s_buttonSize, 2 * s_buttonSize));
	m_playPauseButton->setIconSize(SynGlyphX::Application::DynamicQSize(2 * s_buttonSize, 2 * s_buttonSize));
	QObject::connect(m_playPauseButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnPlayPause);
	buttonsLayoutCenter->addWidget(m_playPauseButton);

	buttonsLayout->addLayout(buttonsLayoutCenter);

	QHBoxLayout* buttonsLayoutRight = new QHBoxLayout();
	buttonsLayoutRight->setContentsMargins(0, 0, 0, 0);

	m_goForwardButton = new QPushButton(this);
	m_goForwardButton->setIcon(QIcon(":SGXGUI/Resources/Video/end.png"));
	m_goForwardButton->setIconSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goForwardButton->setFixedSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goForwardButton->setToolTip(tr("Increment Forwards"));
	QObject::connect(m_goForwardButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnIncrementTimeFilter);
	buttonsLayoutRight->addWidget(m_goForwardButton);

	m_goToEndButton = new QPushButton(this);
	m_goToEndButton->setIcon(QIcon(":SGXGUI/Resources/Video/end.png"));
	m_goToEndButton->setIconSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goToEndButton->setFixedSize(SynGlyphX::Application::DynamicQSize(1.5 * s_buttonSize, s_buttonSize));
	m_goToEndButton->setToolTip(tr("Go To End"));
	QObject::connect(m_goToEndButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnGoToEnd);
	buttonsLayoutRight->addWidget(m_goToEndButton);

	buttonsLayoutRight->addStretch(1);

	m_moveCameraOnUpdateCheckbox = new QCheckBox(tr("Move Camera"), this);
	buttonsLayoutRight->addWidget(m_moveCameraOnUpdateCheckbox);

	m_filterViewCheckbox = new QCheckBox(tr("Hide Filtered"), this);
	buttonsLayoutRight->addWidget(m_filterViewCheckbox);

	buttonsLayout->addLayout(buttonsLayoutRight, 1);

	mainLayout->addLayout(buttonsLayout);

	setLayout(mainLayout);

	SetPlayTimerInterval(1000);
	QObject::connect(&m_playTimer, &QTimer::timeout, this, &PseudoTimeFilterWidget::IncrementTime);

	ReadSettings();
	Disable();
}

PseudoTimeFilterWidget::~PseudoTimeFilterWidget()
{
	WriteSettings();
}

void PseudoTimeFilterWidget::ResetForNewVisualization() {

	EnableButtons(true);
	m_slider->setMaximum(20);

	m_filterState = FilterState::Inactive;
	m_filteringManager->EnableTimeFilter(false);
	UpdateSelectedField(m_columnsModel->index(0, 0, m_columnsModel->index(0, 0, m_columnsModel->index(0, 0))));
	m_playPauseButton->setToolTip(tr("Play"));
	m_playPauseButton->setIcon(QIcon(":SGXGUI/Resources/Video/play.png"));
	m_playTimer.stop();
	ResetSliderAndLabel();
}

void PseudoTimeFilterWidget::Disable() {

	EnableButtons(false);
	ChangeFilterState(FilterState::Inactive);
	m_filteringManager->EnableTimeFilter(false);
}

void PseudoTimeFilterWidget::SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgetsManager) {

	linkedWidgetsManager.AddFilterViewCheckbox(m_filterViewCheckbox);
}

void PseudoTimeFilterWidget::ReadSettings() {

	QSettings settings;
	settings.beginGroup("PseudoTimeFilter");
	m_repeatButton->setChecked(settings.value("repeat", false).toBool());
	m_moveCameraOnUpdateCheckbox->setChecked(settings.value("moveCamera", false).toBool());
	settings.endGroup();
}

void PseudoTimeFilterWidget::WriteSettings() {

	QSettings settings;
	settings.beginGroup("PseudoTimeFilter");
	settings.setValue("repeat", m_repeatButton->isChecked());
	settings.setValue("moveCamera", m_moveCameraOnUpdateCheckbox->isChecked());
	settings.endGroup();
}

void PseudoTimeFilterWidget::EnableButtons(bool enable) {

	m_slider->setEnabled(enable);
	m_currentPositionLabel->setEnabled(enable);
	m_stopButton->setEnabled(enable);
	m_goToStartButton->setEnabled(enable);
	m_goBackwardButton->setEnabled(enable);
	m_playPauseButton->setEnabled(enable);
	m_goForwardButton->setEnabled(enable);
	m_goToEndButton->setEnabled(enable);
	m_fieldSelectorButton->setEnabled(enable);
}

void PseudoTimeFilterWidget::OnPlayPause() {

	if (m_filterState == FilterState::ActiveRunning) {

		ChangeFilterState(FilterState::ActivePaused);
	}
	else {

		ChangeFilterState(FilterState::ActiveRunning);
	}
}

void PseudoTimeFilterWidget::OnStop() {

	ChangeFilterState(FilterState::Inactive);
}

void PseudoTimeFilterWidget::OnGoToStart() {

	if (m_filterState == FilterState::Inactive) {

		ChangeFilterState(FilterState::ActivePaused);
	}
	m_slider->setValue(0);
}

void PseudoTimeFilterWidget::OnGoToEnd() {

	if (m_filterState == FilterState::Inactive) {

		ChangeFilterState(FilterState::ActivePaused);
	}
	m_slider->setValue(m_slider->maximum());
}

void PseudoTimeFilterWidget::OnSliderValueChanged(int index) {

	if (m_filterState == FilterState::Inactive) {

		ChangeFilterState(FilterState::ActivePaused);
	}
	
	UpdateTimeFilter();
}

void PseudoTimeFilterWidget::UpdateTimeFilter() {

	int sliderValue = m_slider->value();
	if ((sliderValue == m_slider->maximum()) && (!m_repeatButton->isChecked())) {

		m_playTimer.stop();
	}

	if (m_filterState == FilterState::Inactive) {

		m_currentPositionLabel->clear();
		m_filteringManager->EnableTimeFilter(false);
	}
	else {

		const auto& newSelection = m_selectionForEachDistinctValue.at(sliderValue);
		m_currentPositionLabel->setText(newSelection.first);
		
		m_filteringManager->EnableTimeFilter(true);
		m_filteringManager->SetTimeFilterIndexesForTable(m_sourceCacheTableName, newSelection.second);
	}
}

void PseudoTimeFilterWidget::OnIncrementTimeFilter() {

	ChangeFilterState(FilterState::ActivePaused);
	int sliderValue = m_slider->value() + 1;

	if (sliderValue <= m_slider->maximum()) {
		m_slider->setValue(sliderValue);
	}
}

void PseudoTimeFilterWidget::OnDecrementTimeFilter() {

	ChangeFilterState(FilterState::ActivePaused);
	int sliderValue = m_slider->value() - 1;

	if (sliderValue >= 0) {
		m_slider->setValue(sliderValue);
	}
}

void PseudoTimeFilterWidget::ChangeFilterState(FilterState newFilterState) {

	FilterState oldFilterState = m_filterState;
	m_filterState = newFilterState;

	if (newFilterState == FilterState::ActiveRunning) {

		m_playPauseButton->setToolTip(tr("Pause"));
		m_playPauseButton->setIcon(QIcon(":SGXGUI/Resources/Video/pause.png"));
		if (oldFilterState == FilterState::ActivePaused) {

			IncrementTime();
		}
		else {

			UpdateTimeFilter();
		}
		m_playTimer.start();
	}
	else {

		m_playPauseButton->setToolTip(tr("Play"));
		m_playPauseButton->setIcon(QIcon(":SGXGUI/Resources/Video/play.png"));
		m_playTimer.stop();
		
		if (newFilterState == FilterState::Inactive) {

			ResetSliderAndLabel();
			m_filteringManager->EnableTimeFilter(false);
		}
	}

	m_stopButton->setEnabled(newFilterState != FilterState::Inactive);
	m_fieldSelectorButton->setEnabled((newFilterState == FilterState::Inactive) && (m_columnsModel->rowCount() > 0));
}

void PseudoTimeFilterWidget::ResetSliderAndLabel() {

	m_slider->blockSignals(true);
	m_slider->setValue(0);
	m_slider->blockSignals(false);
	m_currentPositionLabel->clear();
}

void PseudoTimeFilterWidget::SetPlayTimerInterval(unsigned int milliseconds) {

	m_playTimer.setInterval(milliseconds);
}

void PseudoTimeFilterWidget::IncrementTime() {

	if (m_filterState == FilterState::ActiveRunning) {

		if (m_slider->value() == m_slider->maximum()) {

			m_slider->setValue(0);
		}
		else {

			m_slider->setValue(m_slider->value() + 1);
		}
	}
}

void PseudoTimeFilterWidget::OnFilterSelectionButtonClicked() {

	QTreeView* fieldSelectorWidget = new QTreeView(this);
	fieldSelectorWidget->setHeaderHidden(true);
	fieldSelectorWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	fieldSelectorWidget->setModel(m_columnsModel);
	fieldSelectorWidget->selectionModel()->select(m_selectedField, QItemSelectionModel::ClearAndSelect);
	fieldSelectorWidget->scrollTo(m_selectedField);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, fieldSelectorWidget, this);
	dialog.setWindowTitle(tr("Select Field For Time Filter"));
	
	if (dialog.exec() == QDialog::Accepted) {
		
		if (!fieldSelectorWidget->selectionModel()->selectedIndexes().isEmpty()) {

			UpdateSelectedField(fieldSelectorWidget->selectionModel()->selectedIndexes().front());
		}
		else {

			UpdateSelectedField(m_columnsModel->index(0, 0, m_columnsModel->index(0, 0, m_columnsModel->index(0, 0))));
		}
	}
}

void PseudoTimeFilterWidget::UpdateSelectedField(const QModelIndex& newSelectedField) {

	bool toIndex = m_selectedField.isValid();
	m_selectedField = newSelectedField;

	QString columnName = m_columnsModel->data(newSelectedField).toString();
	QModelIndex tableIndex = newSelectedField.parent();
	QString tableName = m_columnsModel->data(tableIndex).toString();
	QModelIndex dataSourceIndex = tableIndex.parent();
	QString dataSourceName = m_columnsModel->data(dataSourceIndex).toString();
	QString dataSourceIdString = m_columnsModel->data(dataSourceIndex, SourceDataInfoModel::IDRole).toString();

	m_sourceCacheTableName = SourceDataCache::CreateTablename(dataSourceIdString, tableName);

	if (toIndex){
		m_filteringManager->GetSourceDataCache()->CreateIndex(m_sourceCacheTableName, columnName);
	}
	m_selectionForEachDistinctValue = m_filteringManager->GetSourceDataCache()->GetIndexesOrderedByDistinctValue(m_sourceCacheTableName, columnName);

	m_slider->setMaximum(int(m_selectionForEachDistinctValue.size()) - 1);

	m_fieldLabel->setText(tr("Selected Field: ") + dataSourceName + ":" + tableName + " " + columnName);
}
