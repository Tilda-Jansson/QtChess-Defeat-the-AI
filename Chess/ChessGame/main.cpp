#include <QApplication>
#include <QScreen>
#include "Game.h"
//#include "StartMenuDialog.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Game *game = new Game();  // This is local to main, which is correct.

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - game->width()) / 2;
    int y = (screenGeometry.height() - game->height()) / 2;

    game->move(x, y);

    game->showStartMenu();

    return a.exec();
}
