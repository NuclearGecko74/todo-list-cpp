#include "SQLiteCpp/SQLiteCpp.h"
#include <string>

class AuthManager {
private:
    SQLite::Database& m_db; // Referencia a la BD abierta

    // Función privada para hashear contraseñas
    std::string hashPassword(const std::string& plainPassword);

public:
    AuthManager(SQLite::Database& db);

    /**
     * Comprueba las credenciales.
     * Devuelve el 'userID' si tiene éxito, o -1 si falla.
     */
    int login(const std::string& username, const std::string& password);

    /**
     * Registra un nuevo usuario.
     * Devuelve 'true' si tiene éxito.
     */
    bool registerUser(const std::string& username, const std::string& password);
};