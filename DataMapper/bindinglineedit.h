#ifndef BINDINGLINEEDIT_H
#define BINDINGLINEEDIT_H

#include <QtWidgets/QLineEdit>
#include "inputbinding.h"
#include "minmaxglyphmodel.h"

class BindingLineEdit : public QLineEdit
{
	Q_OBJECT

	Q_PROPERTY(SynGlyphX::InputField value READ GetInputField WRITE SetInputField NOTIFY ValueChanged USER true)

public:
	BindingLineEdit(MinMaxGlyphModel* model, QWidget *parent = 0, bool onlyAcceptsNumericFields = true);
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
	MinMaxGlyphModel* m_model;
	SynGlyphX::InputField m_inputField;
	bool m_onlyAcceptsNumericFields;
	QAction* m_clearAction;
	//QAction* m_useInputFieldMinMaxActon;
};

#endif // BINDINGLINEEDIT_H
