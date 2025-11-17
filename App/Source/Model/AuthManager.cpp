#include "AuthManager.h"

#include <iostream> // Para std::cerr (ver notas en 'catch')
#include <string>

AuthManager::AuthManager(SQLite::Database& db) : m_db(db) {}

std::string AuthManager::hashPassword(const std::string& plainPassword) {
    size_t hash = 5381;
    for (char c : plainPassword) {
        hash = ((hash << 5) + hash) + static_cast<size_t>(c);
    }
    return std::to_string(hash);
}

int AuthManager::login(const std::string& username, const std::string& password) {
    try {
        // [Configuración]: Ajusta esta consulta SQL con los nombres de tu tabla y columnas.
        // Cambia 'user', 'userID', 'passwordHash', y 'username' si los tuyos se llaman diferente.
        SQLite::Statement query(m_db, "SELECT userID, passwordHash FROM user WHERE username = ?");

        query.bind(1, username);

        if (query.executeStep()) {
            // [Configuración]: Asegúrate que el nombre de columna 'passwordHash' es correcto.
            std::string dbHash = query.getColumn("passwordHash");
            std::string inputHash = hashPassword(password);

            if (dbHash == inputHash) {
                // [Configuración]: Asegúrate que el nombre de columna 'userID' es correcto.
                return query.getColumn("userID");
            }
        }
    }
    catch (std::exception& e) {
        // [Acción]: Esta línea imprime el error a la consola.
        // Para una app de escritorio, querrás reemplazar esto por un sistema de 'Logging' 
        // o una función que muestre un mensaje de error en la UI (ej. "Error en BD").
        std::cerr << "Error en login: " << e.what() << std::endl;
    }

    return -1; // Fracaso
}

bool AuthManager::registerUser(const std::string& username, const std::string& password) {
    try {
        // [Configuración]: Ajusta esta consulta SQL con los nombres de tu tabla y columnas.
        // Cambia 'user', 'username', 'passwordHash', 'createTime' si los tuyos se llaman diferente.
        SQLite::Statement query(m_db, "INSERT INTO user (username, passwordHash, createTime) VALUES (?, ?, DATETIME('now'))");

        query.bind(1, username);
        query.bind(2, hashPassword(password));

        query.exec();
        return true; // Éxito

    }
    catch (std::exception& e) {
        // [Acción]: Esta línea imprime el error a la consola (ej. "Usuario ya existe").
        // Para una app de escritorio, querrás reemplazar esto por un 'Logging'
        // o una función que muestre un mensaje al usuario (ej. "El nombre de usuario ya está en uso").
        std::cerr << "Error al registrar: " << e.what() << std::endl;
        return false; // Fracaso
    }
}