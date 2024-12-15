#include "progress_bar_widget.h"
#include <QHBoxLayout>
#include <QTime>

ProgressBarWidget::ProgressBarWidget(QWidget* parent)
    : QWidget(parent)
    , totalDuration(0)
    , isUserInteracting(false)
{
    slider = new QSlider(Qt::Horizontal, this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(slider);
    layout->setContentsMargins(0, 0, 0, 0);
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
        }
    });
}

void ProgressBarWidget::setDuration(qint64 duration) {
    totalDuration = duration;
    slider->setRange(0, static_cast<int>(duration));
}

void ProgressBarWidget::setCurrentPosition(qint64 position) {
    // 只有在非用户操作时才更新滑块位置
    if (!isUserInteracting) {
        slider->blockSignals(true);  // 阻止发送信号
        slider->setValue(static_cast<int>(position));
        slider->blockSignals(false); // 恢复信号
    }
}
