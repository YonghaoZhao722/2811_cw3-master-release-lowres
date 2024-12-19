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
#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets>

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out;
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (f.contains("."))

#if defined(_WIN32)
        if (f.endsWith(".wmv", Qt::CaseInsensitive))  { // windows
#else
        if (f.endsWith(".mp4", Qt::CaseInsensitive) || f.endsWith(".MOV", Qt::CaseInsensitive))  { // mac/linux
#endif
            QString thumb = f.left(f.length() - 4) + ".png";
            QIcon ico;

            if (QFile::exists(thumb)) {
                QImageReader imageReader(thumb);
                QImage sprite = imageReader.read();
                if (!sprite.isNull()) {
                    ico = QIcon(QPixmap::fromImage(sprite));
                } else {
                    ico = QIcon(":/icons/defaultPicture.png");
                }
            } else {
                ico = QIcon(":/icons/defaultPicture.png");
            }

            QUrl url = QUrl::fromLocalFile(f);
            out.emplace_back(TheButtonInfo(url, ico));
        }
    }

    return out;
}

void MainWindow::maximizeWindow() {
    if (isFullScreen()) {
        showNormal();

    } else {
        showFullScreen();

    }
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("QMainWindow { background-color: #ffffff; }");
    setupUi();
    // 获取命令行参数
    QStringList args = QCoreApplication::arguments();
    QString videoPath = "";
    if (args.size() > 1) {
        videoPath = args.at(1);
    }

    // 调用 loadVideos
    loadVideos(videoPath);
    setupPlayer();
    createButtons();
    setupProgressBar();
    setupControlButtons();

    // 连接“加载视频”按钮
    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);
    ui->loadButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e6d9d6;"
        "    color: black;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    text-align: center;"
        "    text-decoration: none;"
        "    font-size: 16px;"
        "    border-radius: 8px;"
        "}"
        );

    // 创建“全屏”按钮
    QPushButton* fullScreenButton = new QPushButton("Fullscreen", this);
    fullScreenButton->setObjectName("fullScreenButton");
    fullScreenButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #a6bfbd;"
        "    color: black;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    font-size: 16px;"
        "    border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0b433e;"
        "}"
        );

    // 连接“全屏”按钮的点击信号到 maximizeWindow 槽
    connect(fullScreenButton, &QPushButton::clicked, this, &MainWindow::maximizeWindow);

    // 创建“Delete All”按钮
    QPushButton* deleteAllButton = new QPushButton("Delete All", this);
    deleteAllButton->setObjectName("deleteAllButton");
    deleteAllButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #ff4d4d;"  // 红色背景
        "    color: white;"               // 白色文字
        "    border: none;"
        "    padding: 10px 20px;"
        "    font-size: 16px;"
        "    border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ff1a1a;"  // 悬停时更深的红色
        "}"
        );
    // 获取 Qt Designer 中的 loadButton
    QPushButton* loadButton = ui->loadButton;
    // 将“LOAD”和“DELETE ALL”按钮添加到水平布局
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(loadButton);
    topLayout->addWidget(deleteAllButton);
    topLayout->addWidget(fullScreenButton);
    topLayout->addStretch(); // 将按钮推到左侧

    // 将顶部布局添加到主布局
    QWidget* topWidget = new QWidget(this);
    topWidget->setLayout(topLayout);
    ui->videoLayout->insertWidget(0, topWidget);

    // 连接“Delete All”按钮信号到槽函数
    connect(deleteAllButton, &QPushButton::clicked, this, &MainWindow::onDeleteAllClicked);

    // 开始播放第一个视频
    if (!videos.empty()) {
        qDebug() << "not empty ";
        player->setMedia(videos[0].url);
        player->play();
    }
    else{
        qDebug()<<"empty";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUi()
{
    setWindowTitle("tomeo");
    setMinimumSize(800, 680);

    // 设置视频窗口的样式
    ui->videoWidget->setStyleSheet("QVideoWidget { background-color: black; }");
    setStyleSheet("background-color: black;");
}

void MainWindow::loadVideos(const QString& path)
{
    qDebug() << "Loading videos from path:" << path;

    if (!path.isEmpty()) {
        std::string stdPath = path.toLocal8Bit().toStdString();
        videos = getInfoIn(stdPath);
        qDebug() << "Number of videos found:" << videos.size();
    }

    if (videos.empty()) {
        QMessageBox::information(this, "Tomeo", "No videos found in the given path.\nPlease select a folder.");

        QString selectedDir = QFileDialog::getExistingDirectory(
            this,
            "Select Video Folder",
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );

        if (selectedDir.isEmpty()) {
            QMessageBox::information(this, "Tomeo", "No folder selected. Exiting...");
            exit(-1);
        }

        std::string stdSelectedDir = selectedDir.toLocal8Bit().toStdString(); // 或 selectedDir.toUtf8().toStdString()
        videos = getInfoIn(stdSelectedDir);
        if (videos.empty()) {
            QMessageBox::information(this, "Tomeo", "No videos found in the selected folder. Exiting...");
            exit(-1);
        }
    }

    qDebug() << "Videos successfully loaded.";
}

void MainWindow::setupPlayer()
{
    player = new ThePlayer;
    player->setVideoOutput(ui->videoWidget);
    player->setContent(&buttons, &videos);

    // 设置视频窗口的背景
    ui->videoWidget->setAttribute(Qt::WA_NoSystemBackground);
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
    buttons.clear();
    QWidget* oldContainer = ui->scrollArea->widget();
    if (oldContainer) {
        delete oldContainer;
    }
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
        button->setIconSize(QSize(160, 50));  // 设置基础大小
        button->setObjectName("thumbnailButton");

        // 连接删除请求信号
        connect(button, &TheButton::jumpTo, player, &ThePlayer::jumpTo);
       // connect(button, &TheButton::deleteRequested, this, &MainWindow::onDeleteVideo);
        // 设置大小策略为可扩展但保持宽高比
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, &TheButton::jumpTo, player, &ThePlayer::jumpTo);
        connect(button, &TheButton::deleteRequested, this, &MainWindow::onDeleteVideo); //删除视频
         connect(button, &TheButton::showDetailsRequested, this, &MainWindow::detailVideo);
        button->init(&videos.at(i));
        buttons.push_back(button);

        // 右侧信息容器
        QWidget* infoWidget = new QWidget();
        infoWidget->setObjectName("infoWidget");
        QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setSpacing(40);

        // 文件名标签
        QLabel* nameLabel = new QLabel(videos[i].filename);
        nameLabel->setObjectName("nameLabel");
        nameLabel->setWordWrap(true);  // 允许文字换行
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        // 设置自动调整字体大小
        QFont font = nameLabel->font();
        font.setPointSize(10);  // 设置基础字体大小
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
            min-width: 100px;
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
            min-width: 60px;
            min-height: 50px;
            max-width: 100px;
            max-height:100px;
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
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QWidget* widget = qobject_cast<QWidget*>(obj);
            if (widget) {
                bool ok;
                int index = widget->property("buttonIndex").toInt(&ok);
                if (ok && index >= 0 && index < buttons.size()) {
                    // 触发播放信号
                    buttons[index]->clicked();
                    return true;
                }
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
    QPushButton* prevButton = new QPushButton();
    prevButton->setIcon(QIcon(":/icons/previous.svg"));

    QPushButton* skipBackButton = new QPushButton();
    skipBackButton->setIcon(QIcon(":/icons/rewind.svg"));

    QPushButton* playPauseButton = new QPushButton();
    playPauseButton->setIcon(QIcon(":/icons/play.svg"));

    QPushButton* skipForwardButton = new QPushButton();
    skipForwardButton->setIcon(QIcon(":/icons/fast-forward.svg"));

    QPushButton* nextButton = new QPushButton();
    nextButton->setIcon(QIcon(":/icons/next.svg"));

    QPushButton* speedButton = new QPushButton("1.0x");

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

    QList<QPushButton*> controlButtons = {prevButton, skipBackButton, playPauseButton,
                                           skipForwardButton, nextButton, speedButton};

    for (auto* button : controlButtons) {
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

    connect(player, &QMediaPlayer::stateChanged, [playPauseButton](QMediaPlayer::State state) {
        playPauseButton->setIcon(QIcon(state == QMediaPlayer::PlayingState ?
                                           ":/icons/pause.svg" : ":/icons/play.svg"));
    });

    vLayout->addStretch();
    vLayout->addLayout(controlLayout);
    vLayout->addStretch();

    controlWidget->setFixedHeight(60);
    ui->videoLayout->insertWidget(2, controlWidget);
}

// 实现加载文件
void MainWindow::onLoadButtonClicked()
{
    qDebug() << "onLoadButtonClicked called";

    // 创建一个提示框，让用户选择加载模式
    QMessageBox msgBox;
    msgBox.setWindowTitle("Load Video");
    msgBox.setText("Do you want to load video files or a folder?");
    QPushButton *fileButton = msgBox.addButton("File", QMessageBox::AcceptRole);
    QPushButton *folderButton = msgBox.addButton("Folder", QMessageBox::RejectRole);
    msgBox.addButton(QMessageBox::Cancel);

    msgBox.exec();

    if (msgBox.clickedButton() == fileButton) {
        // 选择加载文件
        QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select Video Files"),
                                                              "",
                                                              tr("Video Files (*.mp4 *.MOV *.wmv)"));
        if (fileNames.isEmpty()) {
            return;
        }

        QString errorMsg;
        bool hasError = false;

        for (const QString& file : fileNames) {
            QFileInfo info(file);
            QString suffix = info.suffix().toLower();
            if (suffix != "mp4" && suffix != "mov" && suffix != "wmv") {
                errorMsg += QString("Unsupported file format: %1\n").arg(file);
                hasError = true;
                continue;
            }

            QUrl url = QUrl::fromLocalFile(file);
            // 检查是否重复
            bool isDuplicate = false;
            for (const auto &v : videos) {
                if (v.url.toString() == url.toString()) {
                    errorMsg += QString("Same Video File: %1\n").arg(file);
                    hasError = true;
                    isDuplicate = true;
                    break;
                }
            }
            if (isDuplicate) continue;

            QString thumb = file.left(file.length() - 4) + ".png";
            QIcon ico;

            if (QFile::exists(thumb)) {
                QImageReader imageReader(thumb);
                QImage sprite = imageReader.read();
                if (!sprite.isNull()) {
                    ico = QIcon(QPixmap::fromImage(sprite));
                } else {
                    qDebug() << "Unable to process thumbnail：" << thumb;
                    ico = QIcon(":/icons/defaultPicture.png");
                }
            }

            // 如果没有缩略图，使用默认图标
            if (ico.isNull()) {
                ico = QIcon(":/icons/defaultPicture.png");
            }

            videos.emplace_back(TheButtonInfo(url, ico));
        }

        if (hasError) {
            QMessageBox errorBox;
            errorBox.setIcon(QMessageBox::Warning);
            errorBox.setWindowTitle("Error Loading Video");
            errorBox.setText(errorMsg);

            errorBox.setStyleSheet(
                "QMessageBox { background-color: white; color: black; }"
                "QPushButton { background-color: #d0d0d0; }"
                );
            errorBox.exec();
        }

    }
    else if (msgBox.clickedButton() == folderButton) {
        // 选择加载文件夹
        QString folder = QFileDialog::getExistingDirectory(this, tr("Select Video Folder"), "");

        if (folder.isEmpty()) {
            return;
        }

        std::vector<TheButtonInfo> folderVideos = getInfoIn(folder.toStdString());
        QString folderErrorMsg;
        bool folderHasError = false;

        if (folderVideos.empty()) {
            folderErrorMsg = QString("No video files found in folder: %1").arg(folder);
            folderHasError = true;
        }
        else {
            // 检查重复并添加
            for (auto& videoInfo : folderVideos) {
                bool isDuplicate = false;
                for (const auto &v : videos) {
                    if (v.url.toString() == videoInfo.url.toString()) {
                        folderErrorMsg += QString("Same Video File: %1\n").arg(videoInfo.url.toLocalFile());
                        folderHasError = true;
                        isDuplicate = true;
                        break;
                    }
                }
                if (!isDuplicate) {
                    videos.push_back(videoInfo);
                }
            }
        }

        if (folderHasError) {
            QMessageBox errorBox;
            errorBox.setIcon(QMessageBox::Warning);
            errorBox.setWindowTitle("Error Loading Video");
            errorBox.setText(folderErrorMsg);
            errorBox.setStyleSheet(
                "QMessageBox { background-color: white; color: black; }"
                "QPushButton { background-color: #d0d0d0; }"
                );
            errorBox.exec();
        }

    }
    else {
        return;
    }

    // 删除所有按钮的父控件
    QWidget* oldContainer = ui->scrollArea->widget();
    if (oldContainer) {
        delete oldContainer;
    }
    buttons.clear();

    createButtons();
}

//删除播放列表所有视频
void MainWindow::onDeleteAllClicked()
{
    if (videos.empty()) return;

    // 获取当前播放的视频 URL 字符串
    QString currentUrlStr = player->currentMedia().request().url().toString();

    // 创建确认对话框
    QMessageBox msgBox;
    msgBox.setWindowTitle("Confirm Deletion");
    msgBox.setText("Are you sure you want to delete all videos from the playlist except the currently playing one?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: white; color: black; }"
        "QPushButton { background-color: #d0d0d0; }"
        );

    // 显示对话框并获取用户选择
    if (msgBox.exec() == QMessageBox::Yes) {
        // 创建一个新的视频列表，仅包含当前播放的视频
        std::vector<TheButtonInfo> newVideos;

        for (const auto &v : videos) {
            if (v.url.toString() == currentUrlStr) {
                newVideos.push_back(v);
            }
            // 不需要删除，因为不再使用指针
        }

        // 更新视频列表
        videos = std::move(newVideos);

        // 删除所有按钮的父控件
        QWidget* oldContainer = ui->scrollArea->widget();
        if (oldContainer) {
            delete oldContainer;
        }
        buttons.clear();

        // 刷新播放列表 UI
        createButtons();
    }
}

void MainWindow::onDeleteVideo(TheButton* button)
{
    if (!button) return;

    // 找到按钮对应的视频信息
    auto it = std::find(buttons.begin(), buttons.end(), button);
    if (it == buttons.end()) {
        qDebug() << "Could not find corresponding video info for the button.";
        return;
    }

    int index = std::distance(buttons.begin(), it);
    if (index < 0 || index >= static_cast<int>(videos.size())) {
        qDebug() << "Invalid index for video deletion";
        return;
    }

    // 检查是否是唯一的视频
    if (videos.size() == 1) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Cannot Delete Video");
        msgBox.setText("This is the only video remaining in the playlist.");
        msgBox.setInformativeText("At least one video must be kept.");
        msgBox.exec();
        return;
    }

    // 确认删除
    QMessageBox msgBox;
    msgBox.setWindowTitle("Confirm Deletion");
    msgBox.setText(QString("Are you sure you want to delete \"%1\" from the playlist?")
                       .arg(videos[index].filename));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() != QMessageBox::Yes) {
        return;
    }

    // 在UI中删除按钮
    QWidget* container = ui->scrollArea->widget();
    if (container) {
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(container->layout());
        if (layout) {
            // 找到并删除对应的按钮widget
            for (int i = 0; i < layout->count(); ++i) {
                QWidget* widget = layout->itemAt(i)->widget();
                if (widget && widget->property("buttonIndex").toInt() == index) {
                    layout->removeWidget(widget);
                    widget->deleteLater();  // 安全删除widget
                    break;
                }
            }

            // 更新剩余按钮的索引
            for (int i = 0; i < layout->count(); ++i) {
                QWidget* widget = layout->itemAt(i)->widget();
                if (widget) {
                    int btnIndex = widget->property("buttonIndex").toInt();
                    if (btnIndex > index) {
                        widget->setProperty("buttonIndex", btnIndex - 1);
                    }
                }
            }
        }
    }

    // 更新数据
    videos.erase(videos.begin() + index);
    buttons.erase(buttons.begin() + index);

    // 强制布局更新
    if (container) {
        container->layout()->activate();
    }
}

void MainWindow::detailVideo(TheButton* button){

    if (!button || !button->info) {
        qDebug() <<"no information!";
        return;
    }

    // 获取视频信息
    QString filename = button->info->filename;
    QString filepath = button->info->url.toLocalFile();
    qint64 durationMs = button->info->duration;
    QString durationStr = QString::number(durationMs / 60000) + ":" + QString::number((durationMs % 60000) / 1000).rightJustified(2, '0');

    // 创建详细信息对话框
    QDialog detailDialog(this);
    detailDialog.setWindowTitle("Details");
    detailDialog.setStyleSheet("QDialog { background-color: white; }");
    QVBoxLayout* layout = new QVBoxLayout(&detailDialog);

    QLabel* nameLabel = new QLabel("Filename: " + filename, &detailDialog);
    nameLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    QLabel* pathLabel = new QLabel("Path: " + filepath, &detailDialog);
    pathLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    QLabel* durationLabel = new QLabel("Time: " + durationStr + " minutes", &detailDialog);
    durationLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    layout->addWidget(nameLabel);
    layout->addWidget(pathLabel);
    layout->addWidget(durationLabel);

    detailDialog.setLayout(layout);
    detailDialog.exec();
}
