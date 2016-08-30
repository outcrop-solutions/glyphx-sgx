#include "prefixsuffixvalidator.h"

namespace SynGlyphX {

	PrefixSuffixValidator::PrefixSuffixValidator(QObject *parent)
		: QValidator(parent),
		m_prefix(""),
		m_suffix("")
	{

	}

	PrefixSuffixValidator::~PrefixSuffixValidator()
	{

	}

	void PrefixSuffixValidator::SetPrefix(const QString& prefix) {

		m_prefix = prefix;
	}

	const QString& PrefixSuffixValidator::GetPrefix() const {

		return m_prefix;
	}

	void PrefixSuffixValidator::SetSuffix(const QString& suffix) {

		m_suffix = suffix;
	}

	const QString& PrefixSuffixValidator::GetSuffix() const {

		return m_suffix;
	}

	QValidator::State PrefixSuffixValidator::validate(QString& input, int& pos) const {

		int prefixLength = m_prefix.length();
		int suffixLength = m_suffix.length();
		if (input.length() < (prefixLength + suffixLength)) {

			return QValidator::State::Invalid;
		}

		if ((prefixLength > 0) && (input.left(prefixLength) != m_prefix)) {

			return QValidator::State::Invalid;
		}

		if ((suffixLength > 0) && (input.right(suffixLength) != m_suffix)) {

			return QValidator::State::Invalid;
		}

		return QValidator::State::Acceptable;
	}

} //namespace SynGlyphX