#ifndef RICHTEXTEDITOR_H
#define RICHTEXTEDITOR_H

#include "sgxgui_global.h"
#include <QtWidgets/QTextEdit>

namespace SynGlyphX {

	class SGXGUI_EXPORT RichTextEditor : public QWidget
	{
		Q_OBJECT

	public:
		RichTextEditor(const QString& title = "", QWidget *parent = 0);
		~RichTextEditor();

	private:
		QTextEdit* m_textEdit;
	};

} //namespace SynGlyphx

#endif // RICHTEXTEDITOR_H
