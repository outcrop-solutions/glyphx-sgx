#include "inputfieldmimedata.h"

const QString InputFieldMimeData::MimeType = "application/datasource-field";

InputFieldMimeData::InputFieldMimeData(const SynGlyphX::InputField& inputfield)
	: QMimeData(),
	m_inputfield(inputfield)
{

}

InputFieldMimeData::~InputFieldMimeData()
{

}

const SynGlyphX::InputField& InputFieldMimeData::GetInputField() const {

	return m_inputfield;
}

QStringList InputFieldMimeData::formats() const {

	QStringList list;
	list.push_back(MimeType);
	return list;
}

bool InputFieldMimeData::hasFormat(const QString& mimeType) const {

	return mimeType == MimeType;
}