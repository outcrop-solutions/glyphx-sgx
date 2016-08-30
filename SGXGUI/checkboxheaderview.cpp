#include "checkboxheaderview.h"
#include <QtGui/QPainter>

namespace SynGlyphX {

	CheckBoxHeaderView::CheckBoxHeaderView(Qt::Orientation orientation, QWidget *parent)
		: QHeaderView(orientation, parent),
		m_checkBoxState(AllSomeNone::None)
	{

	}

	CheckBoxHeaderView::~CheckBoxHeaderView()
	{

	}

	void CheckBoxHeaderView::SetState(AllSomeNone state) {

		if (m_checkBoxState != state) {

			m_checkBoxState = state;
			RedrawCheckBox();
		}
	}

	AllSomeNone CheckBoxHeaderView::GetState() const {

		return m_checkBoxState;
	}

	void CheckBoxHeaderView::mousePressEvent(QMouseEvent* event) {

		if (m_checkBoxState == AllSomeNone::None) {

			SetState(AllSomeNone::All);
		}
		else {

			SetState(AllSomeNone::None);
		}

		emit CheckBoxClicked(m_checkBoxState);
	}

	void CheckBoxHeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const {

		painter->save();
		QHeaderView::paintSection(painter, rect, logicalIndex);
		painter->restore();

		if (logicalIndex == 0)
		{
			QStyleOptionButton option;
			option.rect = QRect(1, 3, 20, 20);
			option.state = QStyle::State_Enabled | QStyle::State_Active;

			if (m_checkBoxState == AllSomeNone::All) {

				option.state |= QStyle::State_On;
			}
			else if (m_checkBoxState == AllSomeNone::None) {

				option.state |= QStyle::State_Off;
			}
			else {

				option.state |= QStyle::State_NoChange;
			}

			style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
		}
	}

	void CheckBoxHeaderView::RedrawCheckBox() {

		viewport()->update();
	}

} //namespace SynGlyphX