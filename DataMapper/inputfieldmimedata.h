#ifndef INPUTFIELDMIMEDATA_H
#define INPUTFIELDMIMEDATA_H

#include <QtCore/QMimeData>
#include "inputfield.h"

class InputFieldMimeData : public QMimeData
{
	Q_OBJECT

public:
	static const QString MimeType;

	InputFieldMimeData(const SynGlyphX::InputField& inputField);
	~InputFieldMimeData();

	const SynGlyphX::InputField& GetInputField() const;

	virtual QStringList formats() const;
	virtual bool hasFormat(const QString& mimeType) const;

private:
	SynGlyphX::InputField m_inputfield;
};

#endif // INPUTFIELDMIMEDATA_H
