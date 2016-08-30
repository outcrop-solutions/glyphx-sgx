#include "notemptyvalidator.h"

namespace SynGlyphX {

	NotEmptyValidator::NotEmptyValidator(bool allowWhitespace, QObject *parent)
		: QRegularExpressionValidator(parent)
	{
		QRegularExpression regExp;
		if (allowWhitespace) {

			//makes sure that there has to be at least one character (this can include whitespace)
			regExp.setPattern(".+");
		}
		else {

			//Must have at least one or more chracters, none of which can be whitespace
			regExp.setPattern("\\S+");
		}
		        
		setRegularExpression(regExp);
	}

	NotEmptyValidator::~NotEmptyValidator()
	{

	}

} //namespace SynGlyphX