//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#include "Database.hpp"
#include "Application.hpp"

Database* Database::dbInstance = nullptr;

Database::Database() : db(nullptr) {
	int rc;

	rc = sqlite3_open((IApplication::Create()->getInstallationRoot() + "/share/Infinitarium/infinitarium.db").c_str(), &db);

	if (rc) {
		const std::string str("Fatal: Database exception, ");
		std::exception e( (str + sqlite3_errmsg(db)).c_str() );
		throw(e);
	}
}

Database::~Database() {
	sqlite3_close(db);
}

Database& Database::instance() {
	if (!dbInstance) {
		dbInstance = new Database;
	}

	return *dbInstance;
}

void Database::destroy() {
	delete dbInstance;
	dbInstance = nullptr;
}

int Database::callback(void* data, int argc, char** argv, char** azColName) {
	ResultSet* resultSet = reinterpret_cast<ResultSet*>(data);

	std::vector<std::string> cols;
	cols.reserve(10);
	for (unsigned int i = 0; i < argc; i++) {
		cols.push_back(argv[i] ? argv[i] : "NULL");
	}

	resultSet->push_back(std::move(cols));

	return 0;
}

Database::ResultSet Database::execute( const std::string& query ) {
	ResultSet resultSet;
	resultSet.reserve(100);

	if (query.empty())
		return resultSet;

	char* zErrMsg = nullptr;
	int rc = sqlite3_exec(db, query.c_str(), callback, reinterpret_cast<void*>(&resultSet), &zErrMsg);

	if (rc != SQLITE_OK) {
		std::cerr << "Warning: SQL error, " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		zErrMsg = nullptr;
	}

	// Move, no copy
	return resultSet;
}