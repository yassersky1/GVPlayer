#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;
class QSlider;
class QTimer;
class MpvWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(
        QWidget *parent = nullptr
    );

    ~MainWindow() override;

private:
    void openVideoFile();

    void togglePlayback();

    void updateProgress();

    void seekFromProgressSlider();

    void updateZoomLabel();

    void updateSpeedButtons(
        double activeSpeed
    );

    void setPlaybackControlsEnabled(
        bool enabled
    );

    MpvWidget *m_mpvWidget;

    QLabel *m_titleLabel;

    QSlider *m_progressSlider;

    QLabel *m_timeLabel;

    QPushButton *m_playPauseButton;

    QPushButton *m_backFiveMinutesButton;

    QPushButton *m_backFiveSecondsButton;

    QPushButton *m_forwardFiveSecondsButton;

    QPushButton *m_speedNormalButton;

    QPushButton *m_speedDoubleButton;

    QPushButton *m_speedQuadButton;

    QPushButton *m_speedOctupleButton;

    QPushButton *m_zoomOutButton;

    QPushButton *m_resetZoomButton;

    QPushButton *m_zoomInButton;

    QLabel *m_zoomLabel;

    QTimer *m_progressTimer;

    bool m_hasLoadedFile;
};

#endif
