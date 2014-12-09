#ifndef INTSIZEWIDGET_H
#define INTSIZEWIDGET_H

#include "sizewidget.h"
#include <QtWidgets/QSpinBox>

namespace SynGlyphX {

	class SGXGUI_EXPORT IntSizeWidget : public SizeWidget
	{
		Q_OBJECT

	public:
		IntSizeWidget(bool showLockRatioCheckBox, QWidget *parent);
		~IntSizeWidget();

		void SetSize(const QSize& size);
		QSize GetSize() const;

		void SetRange(int min, int max);

	protected slots:
		virtual void OnWidthChanged();
		virtual void OnHeightChanged();

	protected:
		virtual QMetaObject::Connection ConnectLockRatioToWidthSpinBox();
		virtual QMetaObject::Connection ConnectLockRatioToHeightSpinBox();
		virtual double GetRatio();

	private:
		QSpinBox* m_intWidthSpinBox;
		QSpinBox* m_intHeightSpinBox;
	};

} //namespace SynGlyphX

#endif // INTSIZEWIDGET_H
