#include "keywordfilter.h"


KeywordFilter::KeywordFilter() :
	m_keyword(""),
	m_caseSensitive(false),
	m_exactMatch(false)
{
}

KeywordFilter::KeywordFilter(const KeywordFilter& filter) :
	m_keyword(filter.m_keyword),
	m_caseSensitive(filter.m_caseSensitive),
	m_exactMatch(filter.m_exactMatch) {

}

KeywordFilter::~KeywordFilter()
{
}

KeywordFilter& KeywordFilter::operator=(const KeywordFilter& filter) {

	m_keyword = filter.m_keyword;
	m_caseSensitive = filter.m_caseSensitive;
	m_exactMatch = filter.m_exactMatch;

	return *this;
}

bool KeywordFilter::operator==(const KeywordFilter& filter) const {

	if (m_keyword != filter.m_keyword) {

		return false;
	}
	
	if (m_caseSensitive != filter.m_caseSensitive) {

		return false;
	}

	return (m_exactMatch == filter.m_exactMatch);
}

bool KeywordFilter::operator!=(const KeywordFilter& filter) const {

	return !operator==(filter);
}

void KeywordFilter::SetKeyword(const QString& keyword) {

	m_keyword = keyword;
}

const QString& KeywordFilter::GetKeyword() const {

	return m_keyword;
}

void KeywordFilter::SetCaseSensitive(bool caseSensitive) {

	m_caseSensitive = caseSensitive;
}

bool KeywordFilter::GetCaseSensitive() const {

	return m_caseSensitive;
}

void KeywordFilter::SetExactMatch(bool exactMatch) {

	m_exactMatch = exactMatch;
}

bool KeywordFilter::GetExactMatch() const {

	return m_exactMatch;
}