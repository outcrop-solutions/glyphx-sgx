#include "imagelabel.h"
#include <QtGui/QResizeEvent>

namespace SynGlyphX {

	ImageLabel::ImageLabel(QWidget *parent)
		: QLabel(parent),
		m_imageRatio(0.0)
	{
		setScaledContents(false);
	}

	ImageLabel::~ImageLabel()
	{

	}

	QSize ImageLabel::sizeHint() const {

		//unsigned int widgetWidth = width();
		//return QSize(widgetWidth, static_cast<int>(m_imageRatio * widgetWidth));
		//return QSize(widgetWidth, widgetWidth / 2);
		return QSize(250, 125);
	}

	void ImageLabel::SetPixmap(const QPixmap& pixmap) {

		m_pixmap = pixmap;
		m_imageRatio = pixmap.height() / static_cast<double>(pixmap.width());
		ScalePixmap();
	}

	void ImageLabel::resizeEvent(QResizeEvent* resizeEvent) {

		ScalePixmap();
		resizeEvent->accept();
		QLabel::resizeEvent(resizeEvent);
	}

	void ImageLabel::ScalePixmap() {

		if (!m_pixmap.isNull()) {

			setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}
	}

} //namespace SynGlyphX