#include <ui/HcTheme.h>

HcTheme::HcTheme( QWidget *parent ) {}
HcTheme::~HcTheme() = default;

HcDeclareColorMethod( Background )
HcDeclareColorMethod( CurrentLine )
HcDeclareColorMethod( Foreground )
HcDeclareColorMethod( Comment )
HcDeclareColorMethod( Cyan )
HcDeclareColorMethod( Green )
HcDeclareColorMethod( Orange )
HcDeclareColorMethod( Pink )
HcDeclareColorMethod( Purple )
HcDeclareColorMethod( Red )
HcDeclareColorMethod( Yellow )

auto HcTheme::getHtmlColorStart(
    const QColor& color
) -> QString {
    return R"(<span style="color: )" + color.name() + R"(;">)";
}

auto HcTheme::getHtmlEnd() -> QString
{
    return R"(</span>)";
}