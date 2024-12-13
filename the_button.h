//
//
//
#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QLabel>
#include <QVBoxLayout>
#include <QMediaPlayer>

// TheButtonInfo 类扩展，添加duration属性
class TheButtonInfo {
public:
    QUrl* url;      // video file to play
    QIcon* icon;    // icon to display
    qint64 duration;// video duration in milliseconds

    TheButtonInfo(QUrl* url, QIcon* icon) :
        url(url),
        icon(icon),
        duration(0) {} // 初始化duration为0
};

class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info;

    TheButton(QWidget *parent) : QPushButton(parent) {
        setIconSize(QSize(200,110));
        setFixedSize(200, 140); // 设置固定大小，为时长标签留出空间

        // 创建并设置时长标签
        durationLabel = new QLabel(this);
        durationLabel->setAlignment(Qt::AlignCenter);
        durationLabel->setStyleSheet("QLabel { color: white; background-color: rgba(0,0,0,0.5); "
                                     "border-radius: 2px; padding: 2px 4px; }");
        durationLabel->move(5, 115); // 位置在缩略图下方

        // 连接点击信号
        connect(this, SIGNAL(released()), this, SLOT(clicked()));
    }

    void init(TheButtonInfo* i);
    void setDuration(qint64 duration); // 新增：设置时长的方法

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QLabel* durationLabel;      // 显示时长的标签
    QMediaPlayer* tempPlayer;   // 临时播放器，用于获取视频时长

    // 将时长转换为字符串形式 (例如 "3:45")
    QString formatDuration(qint64 milliseconds) {
        int seconds = milliseconds / 1000;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        return QString("%1:%2")
            .arg(minutes)
            .arg(seconds, 2, 10, QChar('0'));
    }

private slots:
    void clicked();
    // void handleDurationChanged(qint64 duration); // 处理视频时长获取

signals:
    void jumpTo(TheButtonInfo*);
};

#endif //CW2_THE_BUTTON_H
