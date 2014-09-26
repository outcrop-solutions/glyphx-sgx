/* This file is part of QSqlPlaintextDriver.
 *
 * QSqlPlaintextDriver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * 
 * QSqlPlaintextDriver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with QSqlPlaintextDriver.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <csvresult.hpp>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtCore/QString>

CSVResult::CSVResult( const QSqlDriver* driver ) : QSqlResult( driver )  {
	CSVDriver *drv	= (CSVDriver*)driver;
	csvFilename		= drv->getCsvFileName();
	contents		= drv->getContents();
}

QVariant CSVResult::data( int index ) {
	// TODO: Make delimiter variable
	// at() returns the current data record (the internal pointer)
	// after split( ";" ) the at returns the value at the column
	// pointed to by index
	
	QStringList fields = contents->at( at() ).split( ";" );
	
	if ( index > -1 && index < fields.size() ) {
		return fields.at( index );
	} else {
		setLastError(
				QSqlError(
					"Unable to return field value: Index out of range",
					"[CSVDRIVER]",
					QSqlError::UnknownError,
					200
					)
				);
		return QVariant( "<undef>" );
	}
}

bool CSVResult::isNull( int index ) {
    return false;
}

bool CSVResult::reset( const QString& query ) {
	QString data;
	int space = query.indexOf( " " );

	if ( space > -1 ) {
		data = query.mid( space+1 );
	}

	// select moves the cursor to index -1 so that a call to
	// fetchNext will return the first record at index 0
	if ( query.startsWith( "select" ) ) {
		if ( at() > -1 ) {
			setAt( -1 );
		}
		setSelect( true );
		setActive( true );

		return true;
	} else

	// data is being appended to the contents stringlist
	if ( query.startsWith( "insert" ) ) {
		contents->append( data );
		setActive( true );
		return true;
	} else

	// the commit command writes the in-memory stringlist to the
	// underlying file.
	// if payload to commit is not empty take that as the output
	// filename to write the data back to
	if ( query.startsWith( "commit" ) ) {
		QFile *f = new QFile( !data.isEmpty() ? data : csvFilename );
		if ( !f->open( QIODevice::WriteOnly | QIODevice::Text ) ) {
			setLastError(
					QSqlError(
						"Unable to commit: The CSV file could not be openend for write.",
						"[CSVDRIVER]",
						QSqlError::ConnectionError,
						102
						)
					);
			return false;
		}

		foreach( QString buffer, *contents ) {
			f->write( buffer.toLatin1() + "\n" );
		}

		f->close();
		delete f;

		return true;
	} else

	// delete the item specified at the index
	if ( query.startsWith( "delete" ) ) {
		// if no argument given, return false
		if ( data.isEmpty() ) {
			setLastError(
					QSqlError(
						"No delete index supplied",
						"[CSVDRIVER]",
						QSqlError::StatementError,
						101
						)
					);
			return false;
		}

		// try cast to real int from string
		int index = data.toInt();

		// if cast fails it is 0 but doesn't harm due to
		// following constraint check
		if ( index > -1 && index < contents->size() ) {

			// if within range do remove
			contents->removeAt( index );
			return true; 
		} else {
			setLastError(
					QSqlError(
						"Delete index out of valid range: " + QString::number( index ),
						"[CSVDRIVER]",
						QSqlError::StatementError,
						100
						)
					);
			return false;
		}
	} else {
		// return false on non-implemented commands
		// set last SQL error to QSqlError::StatementError
		setLastError(
				QSqlError(
					"Command not implemented: " + query,
					"[CSVDRIVER]",
					QSqlError::StatementError,
					43
					)
				);
		return false;
	}
}

bool CSVResult::fetch( int index ) {

	// return false if the size of the data container
	// is zero or has not been initialized
	if ( contents->size() == 0 ) {
		return false;
	}

	// position the internal pointer to the index
	// after constraint check
	if ( index >= 0 && index < contents->size()-1 ) {
		setAt( index );
		return true;
	} else {
		return false;
	}
}

bool CSVResult::fetchFirst() {
	setAt( 0 );
	return true;
}

bool CSVResult::fetchNext() {
	// abort if data pool is empty
	if ( contents->size() == 0 ) {
		return false;
	}

	// advance pointer if boundary check is OK
	if ( at() < contents->size()-1 && contents->size() > 0 ) {
		setAt( at()+1 );
		return true;
	} else {
		return false;
	}
}

bool CSVResult::fetchLast() {
	setAt( contents->size()-1 );
	return true;
}

int CSVResult::size() {
	return contents->size();
}

int CSVResult::numRowsAffected() {
	// TODO: Implement me!
	return 0;
}

QSqlRecord CSVResult::record() {
	// TODO: Implement me! (Not needed actually)
    return QSqlRecord();
}

