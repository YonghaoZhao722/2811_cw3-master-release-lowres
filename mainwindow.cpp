#include "mainwindow.h"
#include "progress_bar_widget.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImageReader>
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
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("QMainWindow { background-color: #1e1e1e; }");
    setupUi();
    loadVideos("F:\\2811_cw3-master-release-lowres\\videos");
    setupPlayer();
    createButtons();
    setupProgressBar();
    setupControlButtons();


    // 开始播放第一个视频
    if (!videos.empty()) {
        qDebug() << "not empty ";
        player->setMedia(*videos[0].url);
        player->play();
    }
    else{
        qDebug()<<"empty";
    }
}

MainWindow::~MainWindow()
{
    // delete ui;
}

void MainWindow::setupUi()
{
    setWindowTitle("tomeo");
    setMinimumSize(800, 680);

    // 设置视频窗口的样式
    ui->videoWidget->setStyleSheet("QVideoWidget { background-color: black; }");
    setStyleSheet("background-color: black;");

}

void MainWindow::loadVideos(const std::string& path)
{
    qDebug() << "Loading videos from path:" << QString::fromStdString(path);
    if (!path.empty()) {
        videos = getInfoIn(path);
        qDebug() << "Number of videos found:" << videos.size();
    }

    if (videos.empty()) {
        QMessageBox::information(
            this,
            "Tomeo",
            "No videos found! Add command line argument to \"quoted\" file location.");
        exit(-1);
    }
}

void MainWindow::setupPlayer()
{
    player = new ThePlayer;
    player->setVideoOutput(ui->videoWidget);
    player->setContent(&buttons, &videos);

    // 设置视频窗口的背景
    ui->videoWidget->setAttribute(Qt::WA_NoSystemBackground);

    // // 如果视频是上下颠倒的，添加以下代码
    // QTransform transform;
    // transform.rotate(180);
    // ui->videoWidget->setTransform(transform);
}

void MainWindow::setupProgressBar()
{
    ui->progressBar->setFixedHeight(30);
    ui->progressBar->setStyleSheet(
        "QWidget { background: transparent; }"  // 设置整体背景透明
        "QSlider::groove:horizontal {"
        "    background: #ddd;"
        "    height: 8px;"
        "    border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: #666;"
        "    width: 20px;"
        "    margin: -4px 0;"
        "    border-radius: 8px;"
        "}"
        "QLabel {"  // 时间标签样式
        "    color: white;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        );
    player->setProgressBar(ui->progressBar);
}


void MainWindow::createButtons() {
    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(8);

    for (int i = 0; i < videos.size(); i++) {
        // 创建一个整体的容器
        QWidget* buttonWidget = new QWidget();
        buttonWidget->setObjectName("videoItem");
        buttonWidget->setCursor(Qt::PointingHandCursor);
        buttonWidget->installEventFilter(this);

        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(5, 5, 5, 5);
        buttonLayout->setSpacing(2);

        // 缩略图按钮
        TheButton* button = new TheButton(buttonWidget);
        button->setIconSize(QSize(160, 90));  // 设置基础大小
        button->setObjectName("thumbnailButton");
        // 设置大小策略为可扩展但保持宽高比
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, &TheButton::jumpTo, player, &ThePlayer::jumpTo);
        button->init(&videos.at(i));
        buttons.push_back(button);

        // 右侧信息容器
        QWidget* infoWidget = new QWidget();
        infoWidget->setObjectName("infoWidget");
        QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setSpacing(10);

        // 文件名标签
        QLabel* nameLabel = new QLabel(videos[i].filename);
        nameLabel->setObjectName("nameLabel");
        nameLabel->setWordWrap(true);  // 允许文字换行

        // 设置自动调整字体大小
        QFont font = nameLabel->font();
        font.setPointSize(25);  // 设置基础字体大小
        nameLabel->setFont(font);

        infoLayout->addWidget(nameLabel);

        infoLayout->addStretch();

        // 设置按钮和信息区域的比例
        buttonLayout->addWidget(button, 1);  // 缩略图占比更大
        buttonLayout->addWidget(infoWidget, 2);  // 信息区域占比较小

        buttonWidget->setProperty("buttonIndex", i);

        layout->addWidget(buttonWidget);
    }

    layout->addStretch();
    container->setLayout(layout);

    QString styleSheet = R"(
        QWidget#videoItem {
            background-color: #2b2b2b;
            border-radius: 4px;
            padding: 0px;
            margin: 0px;
            min-height: 100px;

        }

        QWidget#videoItem:hover {
            background-color: #3b3b3b;
        }

        QWidget#videoItem:hover #nameLabel {
            color: #ffffff;
            font-weight: bold;
        }

        QLabel#nameLabel {
            color: #e0e0e0;
            padding: 0px;
            margin: 0px;
            min-width: 50px;
        }


        QPushButton#thumbnailButton {
            border: none;
            padding: 0px;
            background: transparent;
            min-width: 120px;
            max-width: 240px;
            min-height: 68px;
            max-height: 135px;
        }

        QWidget#infoWidget {
            background: transparent;
            padding: 0px;
            margin: 0px;
            min-width: 50px;
            max-width: 30%;  /* 限制最大宽度 */
        }

        QScrollArea {
            border: none;
            width: 50%;
            background-color: black;
        }

    )";

    container->setStyleSheet(styleSheet);
    ui->scrollArea->setWidget(container);
    ui->scrollArea->viewport()->installEventFilter(this);

    // 设置容器的大小策略
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateFontSizes();
}

void MainWindow::updateFontSizes() {
    int viewportWidth = ui->scrollArea->viewport()->width();

    // 计算动态字体大小
    int nameFontSize = qMax(8, viewportWidth / 50);  // 最小8pt


    // 更新所有标签的字体大小
    QList<QLabel*> nameLabels = ui->scrollArea->findChildren<QLabel*>("nameLabel");
    for (QLabel* label : nameLabels) {
        QFont font = label->font();
        font.setPointSize(nameFontSize);
        label->setFont(font);
    }

}

void MainWindow::closeEvent(QCloseEvent* event) {
    QApplication::quit();  // 退出整个程序
    event->accept();       // 接受关闭事件
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonRelease) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            bool ok;
            int index = widget->property("buttonIndex").toInt(&ok);
            if (ok && index >= 0 && index < buttons.size()) {
                // 触发相应按钮的 jumpTo 信号
                buttons[index]->clicked();
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}


//=================视频播放控制按钮=================
void MainWindow::setupControlButtons() {
    QWidget* controlWidget = new QWidget();
    QVBoxLayout* vLayout = new QVBoxLayout(controlWidget);
    vLayout->setContentsMargins(0, 0, 0, 10);

    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(10);

    // Initialize buttons
    prevButton = new QPushButton();
    prevButton->setIcon(QIcon(":/icons/previous.svg"));

    skipBackButton = new QPushButton();
    skipBackButton->setIcon(QIcon(":/icons/rewind.svg"));

    playPauseButton = new QPushButton();
    playPauseButton->setIcon(QIcon(":/icons/play.svg"));

    skipForwardButton = new QPushButton();
    skipForwardButton->setIcon(QIcon(":/icons/fast-forward.svg"));

    nextButton = new QPushButton();
    nextButton->setIcon(QIcon(":/icons/next.svg"));

    speedButton = new QPushButton("1.0x");

    // Style buttons
    QString buttonStyle = "QPushButton { "
                          "  background-color: #2b2b2b; "
                          "  border: none; "
                          "  padding: 8px; "  // 减小padding
                          "  border-radius: 6px; "
                          "  margin: 0px; "   // 添加margin: 0
                          "} "
                          "QPushButton:hover { background-color: #3b3b3b; }";

    player->getVolumeButton()->setStyleSheet(buttonStyle);
    player->getVolumeButton()->setFixedSize(50, 50);
    player->getVolumeButton()->setIconSize(QSize(30, 30));

    // 为音量滑块添加样式
    QString sliderStyle = "QSlider::groove:horizontal {"
                          "  height: 3px;"
                          "  background: #4b4b4b;"
                          "  margin: 2px 0;"
                          "}"
                          "QSlider::handle:horizontal {"
                          "  background: white;"
                          "  width: 12px;"
                          "  margin: -4px 0;"
                          "  border-radius: 6px;"
                          "}";
    player->getVolumeSlider()->setStyleSheet(sliderStyle);
    player->getVolumeSlider()->setFixedWidth(120);  // 调整滑块宽度

    QList<QPushButton*> buttons = {prevButton, skipBackButton, playPauseButton,
                                    skipForwardButton, nextButton, speedButton};

    for (auto* button : buttons) {
        button->setStyleSheet(buttonStyle);
        button->setFixedSize(50, 50); //按钮大小
        button->setIconSize(QSize(30, 30));
    }

    speedButton->setStyleSheet(buttonStyle +
                               "QPushButton { min-width: 60px; color: white; }");

    // Left side buttons layout with smaller spacing
    QHBoxLayout* leftButtonsLayout = new QHBoxLayout();
    leftButtonsLayout->setSpacing(6);

    QHBoxLayout* volumeControlLayout = new QHBoxLayout();
    volumeControlLayout->setSpacing(6);
    volumeControlLayout->setContentsMargins(0, 0, 0, 0);  // 移除边距
    volumeControlLayout->addWidget(player->getVolumeButton());
    volumeControlLayout->addWidget(player->getVolumeSlider());

    leftButtonsLayout->addWidget(prevButton);
    leftButtonsLayout->addWidget(skipBackButton);
    leftButtonsLayout->addWidget(playPauseButton);
    leftButtonsLayout->addWidget(skipForwardButton);
    leftButtonsLayout->addWidget(nextButton);

    // Add layouts to main control layout

    controlLayout->addLayout(leftButtonsLayout);  // 左侧按钮组
    controlLayout->addLayout(volumeControlLayout); // 添加音量控制组
    controlLayout->addWidget(speedButton);        // 速度按钮

    controlLayout->addSpacing(10);

    // Connect signals
    connect(prevButton, &QPushButton::clicked, player, &ThePlayer::previousVideo);
    connect(skipBackButton, &QPushButton::clicked, player, &ThePlayer::skipBackward);
    connect(playPauseButton, &QPushButton::clicked, player, &ThePlayer::playPause);
    connect(skipForwardButton, &QPushButton::clicked, player, &ThePlayer::skipForward);
    connect(nextButton, &QPushButton::clicked, player, &ThePlayer::nextVideo);

    connect(player, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        playPauseButton->setIcon(QIcon(state == QMediaPlayer::PlayingState ?
                                           ":/icons/pause.svg" : ":/icons/play.svg"));
    });

    vLayout->addStretch();
    vLayout->addLayout(controlLayout);
    vLayout->addStretch();

    controlWidget->setFixedHeight(60);
    ui->videoLayout->insertWidget(2, controlWidget);
}
