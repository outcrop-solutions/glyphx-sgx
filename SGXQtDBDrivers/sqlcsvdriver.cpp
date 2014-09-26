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

#include <sqlcsvdriver.hpp>
#include <QtSql/QSqlError>

bool CSVDriver::hasFeature( DriverFeature feature ) const {
	switch ( feature ) {
		case QSqlDriver::BLOB:
		case QSqlDriver::PositionalPlaceholders:
			return false;
		case QSqlDriver::Unicode:
		case QSqlDriver::Transactions:
		case QSqlDriver::QuerySize:
			return true;
		default:
			return false;
	}
}

bool CSVDriver::open(
		const QString& newDb,
		const QString& newUser = "nobody",
		const QString& newPassword = "anonymous",
		const QString& newHost = "localhost",
		int newPort = -1,
		const QString& newOptions = "nop"
		) {

	csvFilename		= newDb;
	user			= newUser;
	password		= newPassword;
	host			= newHost;
	options			= newOptions;
	port			= newPort;

	f = new QFile( csvFilename );
	contents = new QStringList();

	if ( !f->open( QIODevice::ReadWrite | QIODevice::Text ) ) {
		setLastError(
				QSqlError(
					"Unable to open Backingstore: " + csvFilename,
					"[CSVDRIVER]",
					QSqlError::ConnectionError,
					42
					)
				);
		return false;
	}

	// get file from driver and read all lines into memory
	// what happens to be a stringlist
	while ( !f->atEnd() ) {
		// append to stringlist
		contents->append( QString( f->readLine() ).trimmed() );
	}
	f->close();
	//f->seek( 0 );

	// set the necessary flags
	setOpen( true );
	setOpenError( false );

	return true;
}

void CSVDriver::close() {
	if ( isOpen() ) {
		//f->close();
		delete contents;
		delete f;

		setOpen( false );
		setOpenError( false );
	}
}

QSqlResult* CSVDriver::createResult() const {
	return new CSVResult( this );
}

