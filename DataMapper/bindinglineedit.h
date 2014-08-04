#ifndef BINDINGLINEEDIT_H
#define BINDINGLINEEDIT_H

#include <QtWidgets/QLineEdit>
#include "inputbinding.h"

class BindingLineEdit : public QLineEdit
{
	Q_OBJECT

	Q_PROPERTY(SynGlyphX::InputField value READ GetInputField WRITE SetInputField NOTIFY ValueChanged USER true)

public:
	BindingLineEdit(QWidget *parent = 0, bool onlyAcceptsNumericFields = true);
	~BindingLineEdit();

	const SynGlyphX::InputField& GetInputField() const;
	bool OnlyAcceptsNumericField() const;

public slots:
	void SetInputField(const SynGlyphX::InputField& inputfield);
	void Clear();

signals:
	void ValueChanged(const SynGlyphX::InputField&);
	void ValueChangedByUser(const SynGlyphX::InputField&);

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
	virtual void contextMenuEvent(QContextMenuEvent* event);

private:
	SynGlyphX::InputField m_inputField;
	bool m_onlyAcceptsNumericFields;
	QAction* m_clearAction;
	//QAction* m_useInputFieldMinMaxActon;
};

#endif // BINDINGLINEEDIT_H
