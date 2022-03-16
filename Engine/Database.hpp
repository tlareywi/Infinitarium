//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <sqlite3.h>

class Database {
public:
	~Database();

	typedef std::vector<std::vector<std::string>> ResultSet;

	static Database& instance();
	static void destroy();

	ResultSet execute(const std::string& query);

private:
	Database();
	static Database* dbInstance;

	static int callback(void* data, int argc, char** argv, char** azColName);

	sqlite3* db;
};