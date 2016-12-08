#include "ResizeableImageLabel.h"
#include <QtGui/QResizeEvent>
#include "application.h"

namespace SynGlyphX {

	ResizeableImageLabel::ResizeableImageLabel(bool fixedImageRatio, QWidget *parent)
		: QLabel(parent),
        m_imageRatio(0.0),
		m_fixedImageRatio(fixedImageRatio)
	{
		setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		setScaledContents(false);
	}

	ResizeableImageLabel::~ResizeableImageLabel()
	{

	}

	QSize ResizeableImageLabel::sizeHint() const {

		return Application::DynamicQSize(250, 125); //QSize(250, 125);
	}

	void ResizeableImageLabel::SetPixmap(const QPixmap& pixmap) {

		m_pixmap = pixmap;
		if (m_fixedImageRatio) {

			m_imageRatio = pixmap.height() / static_cast<double>(pixmap.width());
		}
		ScalePixmap(size());
	}

	void ResizeableImageLabel::resizeEvent(QResizeEvent* resizeEvent) {

		ScalePixmap(resizeEvent->size());
		//resizeEvent->accept();
		QLabel::resizeEvent(resizeEvent);
	}

	void ResizeableImageLabel::ScalePixmap(const QSize& boundingSize) {

		if (!m_pixmap.isNull()) {

			if (m_fixedImageRatio) {

				QSize pixmapSize;
				double widgetAspectRatio = boundingSize.height() / static_cast<double>(boundingSize.width());
				if (widgetAspectRatio < m_imageRatio) {

					setPixmap(m_pixmap.scaledToHeight(boundingSize.height()-50, Qt::SmoothTransformation));
				}
				else {

					setPixmap(m_pixmap.scaledToWidth(boundingSize.width()-50, Qt::SmoothTransformation));
				}
			}
			else {

				setPixmap(m_pixmap.scaled(boundingSize));
			}			
		}
	}

} //namespace SynGlyphX
