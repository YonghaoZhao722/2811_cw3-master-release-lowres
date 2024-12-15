#include "progress_bar_widget.h"
#include <QHBoxLayout>
#include <QTime>
#include <QResizeEvent>

ProgressBarWidget::ProgressBarWidget(QWidget* parent)
    : QWidget(parent)
    , totalDuration(0)
    , isUserInteracting(false)
{
    // 创建滑块和时间标签
    slider = new QSlider(Qt::Horizontal, this);
    timeLabel = new QLabel("00:00 / 00:00", this);

    // 设置时间标签属性
    timeLabel->setFixedWidth(100);  // 固定时间标签宽度
    timeLabel->setAlignment(Qt::AlignCenter);

    // 创建水平布局
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(slider, 1);  // 设置stretch factor为1，使进度条占据剩余空间
    layout->addWidget(timeLabel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    setLayout(layout);

    // 当用户开始拖动滑块
    connect(slider, &QSlider::sliderPressed, this, [this]() {
        isUserInteracting = true;
    });

    // 当用户释放滑块
    connect(slider, &QSlider::sliderReleased, this, [this]() {
        isUserInteracting = false;
        emit positionChanged(slider->value());
    });

    // 用户点击滑块的新位置
    connect(slider, &QSlider::sliderMoved, this, [this](int value) {
        if (isUserInteracting) {
            emit positionChanged(value);
            // 更新时间显示
            timeLabel->setText(QString("%1 / %2")
                                   .arg(formatTime(value))
                                   .arg(formatTime(totalDuration)));
        }
    });
}

void ProgressBarWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // 当窗口大小改变时，进度条会自动调整大小
    // 因为我们使用了layout的stretch factor，所以不需要在这里手动计算宽度
}

QString ProgressBarWidget::formatTime(qint64 milliseconds)
{
    int seconds = milliseconds / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

void ProgressBarWidget::setDuration(qint64 duration) {
    totalDuration = duration;
    slider->setRange(0, static_cast<int>(duration));
    timeLabel->setText(QString("00:00 / %1").arg(formatTime(duration)));
}

void ProgressBarWidget::setCurrentPosition(qint64 position) {
    if (!isUserInteracting) {
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(position));
        slider->blockSignals(false);

        timeLabel->setText(QString("%1 / %2")
                               .arg(formatTime(position))
                               .arg(formatTime(totalDuration)));
    }
}
