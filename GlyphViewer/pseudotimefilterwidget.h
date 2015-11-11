#ifndef PSEUDOTIMEFILTERWIDGET_H
#define PSEUDOTIMEFILTERWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include <unordered_map>
#include "sourcedatainfomodel.h"
#include <QtWidgets/QCheckBox>
#include "linkedwidgetsmanager.h"
#include "sourcedataselectionmodel.h"

class PseudoTimeFilterWidget : public QWidget
{
	Q_OBJECT

public:
	PseudoTimeFilterWidget(SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent);
	~PseudoTimeFilterWidget();

	void SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgetsManager);

	void ResetForNewVisualization();
	void Disable();

	void SetPlayTimerInterval(unsigned int milliseconds);

private slots:
	void OnPlayPause();
	void OnStop();
	void OnGoToStart();
	void OnGoToEnd();
	void OnSliderValueChanged(int index);
	void IncrementTime();
	void OnFilterSelectionButtonClicked();

private:
	enum FilterState {
		Inactive,
		ActivePaused,
		ActiveRunning
	};

	void ReadSettings();
	void WriteSettings();
	void EnableButtons(bool enable);
	void ChangeFilterState(FilterState newFilterState);
	void UpdateTimeFilter();
	void UpdateSelectedField(const QModelIndex& newSelectedField);

	QSlider* m_slider;
	QLineEdit* m_currentPositionLabel;

	//Play/Pause, Stop, Etc. buttons
	QPushButton* m_repeatButton;
	QPushButton* m_stopButton;
	QPushButton* m_goToStartButton;
	QPushButton* m_playPauseButton;
	QPushButton* m_goToEndButton;

	QPushButton* m_fieldSelectorButton;
	SourceDataInfoModel* m_columnsModel;
	QPersistentModelIndex m_selectedField;
	unsigned long m_selectedTableStartingIndex;
	QLabel* m_fieldLabel;

	QTimer m_playTimer;

	FilterState m_filterState;
	SynGlyphX::SourceDataCache::SharedPtr m_sourceDataCache;
	SynGlyphX::SourceDataCache::DistinctValueIndexMap m_selectionForEachDistinctValue;
	QString m_sourceCacheTableName;
	SourceDataSelectionModel* m_selectionModel;

	QCheckBox* m_filterViewCheckbox;
	QCheckBox* m_moveCameraOnUpdateCheckbox;

	static unsigned int s_buttonSize;
};

#endif // PSEUDOTIMEFILTERWIDGET_H
