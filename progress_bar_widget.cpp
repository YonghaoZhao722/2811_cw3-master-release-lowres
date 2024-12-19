#include "progress_bar_widget.h"
#include <QHBoxLayout>
#include <QTime>
#include <QResizeEvent>

ProgressBarWidget::ProgressBarWidget(QWidget* parent)
    : QWidget(parent)
    , totalDuration(0)
    , isUserInteracting(false)
{
    // Create the slider and time label
    slider = new QSlider(Qt::Horizontal, this);
    timeLabel = new QLabel("00:00 / 00:00", this);

    // Set properties for the time label
    timeLabel->setFixedWidth(100);  // Fix the width of the time label
    timeLabel->setAlignment(Qt::AlignCenter);

    // Create a horizontal layout
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(slider, 1);  // Set stretch factor to 1 so the slider occupies remaining space
    layout->addWidget(timeLabel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    setLayout(layout);

    // When the user starts dragging the slider
    connect(slider, &QSlider::sliderPressed, this, [this]() {
        isUserInteracting = true;
    });

    // When the user releases the slider
    connect(slider, &QSlider::sliderReleased, this, [this]() {
        isUserInteracting = false;
        emit positionChanged(slider->value());
    });

    // When the user clicks on a new position of the slider
    connect(slider, &QSlider::sliderMoved, this, [this](int value) {
        if (isUserInteracting) {
            emit positionChanged(value);
            // Update the time display
            timeLabel->setText(QString("%1 / %2")
                                   .arg(formatTime(value))
                                   .arg(formatTime(totalDuration)));
        }
    });
}

void ProgressBarWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // Automatically adjust the slider size when the window is resized
    // Since we use layout's stretch factor, there's no need to manually calculate the width here
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
