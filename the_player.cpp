//
//
//

#include "the_player.h"
#include <QVideoWidget>
#include "progress_bar_widget.h"

ThePlayer::ThePlayer()
    : QMediaPlayer(nullptr), progressBar(nullptr), currentVideoIndex(0), isChangingVideo(false)
{
    setVolume(10); // 设置初始音量
    QVideoWidget* videoWidget = new QVideoWidget(); // 创建视频显示部件
    setVideoOutput(videoWidget); // 设置视频输出到 QVideoWidget
    videoWidget->show(); // 显示视频窗口
    setupVolumeControl();

    progressBar = new ProgressBarWidget();
    progressBar->setDuration(duration());
    connect(this, &QMediaPlayer::stateChanged, this, &ThePlayer::playStateChanged);
    connect(this, &QMediaPlayer::positionChanged, this, &ThePlayer::updateProgressBar);

    connect(this, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState) {
            nextVideo(); // 播放结束时自动播放下一个
        }
    });

    connect(this, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        emit playStateChanged(state);
    });

}



// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    if (!buttons->empty() && !infos->empty()) {
        jumpTo(buttons->at(0)->info); // 自动播放第一个视频
    }
}

// // change the image and video for one button every one second
// void ThePlayer::shuffle() {
//     TheButtonInfo* i = & infos -> at (rand() % infos->size() );
// //        setMedia(*i->url);
//     buttons -> at( updateCount++ % buttons->size() ) -> init( i );
// }

void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    qDebug() << "Player state changed to:" << ms << " Current index:" << currentVideoIndex;
    if (ms == QMediaPlayer::StoppedState && !isChangingVideo) {
        play();
    }
}



void ThePlayer::jumpTo(TheButtonInfo* button) {
    if (isChangingVideo) return;

    isChangingVideo = true;

    // 获取目标视频索引
    currentVideoIndex = getCurrentVideoIndex(button);
    qDebug() << "Jumping to video index:" << currentVideoIndex;

    // 设置新视频
    setMedia(button->url);
    play();

    isChangingVideo = false;
}

// 跳转到指定时间点
void ThePlayer::seekToPosition(qint64 value) {
    setPosition(value);
}

// 更新进度条位置
void ThePlayer::updateProgressBar(qint64 position) {
    if (progressBar) {
        progressBar->setCurrentPosition(position);
    }
}

void ThePlayer::setProgressBar(ProgressBarWidget* slider) {
    progressBar = slider;
    if (progressBar) {
        setNotifyInterval(16);
        // 用户改变进度条位置时跳转视频
        connect(progressBar, &ProgressBarWidget::positionChanged,
                this, &ThePlayer::seekToPosition);
        // 视频播放位置变化时更新进度条
        connect(this, &QMediaPlayer::positionChanged,
                progressBar, &ProgressBarWidget::setCurrentPosition);
        // 视频时长变化时更新进度条范围
        connect(this, &QMediaPlayer::durationChanged,
                progressBar, &ProgressBarWidget::setDuration);
    }
}

//==========播放控制区域===============
void ThePlayer::playPause() {
    if (state() == QMediaPlayer::PlayingState) {
        pause();
    } else {
        play();
    }
}

void ThePlayer::skipForward() {
    qint64 newPosition = position() + 2000; // 前进2秒
    if (newPosition < duration()) {
        setPosition(newPosition);
    }
}

void ThePlayer::skipBackward() {
    qint64 newPosition = position() - 2000; // 后退2秒
    if (newPosition >= 0) {
        setPosition(newPosition);
    }
}

void ThePlayer::nextVideo() {
    if (!infos || infos->empty() || isChangingVideo) return;

    isChangingVideo = true;

    // 计算下一个视频索引
    int nextIndex = (currentVideoIndex + 1) % infos->size();
    qDebug() << "Moving from index:" << currentVideoIndex << "to index:" << nextIndex;

    setMedia(QMediaContent());  // 清除当前媒体

    // 设置新视频
    TheButtonInfo* nextVideo = &infos->at(nextIndex);
    setMedia(nextVideo->url);
    currentVideoIndex = nextIndex;

    play();


    qDebug() << "Now playing video at index:" << currentVideoIndex;
    isChangingVideo = false;
}

void ThePlayer::previousVideo() {
    if (!infos || infos->empty() || isChangingVideo) return;

    isChangingVideo = true;

    // 计算前一个视频索引
    int prevIndex = (currentVideoIndex - 1 + infos->size()) % infos->size();
    qDebug() << "Moving from index:" << currentVideoIndex << "to index:" << prevIndex;


    setMedia(QMediaContent());

    // 设置新视频
    TheButtonInfo* prevVideo = &infos->at(prevIndex);
    setMedia(prevVideo->url);
    currentVideoIndex = prevIndex;

    play();

    qDebug() << "Now playing video at index:" << currentVideoIndex;
    isChangingVideo = false;
}

void ThePlayer::changePlaybackSpeed() {
    //TODO
}

int ThePlayer::getCurrentVideoIndex(TheButtonInfo* info) {
    if (!infos) return 0;
    for (size_t i = 0; i < infos->size(); i++) {
        if (infos->at(i).url.toString() == info->url.toString()) {
            return i;
        }
    }
    return 0;
}

//=====Volume=====
void ThePlayer::setupVolumeControl() {
    // 创建音量滑块
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(10);  // 初始音量设为10
    volumeSlider->setFixedWidth(70);

    // 创建音量按钮
    volumeButton = new QPushButton();
    volumeButton->setIcon(QIcon(":/icons/volume.svg"));
    volumeButton->setFixedSize(32, 32);

    // 连接信号与槽
    connect(volumeSlider, &QSlider::valueChanged, this, &ThePlayer::updateVolume);
    connect(volumeButton, &QPushButton::clicked, this, &ThePlayer::toggleMute);

    isMuted = false;
}

void ThePlayer::updateVolume(int value) {
    if (!isMuted) {
        setVolume(value);
    }
}

void ThePlayer::toggleMute() {
    isMuted = !isMuted;
    if (isMuted) {
        setVolume(0);
        volumeButton->setIcon(QIcon(":/icons/mute.svg"));  // 需要添加静音图标资源
    } else {
        setVolume(volumeSlider->value());
        volumeButton->setIcon(QIcon(":/icons/volume.svg"));
    }
}
