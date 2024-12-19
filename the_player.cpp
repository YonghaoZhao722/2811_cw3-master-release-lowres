//
//
//

#include "the_player.h"
#include <QVideoWidget>
#include "progress_bar_widget.h"

ThePlayer::ThePlayer()
    : QMediaPlayer(nullptr), progressBar(nullptr), currentVideoIndex(0), isChangingVideo(false), videoWidget(new QVideoWidget())
{
    setVolume(10);
    QVideoWidget* videoWidget = new QVideoWidget();
    setVideoOutput(videoWidget);
    videoWidget->show();
    setupVolumeControl();

    progressBar = new ProgressBarWidget();
    progressBar->setDuration(duration());
    connect(this, &QMediaPlayer::stateChanged, this, &ThePlayer::playStateChanged);
    connect(this, &QMediaPlayer::positionChanged, this, &ThePlayer::updateProgressBar);

    connect(this, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState) {
            nextVideo();
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
        jumpTo(buttons->at(0)->info);
    }
}


void ThePlayer::playStateChanged(QMediaPlayer::State ms) {
    qDebug() << "Player state changed to:" << ms << " Current index:" << currentVideoIndex;
    if (ms == QMediaPlayer::StoppedState && !isChangingVideo) {
        play();
    }
}



void ThePlayer::jumpTo(TheButtonInfo* button) {
    if (isChangingVideo) return;

    isChangingVideo = true;
    currentVideoIndex = getCurrentVideoIndex(button);
    qDebug() << "Jumping to video index:" << currentVideoIndex;

    setMedia(button->url);
    play();

    isChangingVideo = false;
}


void ThePlayer::seekToPosition(qint64 value) {
    setPosition(value);
}


void ThePlayer::updateProgressBar(qint64 position) {
    if (progressBar) {
        progressBar->setCurrentPosition(position);
    }
}

void ThePlayer::setProgressBar(ProgressBarWidget* slider) {
    progressBar = slider;
    if (progressBar) {
        setNotifyInterval(16);

        connect(progressBar, &ProgressBarWidget::positionChanged,
                this, &ThePlayer::seekToPosition);

        connect(this, &QMediaPlayer::positionChanged,
                progressBar, &ProgressBarWidget::setCurrentPosition);

        connect(this, &QMediaPlayer::durationChanged,
                progressBar, &ProgressBarWidget::setDuration);
    }
}

//==========Play control ===============
void ThePlayer::playPause() {
    if (state() == QMediaPlayer::PlayingState) {
        pause();
    } else {
        play();
    }
}

void ThePlayer::skipForward() {
    qint64 newPosition = position() + 2000;
    if (newPosition < duration()) {
        setPosition(newPosition);
    }
}

void ThePlayer::skipBackward() {
    qint64 newPosition = position() - 2000;
    if (newPosition >= 0) {
        setPosition(newPosition);
    }
}

void ThePlayer::nextVideo() {
    if (!infos || infos->empty() || isChangingVideo) return;

    isChangingVideo = true;

    // calculate next video index
    int nextIndex = (currentVideoIndex + 1) % infos->size();
    qDebug() << "Moving from index:" << currentVideoIndex << "to index:" << nextIndex;

    setMedia(QMediaContent());


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

    // calculate previous video index
    int prevIndex = (currentVideoIndex - 1 + infos->size()) % infos->size();
    qDebug() << "Moving from index:" << currentVideoIndex << "to index:" << prevIndex;


    setMedia(QMediaContent());


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

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(10);
    volumeSlider->setFixedWidth(70);


    volumeButton = new QPushButton();
    volumeButton->setIcon(QIcon(":/icons/volume.svg"));
    volumeButton->setFixedSize(32, 32);


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
        volumeButton->setIcon(QIcon(":/icons/mute.svg"));
    } else {
        setVolume(volumeSlider->value());
        volumeButton->setIcon(QIcon(":/icons/volume.svg"));
    }
}

