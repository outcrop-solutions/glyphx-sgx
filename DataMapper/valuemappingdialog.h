#ifndef VALUEMAPPINGDIALOG_H
#define VALUEMAPPINGDIALOG_H

#include <QtWidgets/QDialog>

class ValueMappingDialog : public QDialog
{
	Q_OBJECT

public:
	ValueMappingDialog(bool isNumericOnly, bool isRange, QWidget *parent);
	~ValueMappingDialog();

protected:
	bool m_isNumericOnly;
	bool m_isRange;
};

#endif // VALUEMAPPINGDIALOG_H
