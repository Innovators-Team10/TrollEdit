#include <QApplication>
#include <QAction>
#include "main_window.h"
#include <QString>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
    int luaopen_lpeg (lua_State *L);
}

static MainWindow* window;

void loadConfig(lua_State *L, const char *fname, int *w, int *h, QString *style) {
    if (luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0))
        qDebug() << "cannot run config. file: " <<  lua_tostring(L, -1);
    lua_getglobal(L, "style");
    lua_getglobal(L, "width");
    lua_getglobal(L, "height");
    if (!lua_isstring(L, -3))
        qDebug() << "'style' should be a string\n";
    if (!lua_isnumber(L, -2))
        qDebug() << "'width' should be a number\n";
    if (!lua_isnumber(L, -1))
        qDebug() << "'height' should be a number\n";
    *style = lua_tostring(L, -3);
    *w = lua_tointeger(L, -2);
    *h = lua_tointeger(L, -1);
}

static int setstyle(lua_State *L) {
    QString str = lua_tostring(L, 1); /* get argument */
    window->setStyleSheet(str);
    return 0;                         /* number of results in LUA*/
}

static void stackDump (lua_State *L)
{
    int i;
    int top = lua_gettop(L);

    qDebug("-------STACK--------|");
    for (i = 1; i <= top; i++) { /* repeat for each level */
        int t = lua_type(L, i);
        switch (t)
        {
            case LUA_TSTRING:
            { /* strings */
                qDebug("%d. string: '%s'\t|", i, lua_tostring(L, i));
                break;
            }
            case LUA_TBOOLEAN:
            { /* booleans */
                qDebug("%d. %s\t|", i, lua_toboolean(L, i) ? "true" : "false");
                break;
            }
            case LUA_TNUMBER:
            { /* numbers */
                qDebug("%d. numbers %g\t|", i, lua_tonumber(L, i));
                break;
            }
            case LUA_TFUNCTION:
            { /* numbers */
                qDebug("%d. function %s\t|", i, lua_tostring(L, i) );
                break;
            }
            default:
            { /* other values*/
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
    QApplication app(argc, argv);
    app.setOrganizationName("Innovators");
    app.setApplicationName("TrollEdit");
    app.setStartDragDistance(app.startDragDistance() * 2);

    // set splashScreen
    QPixmap pixmap(":/splash");
    QSplashScreen splashScreen(pixmap,Qt::WindowStaysOnTopHint);

    // find the directory of the program
    QFileInfo program(argv[0]);
    QString path = program.absoluteDir().path();

    MainWindow w(path);
    w.setWindowOpacity(0);

    // Load config from config_app.lua
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    int width, height; QString style;
    const QString CONFIG_DIR = "/../share/trolledit";
    QDir dir = QDir(QApplication::applicationDirPath() + CONFIG_DIR);
    //QFileInfoList configs = dir.entryInfoList(QStringList("*.lua"), QDir::Files | QDir::NoSymLinks);
    QFileInfo configFile(dir.absolutePath()+ QDir::separator() + "config_app.lua");

    window = reinterpret_cast<MainWindow*>(&w);
    lua_register(L, "setstyle", setstyle);
    loadConfig(L, qPrintable(configFile.absoluteFilePath()), &width, &height, &style);

    qDebug() << configFile.absoluteFilePath() << "width: " << width << " height: " << height << "\n style: " << style;
    //window size
    w.resize(width, height);
    //CSS style
    //w.setStyleSheet(style);

    //w.setStyleSheet();
    splashScreen.show();
    w.show();

    QTimer::singleShot(2000, &splashScreen, SLOT(close()));
    QTimer::singleShot(1000, &w, SLOT(wInit()));
    // open all files given as parameters

    //    w.newFile();
    //    w.open("../input/in.c"); // TEMP
    // open all files given as parameters
    for (int i = 1; i < argc; i++)
        w.open(argv[i]);

    return app.exec();
}

void MainWindow::wInit()
 {
    setWindowOpacity(1);
 }
