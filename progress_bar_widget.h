#ifndef PROGRESS_BAR_WIDGET_H
#define PROGRESS_BAR_WIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QMouseEvent>

class ProgressBarWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProgressBarWidget(QWidget* parent = nullptr);
    void setDuration(qint64 duration);
    void setCurrentPosition(qint64 position);

signals:
    void positionChanged(qint64 position);

private:
    QSlider* slider;
    QLabel* timeLabel;
    qint64 totalDuration;
    bool isUserInteracting;

    QString formatTime(qint64 milliseconds);
    virtual void resizeEvent(QResizeEvent* event) override;  // 添加重置大小事件处理
};
#endif // PROGRESS_BAR_WIDGET_H
