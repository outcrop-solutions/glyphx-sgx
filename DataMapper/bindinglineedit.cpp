#include "bindinglineedit.h"

BindingLineEdit::BindingLineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	setReadOnly(true);
}

BindingLineEdit::~BindingLineEdit()
{

}
