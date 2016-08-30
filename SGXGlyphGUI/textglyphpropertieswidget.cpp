#include "textglyphpropertieswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QDir>
#include <QtGui/QDesktopServices>
#include "labelline.h"

namespace SynGlyphX {

	TextGlyphPropertiesWidget::TextGlyphPropertiesWidget(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);

		LabelLine* tagLine = new LabelLine(tr("Tag"), this);
		mainLayout->addWidget(tagLine);

		m_tagLineEdit = new QLineEdit(this);
		m_tagLineEdit->setReadOnly(true);
		mainLayout->addWidget(m_tagLineEdit);

		LabelLine* urlLine = new LabelLine(tr("URL"), this);
		mainLayout->addWidget(urlLine);

		m_urlLineEdit = new QTextBrowser(this);
		m_urlLineEdit->setFixedHeight(m_tagLineEdit->sizeHint().height() + 5);
		m_urlLineEdit->setReadOnly(true);
		m_urlLineEdit->setOpenLinks(false);
		mainLayout->addWidget(m_urlLineEdit);
		QObject::connect(m_urlLineEdit, &QTextBrowser::anchorClicked, this, &TextGlyphPropertiesWidget::OpenURL);

		LabelLine* descriptionLine = new LabelLine(tr("Description"), this);
		mainLayout->addWidget(descriptionLine);

		m_descriptionEdit = new QTextBrowser(this);
		m_descriptionEdit->setReadOnly(true);
		m_descriptionEdit->setOpenLinks(false);
		mainLayout->addWidget(m_descriptionEdit, 1);
		QObject::connect(m_descriptionEdit, &QTextBrowser::anchorClicked, this, &TextGlyphPropertiesWidget::OpenURL);

		setLayout(mainLayout);
	}

	TextGlyphPropertiesWidget::~TextGlyphPropertiesWidget()
	{

	}

	void TextGlyphPropertiesWidget::SetWidget(const QString& tag, const QString& urlString, const QString& description) {

		m_tagLineEdit->setText(tag);

		QUrl url = QUrl::fromUserInput(urlString);
		if (url.isValid() && (urlString != "nourl.html")) {

			QString urlLink = url.toString(QUrl::None);
			QString urlDisplay;
			if (url.isLocalFile()) {

				urlDisplay = QDir::toNativeSeparators(url.toString(QUrl::PreferLocalFile).mid(1));
			}
			else {

				urlDisplay = urlLink;
			}
			m_urlLineEdit->setText(QString("<a href=\"%1\">%2</a>").arg(urlLink).arg(urlDisplay));
		}
		else {

			m_urlLineEdit->setPlainText(tr("No URL"));
		}

		m_descriptionEdit->setHtml(description);
	}

	void TextGlyphPropertiesWidget::SetReadOnly(bool readOnly) {

		m_tagLineEdit->setReadOnly(readOnly);
		m_urlLineEdit->setReadOnly(readOnly);
		m_descriptionEdit->setReadOnly(readOnly);
	}

	void TextGlyphPropertiesWidget::OpenURL(const QUrl& url) {

		QDesktopServices::openUrl(url);
	}

} //namespace SynGlyphX