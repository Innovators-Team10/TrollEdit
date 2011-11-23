#include <QApplication>
#include <QAction>
#include "main_window.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static void stackDump (lua_State *L) {
int i;
int top = lua_gettop(L);
qDebug("-------STACK--------|");
for (i = 1; i <= top; i++) { /* repeat for each level */
int t = lua_type(L, i);
switch (t) {
case LUA_TSTRING: { /* strings */
qDebug("%d. string: '%s'\t|", i, lua_tostring(L, i));
break;
}
case LUA_TBOOLEAN: { /* booleans */
qDebug("%d. %s\t|", i, lua_toboolean(L, i) ? "true" : "false");
break;
}
case LUA_TNUMBER: { /* numbers */
qDebug("%d. numbers %g\t|", i, lua_tonumber(L, i));
break;
}
case LUA_TFUNCTION: { /* numbers */
qDebug("%d. function %s\t|", i, lua_tostring(L, i) );
break;
}
default: { /* other values*/
qDebug("%d. other %s\t|", i, lua_typename(L, t));
break;
}
}
// qDebug("--------------------|"); /* put a separator */
}
qDebug("");
}

int main(int argc, char *argv[])
{
    //Experimenty//
//    pokus.test=10;
//    printf("Qt pokus %d\n", pokus.test);
    /*

    POKUS *ff;
    lua_State *L;
    int x = 5;
    L = luaL_newstate();
    luaL_openlibs(L);
    int status = luaL_dofile(L, "C:\\Test\\test.lua");
    lua_getglobal(L,"get");
    int result=lua_pcall(L,0,1,0);
    qDebug("%d",result);

    ff = (POKUS*) lua_topointer(L,-1);

    printf("Qt pokus %d\n", ff->test);
    stackDump(L);

    lua_close(L);
//    Sleep(4000);

//    stackDump(L);

    Sleep(10000);

    exit(1);
    //end of experimenty//


*/
    QApplication app(argc, argv);
    app.setOrganizationName("Ufopak");
    app.setApplicationName("TrollEdit");
    app.setStartDragDistance(app.startDragDistance() * 2);

    // find the directory of the program
    QFileInfo program(argv[0]);
    QString path = program.absoluteDir().path();

    MainWindow w(path);
    w.show();

//    w.newFile();
//    w.open("../input/in.c"); // TEMP

    // open all files given as parameters
    for (int i = 1; i < argc; i++)
        w.open(argv[i]);

    return app.exec();
}
