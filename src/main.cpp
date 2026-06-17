#include "MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QSurfaceFormat>

#include <clocale>

int main(
    int argc,
    char *argv[]
)
{
    QSurfaceFormat openGlFormat;

    openGlFormat.setRenderableType(
        QSurfaceFormat::OpenGL
    );

    openGlFormat.setVersion(
        3,
        2
    );

    openGlFormat.setProfile(
        QSurfaceFormat::CoreProfile
    );

    openGlFormat.setSwapBehavior(
        QSurfaceFormat::DoubleBuffer
    );

    openGlFormat.setSwapInterval(
        1
    );

    QSurfaceFormat::setDefaultFormat(
        openGlFormat
    );

    QApplication::setAttribute(
        Qt::AA_ShareOpenGLContexts
    );

    QApplication application(
        argc,
        argv
    );

    std::setlocale(
        LC_NUMERIC,
        "C"
    );

    QCoreApplication::setApplicationName(
        QStringLiteral(
            "GVPlayer"
        )
    );

    QCoreApplication::setApplicationVersion(
        QStringLiteral(
            "1.0.0"
        )
    );

    QCoreApplication::setOrganizationName(
        QStringLiteral(
            "Yassersky"
        )
    );

    QCoreApplication::setOrganizationDomain(
        QStringLiteral(
            "yassersky.com"
        )
    );

    MainWindow mainWindow;

    mainWindow.show();

    return application.exec();
}
