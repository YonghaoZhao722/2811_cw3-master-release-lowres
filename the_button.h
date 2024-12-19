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
        updateDuration();
    }


    qint64 updateDuration() {
        if (url.isEmpty()) return 0;

        QMediaPlayer tempPlayer;
        tempPlayer.setMedia(QMediaContent(url));

        QEventLoop loop;
        QObject::connect(&tempPlayer, &QMediaPlayer::durationChanged,
                         [&loop, this](qint64 newDuration) {
                             duration = newDuration;
                             loop.quit();
                         });


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
        setMinimumSize(200, 112);
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        raise();


        durationLabel = new QLabel(this);
        durationLabel->setAlignment(Qt::AlignCenter);
        durationLabel->setStyleSheet("QLabel { color: white; background-color: rgba(0,0,0,0.5); "
                                     "border-radius: 2px; padding: 2px 4px; }");
        durationLabel->raise();


        connect(this, SIGNAL(released()), this, SLOT(clicked()));
    }

    void init(TheButtonInfo* i);
    void setDuration(qint64 duration);
protected:
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QLabel* durationLabel;

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
