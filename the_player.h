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
    QSlider* volumeSlider;
    QPushButton* volumeButton;
    bool isMuted;
    QPushButton* fullScreenButton;
public:
    ThePlayer();
    void setProgressBar(ProgressBarWidget* slider);

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);
    void setupVolumeControl();
    QSlider* getVolumeSlider() { return volumeSlider; }
    QPushButton* getVolumeButton() { return volumeButton; }

private slots:
    void playStateChanged(QMediaPlayer::State ms);
    void updateProgressBar(qint64 position);

    void seekToPosition(qint64 value);

signals:
    void positionChanged(qint64 position);
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
    void updateVolume(int value);
    void toggleMute();
};

#endif //CW2_THE_PLAYER_H
