//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QScrollArea>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}


int main(int argc, char *argv[]) {

    // 检查 Qt 是否正常工作
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // 创建 Qt 应用程序
    QApplication app(argc, argv);

    // 收集文件夹中的所有视频
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn(std::string(argv[1]));

    if (videos.empty()) {
        QMessageBox::information(
            nullptr,
            "Tomeo",
            "No videos found! Add command line argument to \"quoted\" file location.");
        exit(-1);
    }

    // 视频播放窗口
    QVideoWidget *videoWidget = new QVideoWidget;

    // 视频播放器
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);
    videoWidget->show();

    // 将视频信息和按钮列表关联到播放器
    std::vector<TheButton*> buttons;
    player->setContent(&buttons, &videos);

    // 滚动区域设置
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumHeight(175); // 设置滚动区域的最小高度

    // 按钮容器和布局
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10); // 按钮间距
    buttonLayout->setContentsMargins(10, 0, 10, 0); // 边距
    buttonWidget->setLayout(buttonLayout);

    // 创建按钮并关联功能
    for (size_t i = 0; i < videos.size(); i++) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)),
                        player, SLOT(jumpTo(TheButtonInfo*)));
        buttons.push_back(button);
        buttonLayout->addWidget(button);
        button->init(&videos.at(i));

        // 获取视频时长
        QMediaPlayer *tempPlayer = new QMediaPlayer;
        tempPlayer->setMedia(*videos.at(i).url);
        QEventLoop loop;
        QObject::connect(tempPlayer, &QMediaPlayer::durationChanged,
                         [&](qint64 duration) {
                             videos.at(i).duration = duration;
                             button->setDuration(duration);
                             loop.quit();
                         });
        tempPlayer->play();
        loop.exec();
        delete tempPlayer;
    }

    // Play
    player->setMedia(*videos[0].url);
    player->play();

    // 将按钮容器添加到滚动区域
    scrollArea->setWidget(buttonWidget);

    // 创建主窗口和主布局
    QWidget window;
    QVBoxLayout *mainLayout = new QVBoxLayout();
    window.setLayout(mainLayout);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // 将视频播放窗口和滚动按钮区域添加到主布局
    mainLayout->addWidget(videoWidget);   // 播放窗口在上
    mainLayout->addWidget(scrollArea);    // 按钮区域在下

    // 显示主窗口
    window.show();

    // 等待程序退出
    return app.exec();
}
