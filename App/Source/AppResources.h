#ifndef APPRESOURCES_H
#define APPRESOURCES_H

#include "raylib.h"
#include <SQLiteCpp/SQLiteCpp.h>

class AppResources {
public:
    static void Init();
    static void Shutdown();
    static Font GetFont();
    static SQLite::Database& GetDatabase();
    static int GetUserId();
    static void SetUserId(const int userId);
    static std::string GetUsername();
    static void SetUsername(const std::string& username);

    static bool LoadSession();
    static void SaveSession(int userId);
    static void ClearSession();

private:
    static Font s_Font;
    static std::unique_ptr<SQLite::Database> s_Database;
    static int s_UserId;
    static std::string s_Username;

    static void InitDatabase();
};



#endif
