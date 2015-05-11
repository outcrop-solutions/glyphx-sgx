#include "pseudotimefilterwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtCore/QSettings>
#include <QtWidgets/QTreeView>
#include "singlewidgetdialog.h"

unsigned int PseudoTimeFilterWidget::s_buttonSize = 24;

PseudoTimeFilterWidget::PseudoTimeFilterWidget(SynGlyphX::DataTransformMapping::SharedPtr dataTransformMapping, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, QWidget *parent)
	: QWidget(parent),
	m_filterState(FilterState::Inactive)
{
	m_playTimer.setSingleShot(false);

	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* sliderValueLayout = new QHBoxLayout(this);

	m_slider = new QSlider(Qt::Horizontal, this);
	m_slider->setMinimum(0);
	QObject::connect(m_slider, &QSlider::valueChanged, this, &PseudoTimeFilterWidget::OnSliderValueChanged);
	sliderValueLayout->addWidget(m_slider, 1);
	m_currentPositionLabel = new QLineEdit(this);
	m_currentPositionLabel->setReadOnly(true);
	sliderValueLayout->addWidget(m_currentPositionLabel);

	layout->addLayout(sliderValueLayout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	QHBoxLayout* buttonsLayoutLeft = new QHBoxLayout(this);
	buttonsLayoutLeft->setContentsMargins(0, 0, 0, 0);

	m_fieldSelectorButton = new QPushButton(tr("Select Field"), this);
	QObject::connect(m_fieldSelectorButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnFilterSelectionButtonClicked);
	m_columnsModel = new SourceDataInfoModel(dataTransformMapping, sourceDataCache, this);
	m_columnsModel->SetSelectable(false, false, true);

	buttonsLayoutLeft->addWidget(m_fieldSelectorButton);

	m_fieldLabel = new QLabel(this);
	buttonsLayoutLeft->addWidget(m_fieldLabel);

	buttonsLayoutLeft->addStretch(1);

	m_repeatButton = new QPushButton(this);
	m_repeatButton->setIcon(QIcon(":SGXGUI/Resources/Video/repeat.png"));
	m_repeatButton->setIconSize(QSize(s_buttonSize, s_buttonSize));
	m_repeatButton->setFixedSize(s_buttonSize, s_buttonSize);
	m_repeatButton->setCheckable(true);
	m_repeatButton->setToolTip(tr("Repeat"));
	buttonsLayoutLeft->addWidget(m_repeatButton);

	m_stopButton = new QPushButton(this);
	m_stopButton->setIcon(QIcon(":SGXGUI/Resources/Video/stop.png"));
	m_stopButton->setIconSize(QSize(s_buttonSize, s_buttonSize));
	m_stopButton->setFixedSize(s_buttonSize, s_buttonSize);
	m_stopButton->setToolTip(tr("Stop"));
	QObject::connect(m_stopButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnStop);
	buttonsLayoutLeft->addWidget(m_stopButton);

	m_goToStartButton = new QPushButton(this);
	m_goToStartButton->setIcon(QIcon(":SGXGUI/Resources/Video/start.png"));
	m_goToStartButton->setIconSize(QSize(1.5 * s_buttonSize, s_buttonSize));
	m_goToStartButton->setFixedSize(1.5 * s_buttonSize, s_buttonSize);
	m_goToStartButton->setToolTip(tr("Go To Start"));
	QObject::connect(m_goToStartButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnGoToStart);
	buttonsLayoutLeft->addWidget(m_goToStartButton);

	buttonsLayout->addLayout(buttonsLayoutLeft, 1);

	QHBoxLayout* buttonsLayoutCenter = new QHBoxLayout(this);
	buttonsLayoutCenter->setContentsMargins(0, 0, 0, 0);
	buttonsLayoutCenter->setSizeConstraint(QLayout::SetFixedSize);

	m_playPauseButton = new QPushButton(this);
	m_playPauseButton->setFixedSize(2 * s_buttonSize, 2 * s_buttonSize);
	m_playPauseButton->setIconSize(QSize(2 * s_buttonSize, 2 * s_buttonSize));
	QObject::connect(m_playPauseButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnPlayPause);
	buttonsLayoutCenter->addWidget(m_playPauseButton);

	buttonsLayout->addLayout(buttonsLayoutCenter);

	QHBoxLayout* buttonsLayoutRight = new QHBoxLayout(this);
	buttonsLayoutRight->setContentsMargins(0, 0, 0, 0);

	m_goToEndButton = new QPushButton(this);
	m_goToEndButton->setIcon(QIcon(":SGXGUI/Resources/Video/end.png"));
	m_goToEndButton->setIconSize(QSize(1.5 * s_buttonSize, s_buttonSize));
	m_goToEndButton->setFixedSize(1.5 * s_buttonSize, s_buttonSize);
	m_goToEndButton->setToolTip(tr("Go To End"));
	QObject::connect(m_goToEndButton, &QPushButton::clicked, this, &PseudoTimeFilterWidget::OnGoToEnd);
	buttonsLayoutRight->addWidget(m_goToEndButton);

	buttonsLayoutRight->addStretch(1);

	

	buttonsLayout->addLayout(buttonsLayoutRight, 1);

	layout->addLayout(buttonsLayout);

	setLayout(layout);

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
	m_columnsModel->Reset();

	m_selectedField = m_columnsModel->index(0, 0, m_columnsModel->index(0, 0, m_columnsModel->index(0, 0)));
	UpdateSelectedFieldLabel();
	ChangeFilterState(FilterState::Inactive);
}

void PseudoTimeFilterWidget::Disable() {

	EnableButtons(false);
	ChangeFilterState(FilterState::Inactive);
}

void PseudoTimeFilterWidget::ReadSettings() {

	QSettings settings;
	settings.beginGroup("PseudoTimeFilter");
	m_repeatButton->setChecked(settings.value("repeat", false).toBool());
	settings.endGroup();
}

void PseudoTimeFilterWidget::WriteSettings() {

	QSettings settings;
	settings.beginGroup("PseudoTimeFilter");
	settings.setValue("repeat", m_repeatButton->isChecked());
	settings.endGroup();
}

void PseudoTimeFilterWidget::EnableButtons(bool enable) {

	m_slider->setEnabled(enable);
	m_currentPositionLabel->setEnabled(enable);
	m_stopButton->setEnabled(enable);
	m_goToStartButton->setEnabled(enable);
	m_playPauseButton->setEnabled(enable);
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

	if ((m_slider->value() == m_slider->maximum()) && (!m_repeatButton->isChecked())) {

		m_playTimer.stop();
	}

	m_currentPositionLabel->setText(QString::number(m_slider->value()));
}

void PseudoTimeFilterWidget::ChangeFilterState(FilterState newFilterState) {

	if (newFilterState == FilterState::ActiveRunning) {

		m_playPauseButton->setToolTip(tr("Pause"));
		m_playPauseButton->setIcon(QIcon(":SGXGUI/Resources/Video/pause.png"));
		if (m_filterState == FilterState::ActivePaused) {

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

			m_slider->blockSignals(true);
			m_slider->setValue(0);
			m_slider->blockSignals(false);
			m_currentPositionLabel->clear();
		}
		else {

			
		}
	}

	m_stopButton->setEnabled(newFilterState != FilterState::Inactive);
	m_fieldSelectorButton->setEnabled((newFilterState == FilterState::Inactive) && (m_columnsModel->rowCount() > 0));

	m_filterState = newFilterState;
}

void PseudoTimeFilterWidget::SetPlayTimerInterval(unsigned int milliseconds) {

	m_playTimer.setInterval(milliseconds);
}

void PseudoTimeFilterWidget::IncrementTime() {

	if (m_slider->value() == m_slider->maximum()) {

		m_slider->setValue(0);
	}
	else {

		m_slider->setValue(m_slider->value() + 1);
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

			m_selectedField = fieldSelectorWidget->selectionModel()->selectedIndexes().front();
		}
		else {

			m_selectedField = m_columnsModel->index(0, 0, m_columnsModel->index(0, 0, m_columnsModel->index(0, 0)));
		}
		UpdateSelectedFieldLabel();
	}
}

void PseudoTimeFilterWidget::UpdateSelectedFieldLabel() {

	m_fieldLabel->setText(tr("Selected Field: ") + m_columnsModel->data(m_selectedField).toString());
}