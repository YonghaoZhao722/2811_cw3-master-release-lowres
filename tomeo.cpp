//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//
#include "mainwindow.h"
#include <QFont>



int main(int argc, char *argv[])
{
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    QApplication app(argc, argv);
    QFont appFont("Times New Roman", 12);  // 字体名称和字号
    QApplication::setFont(appFont);

    MainWindow window;
    // if (argc == 2) {
    //     window.loadVideos(std::string(argv[1]));
    // } else {
    //     window.loadVideos("");  // 或者直接调用 window.loadVideos();
    // }

    window.show();

    return app.exec();
}
