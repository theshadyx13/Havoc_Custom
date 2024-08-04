#include <Havoc.h>
#include <ui/HcLineEdit.h>

HcLineEdit::HcLineEdit( QWidget* parent ) : QWidget( parent ) {
    Layout = new QHBoxLayout( this );
    Layout->setSpacing( 0 );
    Layout->setContentsMargins( 0, 0, 0, 0 );

    Label = new QLabel;
    Label->setProperty( "HcLineEdit", "true" );
    Label->setFixedHeight( 26 );

    Input = new QLineEdit;
    Input->setFixedHeight( 26 );
    Input->setProperty( "HcLineEdit", "true" );

    Layout->addWidget( Label );
    Layout->addWidget( Input );

    setLayout( Layout );
}

auto HcLineEdit::setLabelText(
    const QString& text
) -> void {
    Label->setText( text );
}

auto HcLineEdit::setInputText(
    const QString& text
) -> void {
    Input->setText( text );
}

auto HcLineEdit::setValidator(
    const QValidator* val
) -> void {
    Input->setValidator( val );
}

HcLineEdit::~HcLineEdit() {
    delete Layout;
    delete Label;
    delete Input;
}

auto HcLineEdit::text() -> QString {
    return Input->text();
};

HcLabelNamed::HcLabelNamed(
    const QString& title,
    const QString& text,
    QWidget*       parent
) : QWidget( parent ) {
    Layout = new QHBoxLayout( this );
    Layout->setSpacing( 0 );
    Layout->setContentsMargins( 0, 0, 0, 0 );

    Label = new QLabel;
    Label->setFixedHeight( 26 );
    Label->setProperty( "HcLabelNamed", "title" );
    Label->setText( title );

    Text = new QLabel;
    Text->setFixedHeight( 26 );
    Text->setProperty( "HcLabelNamed", "text" );
    Text->setText( text );

    Layout->addWidget( Label );
    Layout->addWidget( Text );

    setLayout( Layout );
}
