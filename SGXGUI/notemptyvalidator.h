#ifndef NOTEMPTYVALIDATOR_H
#define NOTEMPTYVALIDATOR_H

#include "sgxgui_global.h"
#include <QtGui/QRegExpValidator>

namespace SynGlyphX {

	class SGXGUI_EXPORT NotEmptyValidator : public QRegExpValidator
	{
		Q_OBJECT

	public:
		NotEmptyValidator(bool allowWhitespace, QObject *parent);
		~NotEmptyValidator();

	private:

	};

} //namespace SynGlyphX

#endif // NOTEMPTYVALIDATOR_H
