#include "AppResources.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

Font AppResources::s_Font = { 0 };
std::unique_ptr<SQLite::Database> AppResources::s_Database = nullptr;

void AppResources::Init()
{
    s_Font = LoadFontEx("assets/fonts/Inter-Regular.ttf", 76, 0, 0);
    SetTextureFilter(s_Font.texture, TEXTURE_FILTER_BILINEAR);

    GuiSetFont(s_Font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    try
    {
        InitDatabase();
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(std::string("Failed to init database: ") + e.what());
    }
}

void AppResources::Shutdown()
{
    UnloadFont(s_Font);
    s_Database.reset();
}

Font AppResources::GetFont()
{
    return s_Font;
}

SQLite::Database& AppResources::GetDatabase()
{
    return *s_Database;
}

void AppResources::InitDatabase()
{
    s_Database = std::make_unique<SQLite::Database>("RizzList.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    s_Database->exec("PRAGMA foreign_keys = ON;");

    // User Table
    s_Database->exec("CREATE TABLE IF NOT EXISTS user ("
            "userID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT NOT NULL UNIQUE, "
            "passwordHash TEXT NOT NULL, "
            "createDate DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");");

    // List Table
    s_Database->exec("CREATE TABLE IF NOT EXISTS list ("
            "listID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "userID INTEGER NOT NULL, "
            "name TEXT NOT NULL, "
            "description TEXT, "
            "createDate DATETIME DEFAULT CURRENT_TIMESTAMP, "
            "isDeleted INTEGER DEFAULT 0, "
            "FOREIGN KEY(userID) REFERENCES user(userID) ON DELETE CASCADE"
            ");");

    // Task Table
    s_Database->exec("CREATE TABLE IF NOT EXISTS task ("
            "taskID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "listID INTEGER NOT NULL, "
            "title TEXT NOT NULL, "
            "description TEXT, "
            "isDeleted INTEGER DEFAULT 0, "
            "status INTEGER DEFAULT 0, "
            "createDate DATETIME DEFAULT CURRENT_TIMESTAMP, "
            "dueDate DATETIME, "
            "FOREIGN KEY(listID) REFERENCES list(listID) ON DELETE CASCADE"
            ");");

    s_Database->exec("INSERT OR IGNORE INTO user (userID, username, passwordHash) VALUES (1, 'Tester', 'dummy_hash')");
}
