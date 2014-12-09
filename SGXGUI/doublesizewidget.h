#ifndef DOUBLESIZEWIDGET_H
#define DOUBLESIZEWIDGET_H

#include "sizewidget.h"
#include <QtWidgets/QDoubleSpinBox>

namespace SynGlyphX {

	class SGXGUI_EXPORT DoubleSizeWidget : public SizeWidget
	{
		Q_OBJECT

	public:
		DoubleSizeWidget(bool showLockRatioCheckBox, QWidget *parent);
		~DoubleSizeWidget();

		void SetSize(const QSizeF& size);
		QSizeF GetSize() const;

		void SetRange(double min, double max);

	protected slots:
		virtual void OnWidthChanged();
		virtual void OnHeightChanged();

	protected:
		virtual QMetaObject::Connection ConnectLockRatioToWidthSpinBox();
		virtual QMetaObject::Connection ConnectLockRatioToHeightSpinBox();
		virtual double GetRatio();

	private:
		QDoubleSpinBox* m_doubleWidthSpinBox;
		QDoubleSpinBox* m_doubleHeightSpinBox;

	};

} //namespace SynGlyphX

#endif // DOUBLESIZEWIDGET_H
