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
#include <QFileInfo>
#include <QTimer>
#include <QEventLoop>
#include <QIcon>
#include <memory>
// TheButtonInfo 类扩展，添加duration属性
class TheButtonInfo {
public:
    QUrl url;
    QIcon icon;
    qint64 duration;
    QString filename;

    TheButtonInfo(const QUrl& u, const QIcon& i, const QString& name = "") :
        url(u),
        icon(i),
        duration(0),
        filename(name)
    {
        if (filename.isEmpty()) {
            filename = QFileInfo(url.toLocalFile()).fileName();
        }
        updateDuration();  // 在构造函数中调用
    }

    // 新增：获取视频时长的方法
    qint64 updateDuration() {
        if (url.isEmpty()) return 0;

        QMediaPlayer tempPlayer;
        tempPlayer.setMedia(QMediaContent(url));

        QEventLoop loop;
        QObject::connect(&tempPlayer, &QMediaPlayer::durationChanged,
                         [&loop, this](qint64 newDuration) {
                             duration = newDuration;  // 更新 duration
                             loop.quit();  // 结束事件循环
                         });

        // 使用 QTimer 确保 event loop 有时间退出，避免死锁
        QTimer::singleShot(2000, &loop, &QEventLoop::quit);
        loop.exec();

        return duration;
    }
};


class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info;
    TheButton(QWidget *parent) : QPushButton(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setMinimumSize(200, 112);  // 16:9 比例的最小尺寸
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        raise();  // 提升Z顺序

        // 创建并设置时长标签
        durationLabel = new QLabel(this);
        durationLabel->setAlignment(Qt::AlignCenter);
        durationLabel->setStyleSheet("QLabel { color: white; background-color: rgba(0,0,0,0.5); "
                                     "border-radius: 2px; padding: 2px 4px; }");
        durationLabel->raise();

        // 连接点击信号
        connect(this, SIGNAL(released()), this, SLOT(clicked()));
    }

    void init(TheButtonInfo* i);
    void setDuration(qint64 duration); // 新增：设置时长的方法    
protected:
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QLabel* durationLabel;      // 显示时长的标签

public slots:
    void clicked();

private slots:
    void updateDurationPosition();

signals:
    void jumpTo(TheButtonInfo*);
    void deleteRequested(TheButton*);
    void showDetailsRequested(TheButton*);
};

#endif //CW2_THE_BUTTON_H
