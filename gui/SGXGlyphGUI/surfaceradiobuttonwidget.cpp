#include "surfaceradiobuttonwidget.h"

namespace SynGlyphX {

	const QStringList SurfaceRadioButtonWidget::s_buttonNames = SurfaceRadioButtonWidget::GenerateButtonNames();

	SurfaceRadioButtonWidget::SurfaceRadioButtonWidget(Qt::Orientation alignment, QWidget *parent)
		: RadioButtonGroupWidget(s_buttonNames, alignment, parent)
	{
		
	}

	SurfaceRadioButtonWidget::~SurfaceRadioButtonWidget()
	{

	}

	QStringList SurfaceRadioButtonWidget::GenerateButtonNames() {

		QStringList surfaceNames;
		for (auto surface : SynGlyphX::GlyphGeometryInfo::s_surfaceNames.left) {

			surfaceNames.push_back(QString::fromStdWString(surface.second));
		}
		return surfaceNames;
	}

	void SurfaceRadioButtonWidget::SetSurface(GlyphGeometryInfo::Surface surface) {

		blockSignals(true);
		SetCheckedButton(surface);
		blockSignals(false);
	}

	GlyphGeometryInfo::Surface SurfaceRadioButtonWidget::GetSurface() const {

		return SynGlyphX::GlyphGeometryInfo::s_surfaceNames.right.at(GetCheckedButtonLabel().toStdWString());
	}

} //namespace SynGlyphX