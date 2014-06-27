#ifndef BINDINGLINEEDIT_H
#define BINDINGLINEEDIT_H

#include <QtWidgets/QLineEdit>

class BindingLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	BindingLineEdit(QWidget *parent = 0);
	~BindingLineEdit();

protected:
	//virtual void dragEnterEvent(QDragEnterEvent* event);
	//virtual void dropEvent(QDropEvent* event);

private:
	
};

#endif // BINDINGLINEEDIT_H
