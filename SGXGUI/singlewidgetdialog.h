#ifndef SINGLEWIDGETDIALOG_H
#define SINGLEWIDGETDIALOG_H

#include "sgxgui_global.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

namespace SynGlyphX {

	class SGXGUI_EXPORT SingleWidgetDialog : public QDialog
	{
		Q_OBJECT

	public:
		SingleWidgetDialog(QDialogButtonBox::StandardButtons buttons, QWidget* widget, QWidget *parent = 0);
		~SingleWidgetDialog();

	private:

	};

} //namespace SynGlyphX

#endif // SINGLEWIDGETDIALOG_H
