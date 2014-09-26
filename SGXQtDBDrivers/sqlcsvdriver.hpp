#ifndef __sqlcsvdriver_hpp
#define __sqlcsvdriver_hpp

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

#include <QtCore/QFile>
#include <QtSql/QSqlDriver>

#include <csvresult.hpp>

class SGXQTDBDRIVERS_EXPORT CSVDriver : public QSqlDriver {
	public:
		/** Check for DriverFeature.
		 * As of now we support the features Unicode, Transactions and
		 * QuerySize. Not supported are BLOBs and PositionalPlaceholders.
		 * @param feature The driver feature to check
		 * @return true if the feature is supported otherwise false
		 */
		bool hasFeature( DriverFeature feature ) const;

		/** Open the database
		 * @param db Database Name (CSV-filename)
		 * @param user Username (not applicable)
		 * @param password (not applicable)
		 * @param host (not applicable)
		 * @param port (not applicable)
		 * @param options
		 * @return true on success, false on any failure
		 */
		bool open(
				const QString& db,
				const QString& user,
				const QString& password,
				const QString& host,
				int port,
				const QString& options
				);

		/** Close Database
		 */
		void close();

		/** Create a CSVResult Object.
		 * The CSVResult takes over from here. The entire CSVDriver object is
		 * passed to the default c'tor.
		 * From within the CSVResult we have access to the backing store (the
		 * filehandle as well as the preloaded contents) via the getFile and
		 * getContents accessors.
		 * @return A pointer to the created QSqlResult
		 */
		QSqlResult* createResult() const;

		/** Getter for the internal QFile Object
		 * @returns QFile*
		 */
		inline QFile* getFile() { return f; }

		/** Getter for the internal QStringlist
		 * @returns QStringList*
		 */
		inline QStringList* getContents() { return contents; }

		/** Getter for the CVS Filename
		 * @returns QString
		 */
		inline QString getCsvFileName() { return csvFilename; }

	private:
		QString csvFilename;
		QFile *f;
		QStringList *contents;

		// dummy
		QString user, password, host, options;
		int port;
};

#endif
