#include "AuthManager.h"

#include <iostream> 
#include <string>

AuthManager::AuthManager(SQLite::Database& db) : m_db(db)
{

}

std::string AuthManager::hashPassword(const std::string& plainPassword)
{
    size_t hash = 5381;
    for (const auto c : plainPassword)
        {
        hash = ((hash << 5) + hash) + static_cast<size_t>(c);
    }
    return std::to_string(hash);
}

int AuthManager::login(const std::string& username, const std::string& password)
{
    try {
        SQLite::Statement query(m_db, "SELECT userID, passwordHash FROM user WHERE username = ?");

        query.bind(1, username);

        if (query.executeStep())
            {
            std::string dbHash = query.getColumn("passwordHash");
            std::string inputHash = hashPassword(password);

            if (dbHash == inputHash) {
                return query.getColumn("userID");
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error en login: " << e.what() << std::endl;
    }

    return -1;
}

bool AuthManager::registerUser(const std::string& username, const std::string& password)
{
    try
    {
        SQLite::Statement query(m_db, "INSERT INTO user (username, passwordHash, createTime) VALUES (?, ?, DATETIME('now'))");

        query.bind(1, username);
        query.bind(2, hashPassword(password));

        query.exec();
        return true; 

    }
    catch (std::exception& e)
    {
        std::cerr << "Error al registrar: " << e.what() << std::endl;
        return false; 
    }
}
