#include "MainWindow.h"
#include "MpvWidget.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QSlider>
#include <QString>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_mpvWidget(nullptr),
      m_titleLabel(nullptr),
      m_progressSlider(nullptr),
      m_timeLabel(nullptr),
      m_playPauseButton(nullptr),
      m_backFiveMinutesButton(nullptr),
      m_backFiveSecondsButton(nullptr),
      m_forwardFiveSecondsButton(nullptr),
      m_speedNormalButton(nullptr),
      m_speedDoubleButton(nullptr),
      m_speedQuadButton(nullptr),
      m_speedOctupleButton(nullptr),
      m_zoomOutButton(nullptr),
      m_resetZoomButton(nullptr),
      m_zoomInButton(nullptr),
      m_zoomLabel(nullptr),
      m_progressTimer(nullptr),
      m_hasLoadedFile(false)
{
    setWindowTitle(
        QStringLiteral(
            "GV Player"
        )
    );

    resize(
        1100,
        700
    );

    setMinimumSize(
        800,
        550
    );

    QWidget *centralWidget =
        new QWidget(this);

    centralWidget->setObjectName(
        "centralWidget"
    );

    setCentralWidget(
        centralWidget
    );

    QVBoxLayout *mainLayout =
        new QVBoxLayout(
            centralWidget
        );

    mainLayout->setContentsMargins(
        16,
        16,
        16,
        16
    );

    mainLayout->setSpacing(
        12
    );

    m_titleLabel =
        new QLabel(
            QStringLiteral(
                "GV Player"
            ),
            centralWidget
        );

    m_titleLabel->setObjectName(
        "titleLabel"
    );

    m_titleLabel->setAlignment(
        Qt::AlignCenter
    );

    QFont titleFont =
        m_titleLabel->font();

    titleFont.setPointSize(
        18
    );

    titleFont.setBold(
        true
    );

    m_titleLabel->setFont(
        titleFont
    );

    mainLayout->addWidget(
        m_titleLabel
    );

    QFrame *videoFrame =
        new QFrame(
            centralWidget
        );

    videoFrame->setObjectName(
        "videoFrame"
    );

    videoFrame->setFrameShape(
        QFrame::NoFrame
    );

    videoFrame->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    QVBoxLayout *videoFrameLayout =
        new QVBoxLayout(
            videoFrame
        );

    videoFrameLayout->setContentsMargins(
        1,
        1,
        1,
        1
    );

    videoFrameLayout->setSpacing(
        0
    );

    m_mpvWidget =
        new MpvWidget(
            videoFrame
        );

    m_mpvWidget->setObjectName(
        "mpvWidget"
    );

    videoFrameLayout->addWidget(
        m_mpvWidget
    );

    mainLayout->addWidget(
        videoFrame,
        1
    );

    QHBoxLayout *progressLayout =
        new QHBoxLayout();

    progressLayout->setSpacing(
        10
    );

    m_progressSlider =
        new QSlider(
            Qt::Horizontal,
            centralWidget
        );

    m_progressSlider->setObjectName(
        "progressSlider"
    );

    m_progressSlider->setRange(
        0,
        1000
    );

    m_progressSlider->setValue(
        0
    );

    m_progressSlider->setEnabled(
        false
    );

    m_timeLabel =
        new QLabel(
            QStringLiteral(
                "0s / 0s"
            ),
            centralWidget
        );

    m_timeLabel->setObjectName(
        "timeLabel"
    );

    m_timeLabel->setAlignment(
        Qt::AlignCenter
    );

    m_timeLabel->setMinimumWidth(
        96
    );

    progressLayout->addWidget(
        m_progressSlider
    );

    progressLayout->addWidget(
        m_timeLabel
    );

    mainLayout->addLayout(
        progressLayout
    );

    QHBoxLayout *controlsLayout =
        new QHBoxLayout();

    controlsLayout->setSpacing(
        8
    );

    QPushButton *openFileButton =
        new QPushButton(
            QStringLiteral(
                "Open"
            ),
            centralWidget
        );

    m_backFiveMinutesButton =
        new QPushButton(
            QStringLiteral(
                "⏮ 5m"
            ),
            centralWidget
        );

    m_backFiveSecondsButton =
        new QPushButton(
            QStringLiteral(
                "◀ 5s"
            ),
            centralWidget
        );

    m_playPauseButton =
        new QPushButton(
            QStringLiteral(
                "▶ Play"
            ),
            centralWidget
        );

    m_forwardFiveSecondsButton =
        new QPushButton(
            QStringLiteral(
                "5s ▶"
            ),
            centralWidget
        );

    m_speedNormalButton =
        new QPushButton(
            QStringLiteral(
                "1x"
            ),
            centralWidget
        );

    m_speedDoubleButton =
        new QPushButton(
            QStringLiteral(
                "2x"
            ),
            centralWidget
        );

    m_speedQuadButton =
        new QPushButton(
            QStringLiteral(
                "4x"
            ),
            centralWidget
        );

    m_speedOctupleButton =
        new QPushButton(
            QStringLiteral(
                "8x"
            ),
            centralWidget
        );

    m_zoomOutButton =
        new QPushButton(
            QString::fromUtf8(
                "−"
            ),
            centralWidget
        );

    m_resetZoomButton =
        new QPushButton(
            QString::fromUtf8(
                "0"
            ),
            centralWidget
        );

    m_zoomInButton =
        new QPushButton(
            QString::fromUtf8(
                "+"
            ),
            centralWidget
        );

    m_zoomLabel =
        new QLabel(
            QStringLiteral(
                "100%"
            ),
            centralWidget
        );

    openFileButton->setObjectName(
        "openFileButton"
    );

    m_backFiveMinutesButton->setObjectName(
        "backFiveMinutesButton"
    );

    m_backFiveSecondsButton->setObjectName(
        "backFiveSecondsButton"
    );

    m_playPauseButton->setObjectName(
        "playPauseButton"
    );

    m_forwardFiveSecondsButton->setObjectName(
        "forwardFiveSecondsButton"
    );

    m_speedNormalButton->setObjectName(
        "speedNormalButton"
    );

    m_speedDoubleButton->setObjectName(
        "speedDoubleButton"
    );

    m_speedQuadButton->setObjectName(
        "speedQuadButton"
    );

    m_speedOctupleButton->setObjectName(
        "speedOctupleButton"
    );

    m_zoomOutButton->setObjectName(
        "zoomOutButton"
    );

    m_resetZoomButton->setObjectName(
        "resetZoomButton"
    );

    m_zoomInButton->setObjectName(
        "zoomInButton"
    );

    m_zoomLabel->setObjectName(
        "zoomLabel"
    );

    m_zoomLabel->setAlignment(
        Qt::AlignCenter
    );

    m_zoomLabel->setMinimumWidth(
        64
    );

    setPlaybackControlsEnabled(false);

    controlsLayout->addWidget(
        openFileButton
    );

    controlsLayout->addStretch();

    controlsLayout->addWidget(
        m_backFiveMinutesButton
    );

    controlsLayout->addWidget(
        m_backFiveSecondsButton
    );

    controlsLayout->addWidget(
        m_playPauseButton
    );

    controlsLayout->addWidget(
        m_forwardFiveSecondsButton
    );

    controlsLayout->addWidget(
        m_speedNormalButton
    );

    controlsLayout->addWidget(
        m_speedDoubleButton
    );

    controlsLayout->addWidget(
        m_speedQuadButton
    );

    controlsLayout->addWidget(
        m_speedOctupleButton
    );

    controlsLayout->addStretch();

    controlsLayout->addWidget(
        m_zoomOutButton
    );

    controlsLayout->addWidget(
        m_resetZoomButton
    );

    controlsLayout->addWidget(
        m_zoomInButton
    );

    controlsLayout->addWidget(
        m_zoomLabel
    );

    mainLayout->addLayout(
        controlsLayout
    );

    connect(
        openFileButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            openVideoFile();
        }
    );

    connect(
        m_playPauseButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            togglePlayback();
        }
    );

    connect(
        m_backFiveMinutesButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->seekRelative(
                -300.0
            );
        }
    );

    connect(
        m_backFiveSecondsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->seekRelative(
                -5.0
            );
        }
    );

    connect(
        m_forwardFiveSecondsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->seekRelative(
                5.0
            );
        }
    );

    connect(
        m_speedNormalButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->setPlaybackSpeed(
                1.0
            );

            updateSpeedButtons(
                1.0
            );
        }
    );

    connect(
        m_speedDoubleButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->setPlaybackSpeed(
                2.0
            );

            updateSpeedButtons(
                2.0
            );
        }
    );

    connect(
        m_speedQuadButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->setPlaybackSpeed(
                4.0
            );

            updateSpeedButtons(
                4.0
            );
        }
    );

    connect(
        m_speedOctupleButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->setPlaybackSpeed(
                8.0
            );

            updateSpeedButtons(
                8.0
            );
        }
    );

    connect(
        m_zoomOutButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->zoomBy(
                -0.1
            );

            updateZoomLabel();
        }
    );

    connect(
        m_resetZoomButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->resetZoom();

            updateZoomLabel();
        }
    );

    connect(
        m_zoomInButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            m_mpvWidget->zoomBy(
                0.1
            );

            updateZoomLabel();
        }
    );

    connect(
        m_progressSlider,
        &QSlider::sliderReleased,
        this,
        [this]()
        {
            seekFromProgressSlider();
        }
    );

    m_progressTimer =
        new QTimer(
            this
        );

    m_progressTimer->setInterval(
        250
    );

    connect(
        m_progressTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            updateProgress();
            updateZoomLabel();
        }
    );

    centralWidget->setStyleSheet(
        QString::fromUtf8(
            R"(
                QWidget#centralWidget {
                    background-color: #17181c;
                    color: #f5f5f5;
                }

                QLabel#titleLabel {
                    color: #ffffff;
                    padding: 6px;
                }

                QFrame#videoFrame {
                    background-color: #000000;
                    border: 1px solid #34363d;
                    border-radius: 8px;
                }

                QOpenGLWidget#mpvWidget {
                    background-color: #000000;
                    border: none;
                }

                QPushButton {
                    min-height: 34px;
                    padding-left: 14px;
                    padding-right: 14px;
                    color: #ffffff;
                    background-color: #2a2c32;
                    border: 1px solid #3a3d45;
                    border-radius: 7px;
                }

                QPushButton:hover {
                    background-color: #373a42;
                }

                QPushButton:pressed {
                    background-color: #1f2025;
                }

                QPushButton:disabled {
                    color: #70737b;
                    background-color: #202126;
                    border-color: #292b31;
                }

                QPushButton#playPauseButton {
                    min-width: 110px;
                    background-color: #1967d2;
                    border-color: #2877e4;
                    font-weight: bold;
                }

                QPushButton#playPauseButton:hover {
                    background-color: #2675df;
                }

                QPushButton#playPauseButton:disabled {
                    color: #82858d;
                    background-color: #252932;
                    border-color: #30343d;
                }

                QPushButton[activeSpeed="true"] {
                    color: #ffffff;
                    background-color: #1967d2;
                    border-color: #3b8cff;
                    font-weight: bold;
                }

                QPushButton[activeSpeed="true"]:hover {
                    background-color: #2675df;
                }

                QLabel#zoomLabel {
                    min-height: 34px;
                    padding-left: 10px;
                    padding-right: 10px;
                    color: #dfe7ff;
                    background-color: #20242d;
                    border: 1px solid #303949;
                    border-radius: 7px;
                    font-weight: bold;
                }

                QLabel#timeLabel {
                    min-height: 28px;
                    padding-left: 10px;
                    padding-right: 10px;
                    color: #d6d9e0;
                    background-color: #202126;
                    border: 1px solid #30323a;
                    border-radius: 7px;
                    font-weight: bold;
                }

                QSlider::groove:horizontal {
                    height: 6px;
                    background-color: #3b3d45;
                    border-radius: 3px;
                }

                QSlider::handle:horizontal {
                    width: 16px;
                    height: 16px;
                    margin: -5px 0;
                    background-color: #ffffff;
                    border-radius: 8px;
                }

                QSlider::sub-page:horizontal {
                    background-color: #1967d2;
                    border-radius: 3px;
                }

                QSlider::groove:horizontal:disabled {
                    background-color: #292b30;
                }

                QSlider::handle:horizontal:disabled {
                    background-color: #555861;
                }
            )"
        )
    );

    if (
        m_mpvWidget == nullptr
        ||
        !m_mpvWidget->isMpvAvailable()
    )
    {
        QMessageBox::critical(
            this,
            QStringLiteral(
                "mpv Error"
            ),
            m_mpvWidget != nullptr
                ? m_mpvWidget->lastError()
                : QStringLiteral(
                    "Failed to create video player."
                )
        );
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::openVideoFile()
{
    const QString videoFilter =
        QStringLiteral(
            "Video Files "
            "(*.mp4 *.m4v *.mov *.mkv *.avi "
            "*.webm *.flv *.wmv *.ts *.m2ts "
            "*.mts *.mpg *.mpeg *.vob *.3gp "
            "*.ogv *.hevc *.h265);;"
            "All Files (*)"
        );

    const QString filePath =
        QFileDialog::getOpenFileName(
            this,
            QStringLiteral(
                "Open Video File"
            ),
            QDir::homePath(),
            videoFilter
        );

    if (filePath.isEmpty())
    {
        return;
    }

    if (
        m_mpvWidget == nullptr
        ||
        !m_mpvWidget->loadFile(filePath)
    )
    {
        QMessageBox::critical(
            this,
            QStringLiteral(
                "Cannot Open Video"
            ),
            m_mpvWidget != nullptr
                ? m_mpvWidget->lastError()
                : QStringLiteral(
                    "Video player is not available."
                )
        );

        return;
    }

    const QFileInfo fileInformation(
        filePath
    );

    m_titleLabel->setText(
        fileInformation.fileName()
    );

    m_hasLoadedFile = true;

    m_mpvWidget->setPlaybackSpeed(
        1.0
    );

    updateSpeedButtons(
        1.0
    );

    setPlaybackControlsEnabled(
        true
    );

    m_playPauseButton->setText(
        QStringLiteral(
            "⏸ Pause"
        )
    );

    updateProgress();

    updateZoomLabel();

    m_progressTimer->start();
}

void MainWindow::togglePlayback()
{
    if (
        !m_hasLoadedFile
        ||
        m_mpvWidget == nullptr
    )
    {
        return;
    }

    const bool paused =
        m_mpvWidget->togglePause();

    if (paused)
    {
        m_playPauseButton->setText(
            QStringLiteral(
                "▶ Play"
            )
        );
    }
    else
    {
        m_playPauseButton->setText(
            QStringLiteral(
                "⏸ Pause"
            )
        );
    }
}

void MainWindow::updateProgress()
{
    if (
        !m_hasLoadedFile
        ||
        m_mpvWidget == nullptr
        ||
        m_progressSlider == nullptr
        ||
        m_progressSlider->isSliderDown()
    )
    {
        return;
    }

    const double mediaDuration =
        m_mpvWidget->duration();

    const double currentPosition =
        m_mpvWidget->position();

    if (mediaDuration <= 0.0)
    {
        m_progressSlider->setValue(
            0
        );

        if (m_timeLabel != nullptr)
        {
            m_timeLabel->setText(
                QStringLiteral(
                    "0s / 0s"
                )
            );
        }

        return;
    }

    const int sliderValue =
        static_cast<int>(
            (
                currentPosition
                /
                mediaDuration
            )
            *
            static_cast<double>(
                m_progressSlider->maximum()
            )
        );

    m_progressSlider->setValue(
        sliderValue
    );

    if (m_timeLabel != nullptr)
    {
        m_timeLabel->setText(
            QStringLiteral("%1s / %2s").arg(
                static_cast<int>(
                    currentPosition
                    +
                    0.5
                )
            )
            .arg(
                static_cast<int>(
                    mediaDuration
                    +
                    0.5
                )
            )
        );
    }
}

void MainWindow::seekFromProgressSlider()
{
    if (
        !m_hasLoadedFile
        ||
        m_mpvWidget == nullptr
        ||
        m_progressSlider == nullptr
    )
    {
        return;
    }

    const double ratio =
        static_cast<double>(
            m_progressSlider->value()
        )
        /
        static_cast<double>(
            m_progressSlider->maximum()
        );

    m_mpvWidget->seekToRatio(
        ratio
    );

    updateProgress();
}

void MainWindow::updateZoomLabel()
{
    if (
        m_zoomLabel == nullptr
        ||
        m_mpvWidget == nullptr
    )
    {
        return;
    }

    const int zoomPercent =
        static_cast<int>(
            m_mpvWidget->zoomPercent()
            +
            0.5
        );

    m_zoomLabel->setText(
        QStringLiteral("%1%").arg(
            zoomPercent
        )
    );
}

void MainWindow::updateSpeedButtons(
    double activeSpeed
)
{
    QPushButton *speedButtons[] =
    {
        m_speedNormalButton,
        m_speedDoubleButton,
        m_speedQuadButton,
        m_speedOctupleButton
    };

    const double speedValues[] =
    {
        1.0,
        2.0,
        4.0,
        8.0
    };

    for (int index = 0; index < 4; ++index)
    {
        QPushButton *button =
            speedButtons[index];

        if (button == nullptr)
        {
            continue;
        }

        button->setProperty(
            "activeSpeed",
            speedValues[index] == activeSpeed
        );

        button->style()->unpolish(
            button
        );

        button->style()->polish(
            button
        );

        button->update();
    }
}

void MainWindow::setPlaybackControlsEnabled(
    bool enabled
)
{
    if (m_progressSlider != nullptr)
    {
        m_progressSlider->setEnabled(
            enabled
        );
    }

    if (m_timeLabel != nullptr)
    {
        m_timeLabel->setEnabled(
            enabled
        );

        if (!enabled)
        {
            m_timeLabel->setText(
                QStringLiteral(
                    "0s / 0s"
                )
            );
        }
    }

    if (m_playPauseButton != nullptr)
    {
        m_playPauseButton->setEnabled(
            enabled
        );
    }

    if (m_backFiveMinutesButton != nullptr)
    {
        m_backFiveMinutesButton->setEnabled(
            enabled
        );
    }

    if (m_backFiveSecondsButton != nullptr)
    {
        m_backFiveSecondsButton->setEnabled(
            enabled
        );
    }

    if (m_forwardFiveSecondsButton != nullptr)
    {
        m_forwardFiveSecondsButton->setEnabled(
            enabled
        );
    }

    if (m_speedNormalButton != nullptr)
    {
        m_speedNormalButton->setEnabled(
            enabled
        );
    }

    if (m_speedDoubleButton != nullptr)
    {
        m_speedDoubleButton->setEnabled(
            enabled
        );
    }

    if (m_speedQuadButton != nullptr)
    {
        m_speedQuadButton->setEnabled(
            enabled
        );
    }

    if (m_speedOctupleButton != nullptr)
    {
        m_speedOctupleButton->setEnabled(
            enabled
        );
    }

    if (m_zoomOutButton != nullptr)
    {
        m_zoomOutButton->setEnabled(
            enabled
        );
    }

    if (m_resetZoomButton != nullptr)
    {
        m_resetZoomButton->setEnabled(
            enabled
        );
    }

    if (m_zoomInButton != nullptr)
    {
        m_zoomInButton->setEnabled(
            enabled
        );
    }

    if (m_zoomLabel != nullptr)
    {
        m_zoomLabel->setEnabled(
            enabled
        );
    }
}
