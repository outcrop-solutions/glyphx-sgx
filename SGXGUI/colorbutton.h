#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include "sgxgui_global.h"
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMenu>
#include "foundationtypes.h"

namespace SynGlyphX {

	class SGXGUI_EXPORT ColorButton : public QToolButton
	{
		Q_OBJECT

	public:
		ColorButton(bool showAlphaInColorDialog = false, QWidget *parent = 0);
		~ColorButton();

		void SetColor(const SynGlyphX::Color& color);
		const QColor& GetColor() const;

		virtual QSize minimumSizeHint() const;

		static SynGlyphX::Color ConvertQColorToColor(const QColor& qColor);

	public slots:
		void SetColor(const QColor& color);

	signals:
		void ColorChanged(const QColor& color);

	private slots:
		void OnCustomColor();

	private:
		void UpdateButtonColor();

		QColor m_color;
		QMenu* m_menu;
		bool m_showAlphaInColorDialog;
	};

} //namespace SynGlyphX

#endif // COLORBUTTON_H
