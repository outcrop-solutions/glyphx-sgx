
#include "csvfilereader.h"
#include <string>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <boost/algorithm/string/trim.hpp>

namespace SynGlyphX {

	typedef boost::tokenizer< boost::escaped_list_separator<wchar_t>, std::wstring::const_iterator, std::wstring > Tokenizer;
	
	class CSVFileReader::impl
	{
	public:
		impl( const char* filename, wchar_t separator ) : m_separators( '\\', separator, '\"' )
		{
			m_filestream.open( filename );
			if ( m_filestream.fail() ) {

				throw std::runtime_error( "CSV file failed to open" );
			}

			//Headers are on the first line
			m_headers = GetValuesFromLine(false);
			if ( m_headers.empty() ) {

				throw std::runtime_error( "CSV file has no headers" );
			}
		}

		~impl()
		{
			Close();
		}

		CSVFileHandler::CSVValues GetValuesFromLine( bool trim ) {
			if ( m_filestream.eof() ) {

				throw std::runtime_error( "CSV file is at its end" );
			}

			std::wstring buffer, line;
			bool insideQuotes = false;

			//there could be line breaks in the middle of a field so if there is merge the lines
			do {

				std::getline( m_filestream, buffer );
				size_t lastQuotePosition = buffer.find_first_of( '"' );
				while ( lastQuotePosition != std::string::npos ) {

					insideQuotes = !insideQuotes;
					lastQuotePosition = buffer.find_first_of( '"', lastQuotePosition + 1 );
				}

				line.append( buffer );

				if ( insideQuotes ) {

					line.append( L"\n" );
				}
			} while ( insideQuotes );

			CSVFileHandler::CSVValues values;
			Tokenizer csvTokenizer( line, m_separators );
			values.assign( csvTokenizer.begin(), csvTokenizer.end() );

			if ( trim ) {

				for ( std::wstring& value : values ) {

					boost::algorithm::trim( value );
				}
			}

			return values;
		}

		void Close() {
			if ( m_filestream.is_open() ) {

				m_filestream.close();
			}
		}

		const CSVFileHandler::CSVValues& GetHeaders() const {
			return m_headers;
		}

		bool IsAtEndOfFile() const {
			return ( m_filestream.eof() );
		}

	private:
		boost::escaped_list_separator<wchar_t> m_separators;
		CSVFileHandler::CSVValues m_headers;
		std::wifstream m_filestream;
	};

	CSVFileReader::CSVFileReader(const char* filename, wchar_t separator) : pimpl( new impl( filename, separator ) )
	{
	}


	CSVFileReader::~CSVFileReader()
	{
		pimpl->Close();
	}

	const CSVFileHandler::CSVValues& CSVFileReader::GetHeaders() const {
		return pimpl->GetHeaders();
	}

	CSVFileHandler::CSVValues CSVFileReader::GetValuesFromLine(bool trim) {
		return pimpl->GetValuesFromLine( trim );
	}

	bool CSVFileReader::IsAtEndOfFile() const {
		return pimpl->IsAtEndOfFile();
	}

	void CSVFileReader::Close() {
		pimpl->Close();
	}

}
