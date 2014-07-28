#ifndef BINDINGLINEEDIT_H
#define BINDINGLINEEDIT_H

#include <QtWidgets/QLineEdit>
#include "inputfield.h"

class BindingLineEdit : public QLineEdit
{
	Q_OBJECT

	Q_PROPERTY(SynGlyphX::InputField value READ GetInputField WRITE SetInputField NOTIFY ValueChanged USER true)

public:
	BindingLineEdit(QWidget *parent = 0);
	~BindingLineEdit();

	const SynGlyphX::InputField& GetInputField() const;
	void SetInputField(const SynGlyphX::InputField& inputField);

signals:
	void ValueChanged(const SynGlyphX::InputField&);

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);

private:
	SynGlyphX::InputField m_inputField;
};

#endif // BINDINGLINEEDIT_H
