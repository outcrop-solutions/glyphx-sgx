#ifndef PSEUDOTIMEFILTERWIDGET_H
#define PSEUDOTIMEFILTERWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtCore/QTimer>
#include <QtWidgets/QLabel>
#include "sourcedatainfomodel.h"

class PseudoTimeFilterWidget : public QWidget
{
	Q_OBJECT

public:
	PseudoTimeFilterWidget(SynGlyphX::DataTransformMapping::SharedPtr dataTransformMapping, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, QWidget *parent);
	~PseudoTimeFilterWidget();

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
	void UpdateSelectedFieldLabel();

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
	QLabel* m_fieldLabel;

	QTimer m_playTimer;

	FilterState m_filterState;

	static unsigned int s_buttonSize;
};

#endif // PSEUDOTIMEFILTERWIDGET_H
