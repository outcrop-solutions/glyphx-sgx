#ifndef __csvresult_hpp
#define __csvresult_hpp

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

#include "sgxqtdbdrivers_global.h"

#include <QtCore/QStringList>
#include <QtSql/QSqlResult>

#include "sqlcsvdriver.hpp"

class SGXQTDBDRIVERS_EXPORT CSVResult : public QSqlResult {
	public:

		/** CSVResult Implementation.
		 * We need the driver instance here because the file handle is stored there.
		 * @param driver The QSqlDriver Object
		 */
		CSVResult( const QSqlDriver* driver );

	protected:

		/** Return the data item at the requested index
		 * @param index the index of the column
		 * @return QVariant the item at the requested list index
		 */
		QVariant data( int index );

		/** Check if the value at given field index is null.
		 * @return boolean true if the value is null, otherwise false
		 */
		bool isNull( int );

		/** Perform the query.
		 * The following pseudo SQL commands have been implemented:
		 * All commands are case sensitive.
		 *
		 * select: sets the internal pointer before the first entry
		 * insert: replace the special ${AUTO} variable with the new auto increment value
		 * commit: write changes to disk
		 * delete: delete the current entry
		 *
		 * @param query the query string
		 * @return bool
		 */
		bool reset( const QString& query );

		/** Position the pointer to index
		 * @return true on success otherwise false
		 */
		bool fetch( int );

		/** Position the pointer to the first record
		 * @return true on success otherwise false
		 */
		bool fetchFirst();

		/** Advance the pointer to the next record
		 * @return true on success otherwise false
		 */
		bool fetchNext();

		/** Position pointer to last record
		 * @return true on success otherwise false
		 */
		bool fetchLast();

		/** Return the size of the underlying data pool
		 * @return size
		 */
		int size();

		/** Return the number of rows affected by the last action
		 * @return number of rows affected
		 */
		int numRowsAffected();

		/** Return a QSqlRecord
		 * @return QSqlRecord the record
		 */
		QSqlRecord record();

	private:
		//QFile *f;
		QString csvFilename;
		QStringList *contents;
};
#endif

