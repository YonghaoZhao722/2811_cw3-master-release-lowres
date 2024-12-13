//
//
//

#include "the_player.h"
#include <QVideoWidget>

ThePlayer::ThePlayer()
    : QMediaPlayer(nullptr)
{
    setVolume(10); // 设置初始音量
    QVideoWidget* videoWidget = new QVideoWidget(); // 创建视频显示部件
    setVideoOutput(videoWidget); // 设置视频输出到 QVideoWidget
    videoWidget->show(); // 显示视频窗口

    connect(this, &QMediaPlayer::stateChanged, this, &ThePlayer::playStateChanged);
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

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            play(); // starting playing again...
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo(TheButtonInfo* button) {
    qDebug() << "Jumping to video: " << button->url->toString();
    setMedia(*button->url);
    if (mediaStatus() == QMediaPlayer::NoMedia) {
        qDebug() << "Failed to load media.";
    }
    play();
}

