#include <Havoc.h>
#include <core/HcHelper.h>

auto Helper::MessageBox(
    QMessageBox::Icon icon,
    std::string       title,
    std::string       text
) -> void {
    auto box = QMessageBox();

    box.setStyleSheet( Havoc->getStyleSheet() );
    box.setWindowTitle( title.c_str() );
    box.setText( text.c_str() );
    box.setIcon( icon );
    box.exec();
}

auto Helper::FileRead(
    const QString& FilePath
) -> QByteArray {
    auto Content = QByteArray( );
    auto File    = QFile( FilePath );

    if ( FilePath[ 0 ] != ':' ) {
        if ( ! QFile::exists( FilePath ) ) {
            spdlog::error( "Failed to find file: {}", FilePath.toStdString() );
            return nullptr;
        }
    }

    /* try to open file */
    if ( File.open( QIODevice::ReadOnly ) ) {
        Content = File.readAll();
        File.close();
    } else {
        spdlog::error( "Failed to open file: {}", File.errorString().toStdString() );
    }

    return Content;
}

auto Helper::GrayScaleIcon(
    QImage image
) -> QIcon {
    auto im = image.convertToFormat( QImage::Format_ARGB32 );

    for ( int y = 0; y < im.height(); ++y ) {
        auto scanLine = ( QRgb* ) im.scanLine( y );

        for ( int x = 0; x < im.width(); ++x ) {
            auto pixel = *scanLine;
            auto ci    = qGray( pixel );
            *scanLine  = qRgba( ci, ci, ci, qAlpha( pixel ) / 3 );

            ++scanLine;
        }
    }

    return QPixmap::fromImage( im );
}

