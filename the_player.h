//
//
//
#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>

class ThePlayer : public QMediaPlayer {
    Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QVideoWidget* videoWidget;

public:
    ThePlayer();  // 构造函数声明

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);

private slots:
    void playStateChanged(QMediaPlayer::State ms);

public slots:
    // start playing this ButtonInfo
    void jumpTo(TheButtonInfo* button);
};

#endif //CW2_THE_PLAYER_H
