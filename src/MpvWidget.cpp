#include "MpvWidget.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMouseEvent>
#include <QMetaObject>
#include <QOpenGLContext>
#include <QProcess>
#include <QStandardPaths>
#include <QSurfaceFormat>
#include <QWheelEvent>

#include <mpv/client.h>
#include <mpv/render.h>
#include <mpv/render_gl.h>

#include <algorithm>
#include <cmath>

MpvWidget::MpvWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_mpv(nullptr),
      m_renderContext(nullptr),
      m_lastError(),
      m_temporaryPlaybackFile(),
      m_glInitialized(false),
      m_loggedFirstFrameRender(false),
      m_isDraggingVideo(false),
      m_lastDragPosition()
{
    setAutoFillBackground(false);

    setMinimumSize(400, 250);

    setFocusPolicy(Qt::StrongFocus);

    setCursor(
        Qt::OpenHandCursor
    );

    m_mpv = mpv_create();

    if (m_mpv == nullptr)
    {
        setError(
            QString::fromUtf8(
                "Failed to create mpv engine."
            )
        );

        return;
    }

    mpv_set_wakeup_callback(
        m_mpv,
        &MpvWidget::wakeupCallback,
        this
    );

    int result = MPV_ERROR_SUCCESS;

    result = mpv_set_option_string(
        m_mpv,
        "config",
        "no"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to disable mpv config:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "terminal",
        "no"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to disable mpv terminal:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "input-default-bindings",
        "no"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to disable mpv default bindings:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "input-vo-keyboard",
        "no"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to disable mpv keyboard input:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "vo",
        "libmpv"
    );

    if (result < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to set libmpv video output."
            )
        );

        mpv_terminate_destroy(m_mpv);

        m_mpv = nullptr;

        return;
    }

    result = mpv_set_option_string(
        m_mpv,
        "hwdec",
        "auto-copy"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to enable hardware decoding:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "vd-lavc-dr",
        "no"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to disable direct rendering:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "keep-open",
        "yes"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to enable keep-open:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "osc",
        "no"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to disable mpv OSC:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "scale",
        "ewa_lanczossharp"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv scale:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "cscale",
        "ewa_lanczossharp"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv chroma scale:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "dscale",
        "ewa_lanczossharp"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv downscale:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "scale-antiring",
        "0.7"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv scale antiring:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "cscale-antiring",
        "0.7"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv chroma scale antiring:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "dscale-antiring",
        "0.7"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv downscale antiring:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "correct-downscaling",
        "yes"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to enable mpv correct downscaling:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "sigmoid-upscaling",
        "yes"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to enable mpv sigmoid upscaling:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "linear-downscaling",
        "yes"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to enable mpv linear downscaling:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "dither",
        "fruit"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv dither algorithm:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "dither-depth",
        "auto"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv dither depth:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "temporal-dither",
        "yes"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to enable mpv temporal dither:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "deband",
        "yes"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to enable mpv deband:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "deband-iterations",
        "4"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv deband iterations:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "deband-threshold",
        "48"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv deband threshold:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "deband-range",
        "16"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv deband range:"
            << mpv_error_string(result);
    }

    result = mpv_set_option_string(
        m_mpv,
        "deband-grain",
        "12"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to set mpv deband grain:"
            << mpv_error_string(result);
    }

    result = mpv_initialize(m_mpv);

    if (result < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to initialize mpv: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(result)
            )
        );

        mpv_terminate_destroy(m_mpv);

        m_mpv = nullptr;

        return;
    }

    result = mpv_request_log_messages(
        m_mpv,
        "warn"
    );

    if (result < 0)
    {
        qWarning()
            << "Failed to request mpv log messages:"
            << mpv_error_string(result);
    }
}

MpvWidget::~MpvWidget()
{
    if (
        m_renderContext != nullptr
        &&
        context() != nullptr
    )
    {
        makeCurrent();

        mpv_render_context_set_update_callback(
            m_renderContext,
            nullptr,
            nullptr
        );

        mpv_render_context_free(
            m_renderContext
        );

        m_renderContext = nullptr;

        doneCurrent();
    }

    if (m_mpv != nullptr)
    {
        mpv_terminate_destroy(
            m_mpv
        );

        m_mpv = nullptr;
    }

    if (!m_temporaryPlaybackFile.isEmpty())
    {
        QFile::remove(
            m_temporaryPlaybackFile
        );
    }
}

bool MpvWidget::loadFile(
    const QString &filePath
)
{
    if (m_mpv == nullptr)
    {
        setError(
            QString::fromUtf8(
                "mpv engine is not available."
            )
        );

        return false;
    }

    if (filePath.trimmed().isEmpty())
    {
        setError(
            QString::fromUtf8(
                "Video file path is empty."
            )
        );

        return false;
    }

    const QString playbackFilePath =
        preparePlaybackFile(
            filePath
        );

    if (playbackFilePath.isEmpty())
    {
        return false;
    }

    const QByteArray encodedFilePath =
        playbackFilePath.toUtf8();

    const char *command[] =
    {
        "loadfile",
        encodedFilePath.constData(),
        "replace",
        nullptr
    };

    const int result = mpv_command(
        m_mpv,
        command
    );

    if (result < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to open video file: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(result)
            )
        );

        return false;
    }

    int paused = 0;

    const int pauseResult = mpv_set_property(
        m_mpv,
        "pause",
        MPV_FORMAT_FLAG,
        &paused
    );

    if (pauseResult < 0)
    {
        qWarning()
            << "Failed to start playback:"
            << mpv_error_string(pauseResult);
    }

    m_lastError.clear();

    m_loggedFirstFrameRender = false;

    resetZoom();

    update();

    return true;
}

bool MpvWidget::togglePause()
{
    if (m_mpv == nullptr)
    {
        setError(
            QString::fromUtf8(
                "mpv engine is not available."
            )
        );

        return true;
    }

    int paused = 0;

    const int getResult = mpv_get_property(
        m_mpv,
        "pause",
        MPV_FORMAT_FLAG,
        &paused
    );

    if (getResult < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to read playback state: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(getResult)
            )
        );

        return true;
    }

    paused = paused == 0 ? 1 : 0;

    const int setResult = mpv_set_property(
        m_mpv,
        "pause",
        MPV_FORMAT_FLAG,
        &paused
    );

    if (setResult < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to change playback state: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(setResult)
            )
        );

        return true;
    }

    m_lastError.clear();

    return paused != 0;
}

bool MpvWidget::seekRelative(
    double seconds
)
{
    if (m_mpv == nullptr)
    {
        setError(
            QString::fromUtf8(
                "mpv engine is not available."
            )
        );

        return false;
    }

    const QByteArray secondsText =
        QByteArray::number(
            seconds,
            'f',
            3
        );

    const char *command[] =
    {
        "seek",
        secondsText.constData(),
        "relative+exact",
        nullptr
    };

    const int result =
        mpv_command(
            m_mpv,
            command
        );

    if (result < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to seek: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(result)
            )
        );

        return false;
    }

    m_lastError.clear();

    return true;
}

bool MpvWidget::seekToRatio(
    double ratio
)
{
    const double mediaDuration =
        duration();

    if (mediaDuration <= 0.0)
    {
        return false;
    }

    if (ratio < 0.0)
    {
        ratio = 0.0;
    }
    else if (ratio > 1.0)
    {
        ratio = 1.0;
    }

    const QByteArray secondsText =
        QByteArray::number(
            mediaDuration * ratio,
            'f',
            3
        );

    const char *command[] =
    {
        "seek",
        secondsText.constData(),
        "absolute+exact",
        nullptr
    };

    const int result =
        mpv_command(
            m_mpv,
            command
        );

    if (result < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to seek to position: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(result)
            )
        );

        return false;
    }

    m_lastError.clear();

    return true;
}

bool MpvWidget::zoomBy(
    double amount
)
{
    return setZoomLevel(
        mpvDoubleProperty(
            "video-zoom",
            0.0
        )
        +
        amount
    );
}

bool MpvWidget::resetZoom()
{
    if (m_mpv == nullptr)
    {
        setError(
            QString::fromUtf8(
                "mpv engine is not available."
            )
        );

        return false;
    }

    if (
        !setMpvDoubleProperty(
            "video-zoom",
            0.0
        )
        ||
        !setMpvDoubleProperty(
            "video-pan-x",
            0.0
        )
        ||
        !setMpvDoubleProperty(
            "video-pan-y",
            0.0
        )
    )
    {
        return false;
    }

    m_lastError.clear();

    return true;
}

bool MpvWidget::setPlaybackSpeed(
    double speed
)
{
    speed =
        std::clamp(
            speed,
            0.25,
            8.0
        );

    return setMpvDoubleProperty(
        "speed",
        speed
    );
}

double MpvWidget::zoomPercent() const
{
    const double zoomLevel =
        mpvDoubleProperty(
            "video-zoom",
            0.0
        );

    return std::pow(
        2.0,
        zoomLevel
    )
    *
    100.0;
}

bool MpvWidget::isPaused() const
{
    if (m_mpv == nullptr)
    {
        return true;
    }

    int paused = 0;

    const int result = mpv_get_property(
        m_mpv,
        "pause",
        MPV_FORMAT_FLAG,
        &paused
    );

    if (result < 0)
    {
        return true;
    }

    return paused != 0;
}

bool MpvWidget::isMpvAvailable() const
{
    return m_mpv != nullptr;
}

double MpvWidget::position() const
{
    if (m_mpv == nullptr)
    {
        return 0.0;
    }

    double currentPosition = 0.0;

    const int result =
        mpv_get_property(
            m_mpv,
            "time-pos",
            MPV_FORMAT_DOUBLE,
            &currentPosition
        );

    if (result < 0)
    {
        return 0.0;
    }

    return currentPosition;
}

double MpvWidget::duration() const
{
    if (m_mpv == nullptr)
    {
        return 0.0;
    }

    double mediaDuration = 0.0;

    const int result =
        mpv_get_property(
            m_mpv,
            "duration",
            MPV_FORMAT_DOUBLE,
            &mediaDuration
        );

    if (result < 0)
    {
        return 0.0;
    }

    return mediaDuration;
}

QString MpvWidget::lastError() const
{
    return m_lastError;
}

void MpvWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(
        0.0F,
        0.0F,
        0.0F,
        1.0F
    );

    glClear(
        GL_COLOR_BUFFER_BIT
    );

    if (m_mpv == nullptr)
    {
        return;
    }

    mpv_opengl_init_params openGlInitializationParameters =
    {
        &MpvWidget::getProcAddress,
        nullptr
    };

    char *apiType =
        const_cast<char *>(
            MPV_RENDER_API_TYPE_OPENGL
        );

    mpv_render_param renderParameters[] =
    {
        {
            MPV_RENDER_PARAM_API_TYPE,
            apiType
        },
        {
            MPV_RENDER_PARAM_OPENGL_INIT_PARAMS,
            &openGlInitializationParameters
        },
        {
            MPV_RENDER_PARAM_INVALID,
            nullptr
        }
    };

    const int result = mpv_render_context_create(
        &m_renderContext,
        m_mpv,
        renderParameters
    );

    if (result < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to create mpv OpenGL render context: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(result)
            )
        );

        qCritical()
            << m_lastError;

        m_renderContext = nullptr;

        return;
    }

    mpv_render_context_set_update_callback(
        m_renderContext,
        &MpvWidget::renderUpdateCallback,
        this
    );

    m_glInitialized = true;

    qInfo()
        << "mpv OpenGL render context initialized";

    update();
}

void MpvWidget::paintGL()
{
    glClearColor(
        0.0F,
        0.0F,
        0.0F,
        1.0F
    );

    glClear(
        GL_COLOR_BUFFER_BIT
    );

    if (
        m_renderContext == nullptr
        ||
        !m_glInitialized
    )
    {
        return;
    }

    mpv_render_context_update(
        m_renderContext
    );

    const qreal pixelRatio =
        devicePixelRatioF();

    const int frameBufferWidth =
        static_cast<int>(
            static_cast<qreal>(width())
            *
            pixelRatio
        );

    const int frameBufferHeight =
        static_cast<int>(
            static_cast<qreal>(height())
            *
            pixelRatio
        );

    mpv_opengl_fbo frameBuffer =
    {
        static_cast<int>(
            defaultFramebufferObject()
        ),
        frameBufferWidth,
        frameBufferHeight,
        0
    };

    int flipVertically = 1;

    mpv_render_param renderParameters[] =
    {
        {
            MPV_RENDER_PARAM_OPENGL_FBO,
            &frameBuffer
        },
        {
            MPV_RENDER_PARAM_FLIP_Y,
            &flipVertically
        },
        {
            MPV_RENDER_PARAM_INVALID,
            nullptr
        }
    };

    const int renderResult = mpv_render_context_render(
        m_renderContext,
        renderParameters
    );

    if (renderResult < 0)
    {
        qWarning()
            << "Failed to render mpv frame:"
            << mpv_error_string(renderResult);

        return;
    }

    if (!m_loggedFirstFrameRender)
    {
        m_loggedFirstFrameRender = true;

        qInfo()
            << "Rendered first mpv frame"
            << frameBufferWidth
            << "x"
            << frameBufferHeight;
    }

    mpv_render_context_report_swap(
        m_renderContext
    );
}

void MpvWidget::resizeGL(
    int width,
    int height
)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
}

void MpvWidget::mousePressEvent(
    QMouseEvent *event
)
{
    if (
        event != nullptr
        &&
        event->button() == Qt::LeftButton
        &&
        zoomPercent() > 100.5
    )
    {
        m_isDraggingVideo = true;
        m_lastDragPosition = event->pos();

        setCursor(
            Qt::ClosedHandCursor
        );

        event->accept();

        return;
    }

    QOpenGLWidget::mousePressEvent(
        event
    );
}

void MpvWidget::mouseMoveEvent(
    QMouseEvent *event
)
{
    if (
        event != nullptr
        &&
        m_isDraggingVideo
    )
    {
        const QPoint delta =
            event->pos()
            -
            m_lastDragPosition;

        m_lastDragPosition =
            event->pos();

        const double panX =
            mpvDoubleProperty(
                "video-pan-x",
                0.0
            )
            +
            (
                static_cast<double>(
                    delta.x()
                )
                /
                static_cast<double>(
                    std::max(
                        1,
                        width()
                    )
                )
            )
            *
            2.0;

        const double panY =
            mpvDoubleProperty(
                "video-pan-y",
                0.0
            )
            +
            (
                static_cast<double>(
                    delta.y()
                )
                /
                static_cast<double>(
                    std::max(
                        1,
                        height()
                    )
                )
            )
            *
            2.0;

        setMpvDoubleProperty(
            "video-pan-x",
            std::clamp(
                panX,
                -3.0,
                3.0
            )
        );

        setMpvDoubleProperty(
            "video-pan-y",
            std::clamp(
                panY,
                -3.0,
                3.0
            )
        );

        update();

        event->accept();

        return;
    }

    QOpenGLWidget::mouseMoveEvent(
        event
    );
}

void MpvWidget::mouseReleaseEvent(
    QMouseEvent *event
)
{
    if (
        event != nullptr
        &&
        event->button() == Qt::LeftButton
        &&
        m_isDraggingVideo
    )
    {
        m_isDraggingVideo = false;

        setCursor(
            Qt::OpenHandCursor
        );

        event->accept();

        return;
    }

    QOpenGLWidget::mouseReleaseEvent(
        event
    );
}

void MpvWidget::wheelEvent(
    QWheelEvent *event
)
{
    if (event == nullptr)
    {
        return;
    }

    const int wheelDelta =
        event->angleDelta().y();

    if (wheelDelta == 0)
    {
        QOpenGLWidget::wheelEvent(
            event
        );

        return;
    }

    zoomBy(
        wheelDelta > 0
            ? 0.1
            : -0.1
    );

    update();

    event->accept();
}

void *MpvWidget::getProcAddress(
    void *context,
    const char *name
)
{
    Q_UNUSED(context);

    QOpenGLContext *openGlContext =
        QOpenGLContext::currentContext();

    if (
        openGlContext == nullptr
        ||
        name == nullptr
    )
    {
        return nullptr;
    }

    const QFunctionPointer functionPointer =
        openGlContext->getProcAddress(
            QByteArray(name)
        );

    return reinterpret_cast<void *>(
        functionPointer
    );
}

void MpvWidget::renderUpdateCallback(
    void *context
)
{
    MpvWidget *widget =
        static_cast<MpvWidget *>(
            context
        );

    if (widget == nullptr)
    {
        return;
    }

    QMetaObject::invokeMethod(
        widget,
        [widget]()
        {
            widget->update();
        },
        Qt::QueuedConnection
    );
}

void MpvWidget::wakeupCallback(
    void *context
)
{
    MpvWidget *widget =
        static_cast<MpvWidget *>(
            context
        );

    if (widget == nullptr)
    {
        return;
    }

    QMetaObject::invokeMethod(
        widget,
        [widget]()
        {
            widget->processMpvEvents();
        },
        Qt::QueuedConnection
    );
}

void MpvWidget::processMpvEvents()
{
    if (m_mpv == nullptr)
    {
        return;
    }

    while (true)
    {
        mpv_event *event =
            mpv_wait_event(
                m_mpv,
                0.0
            );

        if (
            event == nullptr
            ||
            event->event_id == MPV_EVENT_NONE
        )
        {
            break;
        }

        if (event->event_id == MPV_EVENT_LOG_MESSAGE)
        {
            const mpv_event_log_message *message =
                static_cast<mpv_event_log_message *>(
                    event->data
                );

            if (message != nullptr)
            {
                qInfo()
                    .noquote()
                    << QString::fromUtf8("[mpv:%1] %2")
                           .arg(
                               QString::fromUtf8(
                                   message->prefix
                                       != nullptr
                                       ? message->prefix
                                       : "unknown"
                               ),
                               QString::fromUtf8(
                                   message->text
                                       != nullptr
                                       ? message->text
                                       : ""
                               ).trimmed()
                           );
            }
        }
        else if (
            event->event_id == MPV_EVENT_VIDEO_RECONFIG
        )
        {
            qInfo()
                << "mpv video reconfigured";

            update();
        }
        else if (
            event->event_id == MPV_EVENT_FILE_LOADED
        )
        {
            qInfo()
                << "mpv file loaded";

            update();
        }
        else if (
            event->event_id == MPV_EVENT_END_FILE
        )
        {
            qInfo()
                << "mpv ended file";
        }
    }
}

double MpvWidget::mpvDoubleProperty(
    const char *propertyName,
    double defaultValue
) const
{
    if (
        m_mpv == nullptr
        ||
        propertyName == nullptr
    )
    {
        return defaultValue;
    }

    double value =
        defaultValue;

    const int result =
        mpv_get_property(
            m_mpv,
            propertyName,
            MPV_FORMAT_DOUBLE,
            &value
        );

    if (result < 0)
    {
        return defaultValue;
    }

    return value;
}

bool MpvWidget::setMpvDoubleProperty(
    const char *propertyName,
    double value
)
{
    if (
        m_mpv == nullptr
        ||
        propertyName == nullptr
    )
    {
        setError(
            QString::fromUtf8(
                "mpv engine is not available."
            )
        );

        return false;
    }

    const int result =
        mpv_set_property(
            m_mpv,
            propertyName,
            MPV_FORMAT_DOUBLE,
            &value
        );

    if (result < 0)
    {
        setError(
            QString::fromUtf8(
                "Failed to set video property: "
            )
            +
            QString::fromUtf8(
                mpv_error_string(result)
            )
        );

        return false;
    }

    m_lastError.clear();

    return true;
}

bool MpvWidget::setZoomLevel(
    double zoomLevel
)
{
    zoomLevel =
        std::clamp(
            zoomLevel,
            -1.0,
            4.0
        );

    if (
        !setMpvDoubleProperty(
            "video-zoom",
            zoomLevel
        )
    )
    {
        return false;
    }

    if (zoomLevel <= 0.0)
    {
        setMpvDoubleProperty(
            "video-pan-x",
            0.0
        );

        setMpvDoubleProperty(
            "video-pan-y",
            0.0
        );
    }

    update();

    return true;
}

QString MpvWidget::locateHelperExecutable(
    const QString &executableName
) const
{
    // 1. Prefer a copy bundled inside the .app (self-contained distribution).
    const QString bundledPath =
        QDir(
            QCoreApplication::applicationDirPath()
        )
        .filePath(
            QStringLiteral("../Helpers/")
            +
            executableName
        );

    if (
        QFileInfo(
            bundledPath
        ).isExecutable()
    )
    {
        return QDir::cleanPath(
            bundledPath
        );
    }

    // 2. Fall back to whatever is on PATH (dev runs from a terminal).
    const QString pathExecutable =
        QStandardPaths::findExecutable(
            executableName
        );

    if (!pathExecutable.isEmpty())
    {
        return pathExecutable;
    }

    // 3. GUI apps launched from Finder do not inherit the shell PATH,
    //    so probe the common package-manager locations directly.
    const QStringList commonDirectories =
    {
        QStringLiteral("/opt/homebrew/bin"),
        QStringLiteral("/usr/local/bin"),
        QStringLiteral("/opt/local/bin")
    };

    const QString fallbackExecutable =
        QStandardPaths::findExecutable(
            executableName,
            commonDirectories
        );

    if (!fallbackExecutable.isEmpty())
    {
        return fallbackExecutable;
    }

    return QString();
}

bool MpvWidget::needsHevcAviRemux(
    const QString &filePath
) const
{
    const QFileInfo fileInformation(
        filePath
    );

    if (
        fileInformation.suffix().compare(
            QStringLiteral("avi"),
            Qt::CaseInsensitive
        )
        != 0
    )
    {
        return false;
    }

    const QString ffprobePath =
        locateHelperExecutable(
            QStringLiteral("ffprobe")
        );

    if (ffprobePath.isEmpty())
    {
        return false;
    }

    QProcess ffprobeProcess;

    ffprobeProcess.start(
        ffprobePath,
        {
            QStringLiteral("-hide_banner"),
            QStringLiteral("-v"),
            QStringLiteral("error"),
            QStringLiteral("-select_streams"),
            QStringLiteral("v:0"),
            QStringLiteral("-show_entries"),
            QStringLiteral("stream=codec_name,codec_tag_string"),
            QStringLiteral("-of"),
            QStringLiteral("default=nk=1:nw=1"),
            filePath
        }
    );

    if (
        !ffprobeProcess.waitForFinished(
            5000
        )
    )
    {
        ffprobeProcess.kill();
        ffprobeProcess.waitForFinished();

        return false;
    }

    const QString probeOutput =
        QString::fromUtf8(
            ffprobeProcess.readAllStandardOutput()
        )
        +
        QString::fromUtf8(
            ffprobeProcess.readAllStandardError()
        );

    return
        probeOutput.contains(
            QStringLiteral("unknown"),
            Qt::CaseInsensitive
        )
        &&
        probeOutput.contains(
            QStringLiteral("x265"),
            Qt::CaseInsensitive
        );
}

bool MpvWidget::remuxHevcAviForPlayback(
    const QString &filePath,
    QString *remuxedFilePath
)
{
    if (remuxedFilePath == nullptr)
    {
        return false;
    }

    const QString ffmpegPath =
        locateHelperExecutable(
            QStringLiteral("ffmpeg")
        );

    if (ffmpegPath.isEmpty())
    {
        setError(
            QString::fromUtf8(
                "This AVI file uses a non-standard x265 tag and requires ffmpeg to remux the container before playback."
            )
        );

        return false;
    }

    if (!m_temporaryPlaybackFile.isEmpty())
    {
        QFile::remove(
            m_temporaryPlaybackFile
        );

        m_temporaryPlaybackFile.clear();
    }

    const QFileInfo fileInformation(
        filePath
    );

    const QString temporaryDirectory =
        QStandardPaths::writableLocation(
            QStandardPaths::TempLocation
        );

    const QString outputFilePath =
        QDir(
            temporaryDirectory
        )
        .filePath(
            QStringLiteral("MacVideoReader-%1-%2.mkv")
                .arg(
                    QDateTime::currentMSecsSinceEpoch()
                )
                .arg(
                    fileInformation.completeBaseName()
                )
        );

    QProcess ffmpegProcess;

    ffmpegProcess.start(
        ffmpegPath,
        {
            QStringLiteral("-hide_banner"),
            QStringLiteral("-y"),
            QStringLiteral("-fflags"),
            QStringLiteral("+genpts"),
            QStringLiteral("-c:v"),
            QStringLiteral("hevc"),
            QStringLiteral("-i"),
            filePath,
            QStringLiteral("-c"),
            QStringLiteral("copy"),
            outputFilePath
        }
    );

    if (
        !ffmpegProcess.waitForFinished(
            -1
        )
        ||
        ffmpegProcess.exitStatus() != QProcess::NormalExit
        ||
        ffmpegProcess.exitCode() != 0
        ||
        !QFileInfo::exists(
            outputFilePath
        )
    )
    {
        const QString ffmpegError =
            QString::fromUtf8(
                ffmpegProcess.readAllStandardError()
            )
            .trimmed();

        QFile::remove(
            outputFilePath
        );

        setError(
            QString::fromUtf8(
                "Failed to remux AVI/x265 container: "
            )
            +
            ffmpegError
        );

        return false;
    }

    m_temporaryPlaybackFile =
        outputFilePath;

    *remuxedFilePath =
        outputFilePath;

    qInfo()
        << "Prepared temporary HEVC remux for playback:"
        << outputFilePath;

    return true;
}

QString MpvWidget::preparePlaybackFile(
    const QString &filePath
)
{
    if (
        !needsHevcAviRemux(
            filePath
        )
    )
    {
        return filePath;
    }

    QString remuxedFilePath;

    if (
        !remuxHevcAviForPlayback(
            filePath,
            &remuxedFilePath
        )
    )
    {
        return QString();
    }

    return remuxedFilePath;
}

void MpvWidget::setError(
    const QString &message
)
{
    m_lastError = message;

    qWarning()
        << message;
}
