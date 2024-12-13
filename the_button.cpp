//
//
//

#include "the_button.h"
#include <QTime>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>

void TheButton::init(TheButtonInfo* i) {
    setIcon( *(i->icon) );
    info =  i;
}


void TheButton::clicked() {
    emit jumpTo(info);
}

void TheButton::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);
    if (durationLabel) {
        // 更新时长标签的位置，放在按钮底部
        durationLabel->move(width() - durationLabel->width() - 5,
                            height() - durationLabel->height() - 5);
    }
}

void TheButton::setDuration(long long duration) {
    if (!durationLabel) {
        // 如果 durationLabel 尚未初始化，创建它
        durationLabel = new QLabel(this);
        durationLabel->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 0.6); border-radius: 3px;");
        durationLabel->setAlignment(Qt::AlignCenter);
    }

    // 将时长转换为 mm:ss 格式
    QTime time(0, 0, 0);
    time = time.addMSecs(static_cast<int>(duration));
    QString durationText = time.toString("mm:ss");

    // 设置标签文本
    durationLabel->setText(durationText);
    durationLabel->adjustSize(); // 自动调整大小

    // 将标签定位到按钮的右下角
    durationLabel->move(width() - durationLabel->width() - 5,
                        height() - durationLabel->height() - 5);
}
