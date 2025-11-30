#ifndef TODOLIST_AUTHMANAGER_H
#define TODOLIST_AUTHMANAGER_H

#include "SQLiteCpp/SQLiteCpp.h"
#include <string>

class AuthManager {
public:
    AuthManager(SQLite::Database& db);

    int login(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& password);

private:
    SQLite::Database& m_db;
    std::string hashPassword(const std::string& plainPassword);
};

#endif