#include "singledatetimerangefilterwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <cmath>

namespace SynGlyphX {

	SingleDatetimeRangeFilterWidget::SingleDatetimeRangeFilterWidget(Qt::Orientation orientation, QWidget *parent)
		: SingleRangeFilterWidget(parent)
	{

		mainLayout = new QVBoxLayout(this);

		UseDateAndTimeLayout();

		setLayout(mainLayout);

		QSize sz(size()); //100,30
		QSize psz(parent->size()); //356,726

	}

	void SingleDatetimeRangeFilterWidget::UseDateAndTimeLayout() {

		QHBoxLayout* minLayout = new QHBoxLayout(this);
		QLabel* minDate = new QLabel("From:");
		minLayout->addWidget(minDate);

		QHBoxLayout* innerMinLayout = new QHBoxLayout(this);
		m_minDateEdit = new QDateEdit(QDate::currentDate());
		m_minDateEdit->setCalendarPopup(true);
		innerMinLayout->addWidget(m_minDateEdit, 0, Qt::AlignLeft);
		m_minTimeEdit = new QTimeEdit(QTime::currentTime());
		innerMinLayout->addWidget(m_minTimeEdit, 1, Qt::AlignLeft);

		minLayout->addLayout(innerMinLayout, 1);
		mainLayout->addLayout(minLayout);

		QFrame* line = new QFrame(this);
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Sunken);
		mainLayout->addWidget(line);

		QHBoxLayout* maxLayout = new QHBoxLayout(this);
		QLabel* maxDate = new QLabel("To:    ");
		maxLayout->addWidget(maxDate);

		QHBoxLayout* innerMaxLayout = new QHBoxLayout(this);
		m_maxDateEdit = new QDateEdit(QDate::currentDate());
		m_maxDateEdit->setCalendarPopup(true);
		innerMaxLayout->addWidget(m_maxDateEdit, 0, Qt::AlignLeft);
		m_maxTimeEdit = new QTimeEdit(QTime::currentTime());
		innerMaxLayout->addWidget(m_maxTimeEdit, 1, Qt::AlignLeft);

		maxLayout->addLayout(innerMaxLayout, 1);
		mainLayout->addLayout(maxLayout);

	}

	void SingleDatetimeRangeFilterWidget::SetFieldProperties(FieldProperties fp) { 

		m_fieldProperties = fp; 

		QDateTime min(QDateTime::fromMSecsSinceEpoch(m_min));
		QDateTime max(QDateTime::fromMSecsSinceEpoch(m_max));

		m_minDateEdit->setDateRange(min.date(), max.date());
		m_maxDateEdit->setDateRange(min.date(), max.date());
		m_minDateEdit->setDisplayFormat(m_fieldProperties.DateFormat());
		m_maxDateEdit->setDisplayFormat(m_fieldProperties.DateFormat());
		m_minDateEdit->setDate(min.date());
		m_maxDateEdit->setDate(max.date());

		m_minTimeEdit->setTimeRange(min.time(), max.time());
		m_maxTimeEdit->setTimeRange(min.time(), max.time());
		m_minTimeEdit->setDisplayFormat(m_fieldProperties.TimeFormat());
		m_maxTimeEdit->setDisplayFormat(m_fieldProperties.TimeFormat());
		m_minTimeEdit->setTime(min.time());
		m_maxTimeEdit->setTime(max.time());

		QObject::connect(m_minDateEdit, &QDateEdit::dateChanged, this, &SingleDatetimeRangeFilterWidget::OnMinDatetimeEditUpdated);
		QObject::connect(m_maxDateEdit, &QDateEdit::dateChanged, this, &SingleDatetimeRangeFilterWidget::OnMaxDatetimeEditUpdated);
		QObject::connect(m_minTimeEdit, &QTimeEdit::timeChanged, this, &SingleDatetimeRangeFilterWidget::OnMinDatetimeEditUpdated);
		QObject::connect(m_maxTimeEdit, &QTimeEdit::timeChanged, this, &SingleDatetimeRangeFilterWidget::OnMaxDatetimeEditUpdated);

	}

	void SingleDatetimeRangeFilterWidget::SetRange(const DegenerateInterval& range) {

		m_min = range.GetMin();
		m_max = range.GetMax();
	}

	void SingleDatetimeRangeFilterWidget::OnMinDatetimeEditUpdated() {

		EmitRangeUpdate();
	}

	void SingleDatetimeRangeFilterWidget::OnMaxDatetimeEditUpdated() {

		EmitRangeUpdate();
	}

	void SingleDatetimeRangeFilterWidget::EmitRangeUpdate() {

		emit RangeUpdated(DegenerateInterval(GetMinDatetimeEdit(), GetMaxDatetimeEdit()));
	}

	void SingleDatetimeRangeFilterWidget::SetMinDatetimeEdit(qint64 val) {

		m_minDateEdit->setDate(QDateTime::fromMSecsSinceEpoch(val).date());
		m_minTimeEdit->setTime(QDateTime::fromMSecsSinceEpoch(val).time());
	}

	qint64 SingleDatetimeRangeFilterWidget::GetMinDatetimeEdit() const {

		return QDateTime(m_minDateEdit->date(), m_minTimeEdit->time()).toMSecsSinceEpoch();
	}

	void SingleDatetimeRangeFilterWidget::SetMaxDatetimeEdit(qint64 val) {

		m_maxDateEdit->setDate(QDateTime::fromMSecsSinceEpoch(val).date());
		m_maxTimeEdit->setTime(QDateTime::fromMSecsSinceEpoch(val).time());
	}

	qint64 SingleDatetimeRangeFilterWidget::GetMaxDatetimeEdit() const {

		return QDateTime(m_maxDateEdit->date(), m_maxTimeEdit->time()).toMSecsSinceEpoch();
	}

	QString SingleDatetimeRangeFilterWidget::ConvertDoubleToString(double val, int decimals) const {

		QString str = QString::number(val, 'f', decimals);
		while (str.endsWith('0')) {

			str.chop(1);
		}
		if (str.endsWith('.')) {

			str.chop(1);
		}

		return str;
	}

} //namespace SynGlyphX