//
//
//

#include "the_button.h"
#include <QTime>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>

void TheButton::init(TheButtonInfo* i) {
    info = i;

    if (i->icon) {
        setIcon(*i->icon);
        setIconSize(size());
        // 强制将图标显示在最上层
        setStyleSheet(R"(
            QPushButton {
                border: none;
                background: transparent;
                padding: 0;
                margin: 0;
                // background-position: top;  /* 将图标置于顶部 */
                background-repeat: no-repeat;
                z-index: 999;  /* 尝试强制最高层级 */
            }
        )");

        // 强制刷新和重绘
        update();
        raise();
    }
    setDuration(info->duration);
}


void TheButton::clicked() {
    emit jumpTo(info);
}

void TheButton::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);
    if (durationLabel) {
        durationLabel->raise();  // 确保时长标签始终在顶层
        updateDurationPosition();
    }
}

void TheButton::setDuration(long long duration) {

    if (!durationLabel) {
        durationLabel = new QLabel(this);
        durationLabel->setStyleSheet("QLabel { color: white; background-color: rgba(0, 0, 0, 0.6); border-radius: 3px; padding: 2px 4px; }");
        durationLabel->setAlignment(Qt::AlignCenter);
    }

    // 计算并输出中间值
    int totalSeconds = duration / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;


    QString durationText = QString("%1:%2")
                               .arg(minutes, 2, 10, QChar('0'))
                               .arg(seconds, 2, 10, QChar('0'));


    durationLabel->setText(durationText);
    durationLabel->adjustSize();
    updateDurationPosition();
}

void TheButton::updateDurationPosition() {
    if (durationLabel && !durationLabel->text().isEmpty()) {
        durationLabel->adjustSize();
        durationLabel->move(width() - durationLabel->width() - 5,
                            height() - durationLabel->height() - 5);
    }
}
