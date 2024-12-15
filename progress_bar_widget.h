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
    qint64 totalDuration;
    bool isUserInteracting;  // 标记是否是用户操作
};
#endif // PROGRESS_BAR_WIDGET_H
