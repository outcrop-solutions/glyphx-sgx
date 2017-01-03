#include "csvfilewriter.h"
#include <stdexcept>
#include <codecvt>
#include <locale>
#include <fstream>

namespace SynGlyphX {

	class CSVFileWriter::impl
	{
	public:
		impl( wchar_t _sep ) : m_numFields(0), m_separator( _sep ) { }

		void SetupLocale()
		{
			std::locale newLocale = std::locale( std::locale(), new std::codecvt_utf8_utf16<wchar_t>() );
			m_filestream.imbue( newLocale );
		}

		void Open( const char* filename )
		{
			m_filestream.open( filename, std::ofstream::out | std::ofstream::trunc );
			if ( m_filestream.fail() ) {

				throw std::runtime_error( "CSV file failed to open" );
			}
			SetupLocale();
		}

		bool IsOpen() const {

			return m_filestream.is_open();
		}

		void WriteLine( const CSVFileHandler::CSVValues& values )
		{
			if ( !m_filestream.is_open() ) {

				throw std::runtime_error( "CSVFileWriter can't write line since file isn't open" );
			}

			if ( values.empty() ) {

				throw std::invalid_argument( "CSVFileWriter needs at least one value to write a line" );
			}

			if ( m_numFields == 0 ) {

				m_numFields = values.size();
			}
			else if ( m_numFields != values.size() ) {

				throw std::invalid_argument( "CSVFileWriter needs to have the same number of values as headers to write a line" );
			}

			m_filestream << values[0];
			for ( unsigned int i = 1; i < m_numFields; ++i ) {

				m_filestream << m_separator << values[i];
			}
			m_filestream << std::endl;
		}

		void Close()
		{
			m_filestream.close();
		}

	private:
		std::wofstream m_filestream;
		unsigned int m_numFields;
		wchar_t m_separator;
	};

	CSVFileWriter::CSVFileWriter(wchar_t separator)
	{
		pimpl = new impl( separator );
	}

	CSVFileWriter::CSVFileWriter(const char* filename, wchar_t separator)
	{
		pimpl = new impl( separator );
		pimpl->Open(filename);
	}


	CSVFileWriter::~CSVFileWriter()
	{
		pimpl->Close();
		delete pimpl;
	}

	void CSVFileWriter::Open(const char* filename) {

		pimpl->Open( filename );
	}

	void CSVFileWriter::Close() {

		if (IsOpen()) {
			pimpl->Close();
		}
	}

	bool CSVFileWriter::IsOpen() const {

		return pimpl->IsOpen();
	}

	void CSVFileWriter::WriteLine(const CSVFileHandler::CSVValues& values) {
		pimpl->WriteLine( values );
	}

}
