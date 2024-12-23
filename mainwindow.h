#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVideoWidget>
#include <QScrollArea>
#include <QLabel>
#include <QMediaPlayer>
#include "the_player.h"
#include "the_button.h"
#include <vector>
#include <QMediaPlaylist>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadVideos(const QString& path);

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onLoadButtonClicked();
    void onDeleteAllClicked();
    void onDeleteVideo(TheButton* button);
    void detailVideo(TheButton* button);
    void maximizeWindow();

private:
    Ui::MainWindow *ui;
    ThePlayer *player;
    QMediaPlaylist* playlist;
    std::vector<TheButton*> buttons;
    std::vector<TheButtonInfo> videos;

    void setupUi();
    void setupPlayer();
    void createButtons();
    void updateFontSizes();
    void setupProgressBar();
    void setupPreviewLabel();


    QPushButton* prevButton;
    QPushButton* skipBackButton;
    QPushButton* playPauseButton;
    QPushButton* skipForwardButton;
    QPushButton* nextButton;
    QPushButton* speedButton;
    void setupControlButtons();
};

#endif // MAINWINDOW_H
