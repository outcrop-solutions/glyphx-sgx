#include "fixedaspectratioimagelabel.h"
#include <QtGui/QResizeEvent>

namespace SynGlyphX {

	FixedAspectRatioImageLabel::FixedAspectRatioImageLabel(QWidget *parent)
		: QLabel(parent),
		m_imageRatio(0.0)
	{
		setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		setScaledContents(false);
	}

	FixedAspectRatioImageLabel::~FixedAspectRatioImageLabel()
	{

	}

	QSize FixedAspectRatioImageLabel::sizeHint() const {

		return QSize(250, 125);
	}

	void FixedAspectRatioImageLabel::SetPixmap(const QPixmap& pixmap) {

		m_pixmap = pixmap;
		m_imageRatio = pixmap.height() / static_cast<double>(pixmap.width());
		ScalePixmap(size());
	}

	void FixedAspectRatioImageLabel::resizeEvent(QResizeEvent* resizeEvent) {

		ScalePixmap(resizeEvent->size());
		//resizeEvent->accept();
		QLabel::resizeEvent(resizeEvent);
	}

	void FixedAspectRatioImageLabel::ScalePixmap(const QSize& boundingSize) {

		if (!m_pixmap.isNull()) {

			QSize pixmapSize;
			double widgetAspectRatio = boundingSize.height() / static_cast<double>(boundingSize.width());
			if (widgetAspectRatio < m_imageRatio) {

				setPixmap(m_pixmap.scaledToHeight(boundingSize.height(), Qt::SmoothTransformation));
			}
			else {

				setPixmap(m_pixmap.scaledToWidth(boundingSize.width(), Qt::SmoothTransformation));
			}
			
		}
	}

} //namespace SynGlyphX