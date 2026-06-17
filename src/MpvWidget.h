#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPoint>
#include <QString>

struct mpv_handle;
struct mpv_render_context;

class MpvWidget final
    : public QOpenGLWidget,
      protected QOpenGLFunctions
{
public:
    explicit MpvWidget(QWidget *parent = nullptr);

    ~MpvWidget() override;

    bool loadFile(const QString &filePath);

    bool togglePause();

    bool seekRelative(
        double seconds
    );

    bool seekToRatio(
        double ratio
    );

    bool zoomBy(
        double amount
    );

    bool resetZoom();

    bool setPlaybackSpeed(
        double speed
    );

    double zoomPercent() const;

    bool isPaused() const;

    bool isMpvAvailable() const;

    double position() const;

    double duration() const;

    QString lastError() const;

protected:
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int width, int height) override;

    void mousePressEvent(
        QMouseEvent *event
    ) override;

    void mouseMoveEvent(
        QMouseEvent *event
    ) override;

    void mouseReleaseEvent(
        QMouseEvent *event
    ) override;

    void wheelEvent(
        QWheelEvent *event
    ) override;

private:
    static void *getProcAddress(
        void *context,
        const char *name
    );

    static void renderUpdateCallback(
        void *context
    );

    static void wakeupCallback(
        void *context
    );

    void processMpvEvents();

    double mpvDoubleProperty(
        const char *propertyName,
        double defaultValue
    ) const;

    bool setMpvDoubleProperty(
        const char *propertyName,
        double value
    );

    bool setZoomLevel(
        double zoomLevel
    );

    QString locateHelperExecutable(
        const QString &executableName
    ) const;

    bool needsHevcAviRemux(
        const QString &filePath
    ) const;

    bool remuxHevcAviForPlayback(
        const QString &filePath,
        QString *remuxedFilePath
    );

    QString preparePlaybackFile(
        const QString &filePath
    );

    void setError(
        const QString &message
    );

    mpv_handle *m_mpv;

    mpv_render_context *m_renderContext;

    QString m_lastError;

    QString m_temporaryPlaybackFile;

    bool m_glInitialized;

    bool m_loggedFirstFrameRender;

    bool m_isDraggingVideo;

    QPoint m_lastDragPosition;
};

#endif
