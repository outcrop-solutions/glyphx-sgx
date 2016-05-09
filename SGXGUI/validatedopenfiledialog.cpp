#include "validatedopenfiledialog.h"
#include <QtWidgets/QFileDialog>

namespace SynGlyphX {

	ValidatedOpenFileDialog::ValidatedOpenFileDialog(const QString& startingDir, const QString& caption, const QString& filter, QWidget *parent)
		: QObject(parent),
		m_startingDirectory(startingDir),
		m_caption(caption),
		m_filter(filter)
	{

	}

	ValidatedOpenFileDialog::~ValidatedOpenFileDialog()
	{

	}

	int ValidatedOpenFileDialog::Exec() {

		QString filename;
		do {

			filename = QFileDialog::getOpenFileName(dynamic_cast<QWidget*>(parent()), m_caption, m_startingDirectory, m_filter);
			if (filename.isEmpty()) {

				return QDialog::Rejected;
			}

		} while (!IsFileValid(filename));

		return QDialog::Accepted;
	}

} //namespace SynGlyphX