//
//
//
#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QSlider>
#include <QLabel>
#include "progress_bar_widget.h"

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QVideoWidget* videoWidget;
    ProgressBarWidget* progressBar;
    int currentVideoIndex;
    bool isChangingVideo;
    int getCurrentVideoIndex(TheButtonInfo* info);
    QSlider* volumeSlider;      // 音量滑块
    QPushButton* volumeButton;  // 音量按钮
    bool isMuted;        // 静音状态
    QPushButton* fullScreenButton;
public:
    ThePlayer();  // 构造函数声明
    void setProgressBar(ProgressBarWidget* slider); // 设置进度条方法

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);
    void setupVolumeControl();     // 设置音量控制
    QSlider* getVolumeSlider() { return volumeSlider; }
    QPushButton* getVolumeButton() { return volumeButton; }

private slots:
    void playStateChanged(QMediaPlayer::State ms);
    void updateProgressBar(qint64 position); // 更新进度条位置

    void seekToPosition(qint64 value);  // 改为 qint64

signals:
    void positionChanged(qint64 position); // 重写信号（可选）
    void playbackRateChanged(double rate);

public slots:
    // start playing this ButtonInfo
    void jumpTo(TheButtonInfo* button);
    void playPause();
    void skipForward();
    void skipBackward();
    void nextVideo();
    void previousVideo();
    void changePlaybackSpeed();
    void updateVolume(int value);  // 更新音量
    void toggleMute();            // 切换静音
};

#endif //CW2_THE_PLAYER_H
