#include "imagelabel.h"
#include <QtGui/QResizeEvent>

namespace SynGlyphX {

	ImageLabel::ImageLabel(QWidget *parent)
		: QLabel(parent),
		m_imageRatio(0.0)
	{
		setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		setScaledContents(false);
	}

	ImageLabel::~ImageLabel()
	{

	}

	QSize ImageLabel::sizeHint() const {

		return QSize(250, 125);
	}

	void ImageLabel::SetPixmap(const QPixmap& pixmap) {

		m_pixmap = pixmap;
		m_imageRatio = pixmap.height() / static_cast<double>(pixmap.width());
		ScalePixmap(size());
	}

	void ImageLabel::resizeEvent(QResizeEvent* resizeEvent) {

		ScalePixmap(resizeEvent->size());
		//resizeEvent->accept();
		QLabel::resizeEvent(resizeEvent);
	}

	void ImageLabel::ScalePixmap(const QSize& boundingSize) {

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