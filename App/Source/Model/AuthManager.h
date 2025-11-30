#include "SQLiteCpp/SQLiteCpp.h"
#include <string>

class AuthManager {
private:
    SQLite::Database& m_db; 
    std::string hashPassword(const std::string& plainPassword);

public:
    AuthManager(SQLite::Database& db);

    int login(const std::string& username, const std::string& password);

    bool registerUser(const std::string& username, const std::string& password);
};

