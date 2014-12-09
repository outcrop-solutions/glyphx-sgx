#ifndef SIZEWIDGET_H
#define SIZEWIDGET_H

#include "sgxgui_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QAbstractSpinBox>
#include <QtWidgets/QCheckBox>

namespace SynGlyphX {

    class SGXGUI_EXPORT SizeWidget : public QWidget
    {
        Q_OBJECT

    public:
        SizeWidget(bool showLockRatioCheckBox, QWidget *parent = 0);
        ~SizeWidget();

	protected slots:
		virtual void OnWidthChanged() = 0;
		virtual void OnHeightChanged() = 0;

	protected:
		virtual QMetaObject::Connection ConnectLockRatioToWidthSpinBox() = 0;
		virtual QMetaObject::Connection ConnectLockRatioToHeightSpinBox() = 0;
		virtual double GetRatio() = 0;

		void AddSpinBoxes(QAbstractSpinBox* widthSpinBox, QAbstractSpinBox* heightSpinBox);

		double m_ratio;

	private slots:
		void OnLockRatioChanged();

    private:
		QCheckBox* m_lockRatioCheckBox;
		QAbstractSpinBox* m_widthSpinBox;
		QAbstractSpinBox* m_heightSpinBox;

		QMetaObject::Connection m_widthSpinBoxConnection;
		QMetaObject::Connection m_heightSpinBoxConnection;
    };

} //namespace SynGlyphX

#endif // SIZEWIDGET_H
