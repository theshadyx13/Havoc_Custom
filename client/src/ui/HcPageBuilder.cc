#include <Common.h>
#include <Havoc.h>
#include <ui/HcPageBuilder.h>

HcPageBuilder::HcPageBuilder(
    QWidget* parent
) : QWidget( parent ) {

    if ( objectName().isEmpty() ) {
        setObjectName( "HcPageBuilder" );
    }

    gridLayout_2 = new QGridLayout( this );
    gridLayout_2->setObjectName( "gridLayout_2" );

    LabelPayload = new QLabel( this );
    LabelPayload->setObjectName( "LabelPayload" );
    LabelPayload->setProperty( "HcLabelDisplay", "true" );

    ComboPayload = new QComboBox( this );
    ComboPayload->setObjectName( "ComboPayload" );

    horizontalSpacer = new QSpacerItem( 789, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    SplitterTopBottom = new QSplitter( this );
    SplitterTopBottom->setObjectName( "SplitterTopBottom" );
    SplitterTopBottom->setOrientation( Qt::Vertical );

    SplitterLeftRight = new QSplitter( SplitterTopBottom );
    SplitterLeftRight->setObjectName( "SplitterLeftRight" );
    SplitterLeftRight->setOrientation( Qt::Horizontal );

    StackedBuilders = new QStackedWidget( SplitterLeftRight );
    StackedBuilders->setObjectName( "StackedBuilders" );

    ProfileWidget = new QWidget( SplitterLeftRight );
    ProfileWidget->setObjectName( "ProfileWidget" );
    ProfileWidget->setContentsMargins( 0, 0, 0, 0 );

    gridLayout = new QGridLayout( ProfileWidget );
    gridLayout->setObjectName( "gridLayout" );

    ButtonGenerate = new QPushButton( ProfileWidget );
    ButtonGenerate->setObjectName( "ButtonGenerate" );
    ButtonGenerate->setProperty( "HcButton", "true" );
    ButtonGenerate->setProperty( "HcButtonPurple", "true" );

    ButtonSaveProfile = new QPushButton( ProfileWidget );
    ButtonSaveProfile->setObjectName( "ButtonSaveProfile" );
    ButtonSaveProfile->setProperty( "HcButton", "true" );

    ButtonLoadProfile = new QPushButton( ProfileWidget );
    ButtonLoadProfile->setObjectName( "ButtonLoadProfile" );
    ButtonLoadProfile->setProperty( "HcButton", "true" );

    ListProfiles = new QListWidget( ProfileWidget );
    ListProfiles->setObjectName( "ListProfiles" );

    TextBuildLog = new QTextEdit( SplitterTopBottom );
    TextBuildLog->setObjectName( "TextBuildLog" );
    TextBuildLog->setReadOnly( true );

    SplitterLeftRight->addWidget( StackedBuilders );
    SplitterLeftRight->addWidget( ProfileWidget );
    SplitterTopBottom->addWidget( SplitterLeftRight );
    SplitterTopBottom->addWidget( TextBuildLog );

    gridLayout->addWidget( ButtonGenerate,    0, 0, 1, 1 );
    gridLayout->addWidget( ButtonSaveProfile, 1, 0, 1, 1 );
    gridLayout->addWidget( ButtonLoadProfile, 2, 0, 1, 1 );
    gridLayout->addWidget( ListProfiles,      3, 0, 1, 1 );

    gridLayout_2->addWidget( LabelPayload,      0, 0, 1, 1 );
    gridLayout_2->addWidget( ComboPayload,      0, 1, 1, 1 );
    gridLayout_2->addWidget( SplitterTopBottom, 1, 0, 1, 3 );
    gridLayout_2->addItem( horizontalSpacer, 0, 2, 1, 1 );

    retranslateUi();

    connect( ButtonGenerate,    &QPushButton::clicked, this, &HcPageBuilder::PressedGenerate    );
    connect( ButtonSaveProfile, &QPushButton::clicked, this, &HcPageBuilder::PressedSaveProfile );
    connect( ButtonLoadProfile, &QPushButton::clicked, this, &HcPageBuilder::PressedLoadProfile );

    for ( auto& name : Havoc->Builders() ) {
        if ( Havoc->BuilderObject( name ).has_value() ) {
            AddBuilder( name, Havoc->BuilderObject( name ).value() );
        }
    }

    QMetaObject::connectSlotsByName( this );
}

auto HcPageBuilder::retranslateUi() -> void {
    setWindowTitle( "PageBuilder" );
    setStyleSheet( Havoc->getStyleSheet() );

    ComboPayload->addItem( "(no payload available)" );
    LabelPayload->setText( "Payload:" );
    ButtonGenerate->setText( "Generate" );
    ButtonSaveProfile->setText( "Save Profile" );
    ButtonLoadProfile->setText( "Load Profile" );

    SplitterLeftRight->setSizes( QList<int>() << 1374 << 436 );
}

auto HcPageBuilder::AddBuilder(
    const std::string&  name,
    const py11::object& object
) -> void {
    auto gil     = py11::gil_scoped_acquire();
    auto builder = Builder {
        .name    = name,
        .widget  = new QWidget
    };

    builder.widget->setObjectName( "HcPageBuilder.Builder." + QString( name.c_str() ) );

    try {
        builder.instance = object( name );
        builder.instance.attr( "_hc_main" )();
    } catch ( py11::error_already_set &eas ) {
        py11::gil_scoped_release release;
        Helper::MessageBox(
            QMessageBox::Icon::Critical,
            "Builder loading error",
            std::string( eas.what() )
        );
        return;
    }

    if ( Builders.empty() ) {
        ComboPayload->clear();
        ComboPayload->setEnabled( true );
    }

    ComboPayload->addItem( builder.name.c_str() );
    StackedBuilders->addWidget( builder.widget );

    Builders.push_back( builder );
}

auto HcPageBuilder::RefreshBuilders() -> void {
    //
    // we don't hold the GIL
    //
    if ( ! PyGILState_Check() ) {
        return;
    }

    for ( auto& builder : Builders ) {
        try {
            builder.instance.attr( "refresh" )();
        } catch ( py11::error_already_set &eas ) {
            py11::gil_scoped_release release;
            spdlog::error( "[py11::error_already_set] failed to refresh builder \"{}\": \n{}", builder.name, eas.what() );
            return;
        } catch ( std::exception& e ) {
            spdlog::error( "[std::exception] failed to refresh builder \"{}\": \n{}", builder.name, e.what() );
        }

        py11::gil_scoped_release release;
    }
}

auto HcPageBuilder::PressedGenerate() -> void
{
    auto result = httplib::Result();
    auto data   = json();
    auto body   = json();
    auto config = json();
    auto name   = std::string();
    auto found  = false;

    TextBuildLog->clear();

    if ( Builders.empty() ) {
        Helper::MessageBox(
            QMessageBox::Critical,
            "Payload build failure",
            "Failed to build payload: no builder registered"
        );

        return;
    }

    for ( auto& builder : Builders ) {
        if ( builder.name == ComboPayload->currentText().toStdString() ) {
            auto gil = py11::gil_scoped_acquire();

            found = true;
            name  = builder.name;

            try {
                /* sanity check input */
                if ( py11::hasattr( builder.instance, "sanity_check" ) ) {
                    if ( ! builder.instance.attr( "sanity_check" )().cast<bool>() ) {
                        spdlog::debug( "sanity check failed. exit and dont send request" );
                        return;
                    }
                }

                if ( ( config = builder.instance.attr( "generate" )() ).empty() ) {
                    return;
                }
            } catch ( py11::error_already_set &eas ) {
                spdlog::error( "failed to refresh builder \"{}\": \n{}", builder.name, eas.what() );
                return;
            }

            break;
        }
    }

    if ( ! found ) {
        Helper::MessageBox(
            QMessageBox::Critical,
            "Payload build failure",
            QString( "Failed to build payload: %1" ).arg( ComboPayload->currentText() ).toStdString()
        );
        return;
    }

    data = {
        { "name",   name   },
        { "config", config },
    };

    //
    // TODO: Need a worker thread here for HTTP requests
    //
    if ( ( result = Havoc->ApiSend( "/api/agent/build", data ) ) ) {
        if ( result->status != 200 ) {
            if ( ( data = json::parse( result->body ) ).is_discarded() ) {
                goto InvalidServerResponseError;
            }

            if ( ! data.contains( "error" ) ) {
                goto InvalidServerResponseError;
            }

            if ( ! data[ "error" ].is_string() ) {
                goto InvalidServerResponseError;
            }

            Helper::MessageBox(
                QMessageBox::Critical,
                "Payload build failure",
                QString( "Failed to build payload \"%1\": %2" ).arg( name.c_str() ).arg( data[ "error" ].get<std::string>().c_str() ).toStdString()
            );
        } else {
            auto Dialog = QFileDialog();
            auto Path   = QString();
            auto File   = QFile();

            if ( ( data = json::parse( result->body ) ).is_discarded() ) {
                goto InvalidServerResponseError;
            }

            if ( data.contains( "filename" ) ) {
                if ( data[ "filename" ].is_string() ) {
                    name = data[ "filename" ].get<std::string>();
                }
            }

            Dialog.setDirectory( QDir::homePath() );
            Dialog.selectFile( name.c_str() );

            if ( Dialog.exec() == QFileDialog::Accepted ) {
                Path = Dialog.selectedFiles().value( 0 );

                File.setFileName( Path );
                if ( File.open( QIODevice::ReadWrite ) ) {

                    if ( data.contains( "payload" ) ) {
                        if ( data[ "payload" ].is_string() ) {
                            File.write( QByteArray::fromBase64( data[ "payload" ].get<std::string>().c_str() ) );

                            Helper::MessageBox(
                                QMessageBox::Information,
                                "Payload build",
                                QString( "Saved payload under:\n%1" ).arg( Path ).toStdString()
                            );
                        }
                    }

                } else {
                    Helper::MessageBox(
                        QMessageBox::Critical,
                        "Payload build failure",
                        QString( "Failed to write payload to \"%1\": %2" ).arg( Path ).arg( File.errorString() ).toStdString()
                    );
                }
            }

            RefreshBuilders();
            return;
        }
    }

InvalidServerResponseError:
    Helper::MessageBox(
        QMessageBox::Critical,
        "Payload build failure",
        QString( "Failed to build payload \"%1\": Invalid response from the server" ).arg( name.c_str() ).toStdString()
    );

    RefreshBuilders();
}

auto HcPageBuilder::PressedSaveProfile() -> void
{

}

auto HcPageBuilder::PressedLoadProfile() -> void
{

}
