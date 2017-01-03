#include "ResizeableImageLabel.h"
#include <QtGui/QResizeEvent>
#include "application.h"

namespace SynGlyphX {

	ResizeableImageLabel::ResizeableImageLabel(bool fixedImageRatio, QWidget *parent)
		: QLabel(parent),
		m_imageRatio(0.0),
		m_fixedImageRatio(fixedImageRatio),
		vertical_padding(0),
		horizontal_padding(0)
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

	void ResizeableImageLabel::AddPadding(int top, int right, int bottom, int left){
		vertical_padding = top + bottom;
		horizontal_padding = left + right;
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

					setPixmap(m_pixmap.scaledToHeight(boundingSize.height()-vertical_padding, Qt::SmoothTransformation));
				}
				else {

					setPixmap(m_pixmap.scaledToWidth(boundingSize.width()-horizontal_padding, Qt::SmoothTransformation));
				}
			}
			else {

				setPixmap(m_pixmap.scaled(boundingSize));
			}			
		}
	}

} //namespace SynGlyphX
