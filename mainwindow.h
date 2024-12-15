#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVideoWidget>
#include <QScrollArea>
#include <QLabel>
#include <QMediaPlayer>
#include "the_player.h"
#include "the_button.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadVideos(const std::string& path = "");

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::MainWindow *ui;
    ThePlayer *player;
    std::vector<TheButton*> buttons;
    std::vector<TheButtonInfo> videos;

    void setupUi();
    void setupPlayer();
    void createButtons();
    void updateFontSizes();
    void setupProgressBar();
    void setupPreviewLabel();

    //视频播放控制
    QPushButton* prevButton;
    QPushButton* skipBackButton;
    QPushButton* playPauseButton;
    QPushButton* skipForwardButton;
    QPushButton* nextButton;
    QPushButton* speedButton;
    void setupControlButtons();
};

#endif // MAINWINDOW_H
