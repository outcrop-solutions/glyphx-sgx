#ifndef COLORALPHAWIDGET_H
#define COLORALPHAWIDGET_H

#include "sgxgui_global.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include "foundationtypes.h"
#include "colorbutton.h"

namespace SynGlyphX {

	class SGXGUI_EXPORT ColorAlphaWidget : public QWidget
	{
		Q_OBJECT

	public:
		ColorAlphaWidget(QWidget *parent = 0);
		~ColorAlphaWidget();

		void SetColor(const SynGlyphX::GlyphColor& color, int alpha);
		const QColor& GetColor() const;

	public slots:
		void SetColor(const QColor& color);

	signals:
		void ColorChanged(const QColor& color);

	private slots:
		void OnAlphaSpinBoxChanged(int value);

	private:
		QColor m_color;
		ColorButton* m_button;
		QSpinBox* m_alphaSpinBox;
	};

} //namespace SynGlyphX

#endif // COLORALPHAWIDGET_H
