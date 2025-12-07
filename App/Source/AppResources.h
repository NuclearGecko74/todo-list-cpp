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

private:
    static Font s_Font;
    static std::unique_ptr<SQLite::Database> s_Database;

    static void InitDatabase();
};



#endif
