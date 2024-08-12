#include <Havoc.h>
#include <ui/HcPageListener.h>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

HcListenerItem::HcListenerItem( QWidget* parent ) : QWidget( parent ) {
    setObjectName( "HcStatusItem" );

    GridLayout = new QGridLayout( this );
    GridLayout->setObjectName( "GridLayout" );
    GridLayout->setSpacing( 0 );
    GridLayout->setContentsMargins( 0, 0, 0, 0 );

    LabelStatus = new QLabel( this );
    LabelStatus->setObjectName( "LabelStatus" );

    auto policy = QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
    policy.setHorizontalStretch( 0 );
    policy.setVerticalStretch( 0 );
    policy.setHeightForWidth( LabelStatus->sizePolicy().hasHeightForWidth() );

    LabelStatus->setSizePolicy( policy );
    LabelStatus->setAlignment( Qt::AlignCenter );

    GridLayout->addWidget( LabelStatus, 0, 0, 1, 1 );

    QMetaObject::connectSlotsByName( this );
}

auto HcListenerItem::setStatus(
    const QString& string
) const -> void {
    auto text = QString( string );

    if ( text.isEmpty() ) {
        //
        // if the string status is emtpy then do no
        // change anything about the current status
        //
        return;
    }

    if ( text.startsWith( "+" ) ) {
        LabelStatus->setProperty( "HcLabelDisplay", "green" );
        text.remove( 0, 1 );
    } else if ( text.startsWith( "-" ) ) {
        LabelStatus->setProperty( "HcLabelDisplay", "red" );
        text.remove( 0, 1 );
    } else if ( text.startsWith( "*" ) ) {
        LabelStatus->setProperty( "HcLabelDisplay", "tag" );
        text.remove( 0, 1 );
    }

    LabelStatus->setText( text );
    LabelStatus->style()->unpolish( LabelStatus );
    LabelStatus->style()->polish( LabelStatus );
}

HcListenerItem::HcListenerItem(
    const QString& text,
    QWidget*       parent
) : HcListenerItem( parent ) {
    setStatus( text );
}

HcPageListener::HcPageListener() {
    if ( objectName().isEmpty() ) {
        setObjectName( QString::fromUtf8( "PageListener" ) );
    }

    gridLayout = new QGridLayout( this );
    gridLayout->setObjectName( QString::fromUtf8( "gridLayout" ) );

    ActiveLabel = new QLabel( this );
    ActiveLabel->setObjectName( QString::fromUtf8("LabelDisplayListenerActive" ) );
    ActiveLabel->setProperty( "HcLabelDisplay", "true" );

    horizontalSpacer  = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ButtonNewListener = new QPushButton( this );
    ButtonNewListener->setObjectName( QString::fromUtf8( "ButtonNewListener" ) );
    ButtonNewListener->setProperty( "HcButton", "true" );

    Splitter = new QSplitter( this );
    Splitter->setObjectName( QString::fromUtf8( "Splitter" ) );
    Splitter->setOrientation( Qt::Vertical );

    TableWidget = new QTableWidget( Splitter );
    TableWidget->setObjectName( QString::fromUtf8( "TableWidget" ) );

    TitleName   = new QTableWidgetItem( "Name" );
    TitleType   = new QTableWidgetItem( "Type" );
    TitleHost   = new QTableWidgetItem( "Host" );
    TitlePort   = new QTableWidgetItem( "Port" );
    TitleStatus = new QTableWidgetItem( "Status" );

    if ( TableWidget->columnCount() < 5 ) {
        TableWidget->setColumnCount( 5 );
    }

    TableWidget->setHorizontalHeaderItem( 0, TitleName   );
    TableWidget->setHorizontalHeaderItem( 1, TitleType   );
    TableWidget->setHorizontalHeaderItem( 2, TitleHost   );
    TableWidget->setHorizontalHeaderItem( 3, TitlePort   );
    TableWidget->setHorizontalHeaderItem( 4, TitleStatus );

    /* table settings */
    TableWidget->setEnabled( true );
    TableWidget->setShowGrid( false );
    TableWidget->setSortingEnabled( false );
    TableWidget->setWordWrap( true );
    TableWidget->setCornerButtonEnabled( true );
    TableWidget->horizontalHeader()->setVisible( true );
    TableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    TableWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    TableWidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
    TableWidget->horizontalHeader()->setStretchLastSection( true );
    TableWidget->verticalHeader()->setVisible( false );
    TableWidget->setFocusPolicy( Qt::NoFocus );
    
    TabWidget = new QTabWidget( Splitter );
    TabWidget->setObjectName( QString::fromUtf8( "TabWidget" ) );
    TabWidget->setMovable( true );
    TabWidget->setTabsClosable( true );
    TabWidget->tabBar()->setProperty( "HcTab", "true" );

    Splitter->addWidget( TableWidget );
    Splitter->addWidget( TabWidget );
    Splitter->handle( 1 )->setEnabled( false ); /* disabled by default */

    gridLayout->addWidget( ActiveLabel, 0, 0, 1, 1 );
    gridLayout->addItem( horizontalSpacer, 0, 1, 1, 1 );
    gridLayout->addWidget( ButtonNewListener, 0, 2, 1, 1 );
    gridLayout->addWidget( Splitter, 1, 0, 1, 3 );

    retranslateUi();

    QObject::connect( ButtonNewListener, &QPushButton::clicked, this, &HcPageListener::buttonAddListener );
    QObject::connect( TableWidget, &QTableWidget::customContextMenuRequested, this, &HcPageListener::handleListenerContextMenu );
    QObject::connect( TabWidget->tabBar(), &QTabBar::tabCloseRequested, this, &HcPageListener::tabCloseRequested );

    QMetaObject::connectSlotsByName( this );
}

auto HcPageListener::retranslateUi() -> void {
    setStyleSheet( Havoc->StyleSheet() );

    setWindowTitle( "PageListener" );
    ActiveLabel->setText( "Active: 0" );
    ButtonNewListener->setText( "Add Listener" );
}

auto HcPageListener::buttonAddListener() -> void {
    auto Dialog = new HcListenerDialog;

    Dialog->start();

    if ( Dialog->getCloseState() != Closed ) {
        delete Dialog;
    }
}

auto HcPageListener::addListener(
    const json& data
) -> void {
    auto name     = QString();
    auto type     = QString();
    auto host     = QString();
    auto port     = QString();
    auto status   = QString();
    auto listener = new HcListener();

    if ( data.contains( "name" ) ) {
        if ( data[ "name" ].is_string() ) {
            name = data[ "name" ].get<std::string>().c_str();
        } else {
            spdlog::error( "invalid listener: \"name\" is not string" );
            return;
        }
    } else {
        spdlog::error( "invalid listener: \"name\" is not found" );
        return;
    }

    if ( data.contains( "protocol" ) ) {
        if ( data[ "protocol" ].is_string() ) {
            type = data[ "protocol" ].get<std::string>().c_str();
        } else {
            spdlog::error( "invalid listener: \"protocol\" is not string" );
            return;
        }
    } else {
        spdlog::error( "invalid listener: \"protocol\" is not found" );
        return;
    }

    if ( data.contains( "host" ) ) {
        if ( data[ "host" ].is_string() ) {
            host = data[ "host" ].get<std::string>().c_str();
        } else {
            spdlog::error( "invalid listener: \"host\" is not string" );
            return;
        }
    } else {
        spdlog::error( "invalid listener: \"host\" is not found" );
        return;
    }

    if ( data.contains( "port" ) ) {
        if ( data[ "port" ].is_string() ) {
            port = data[ "port" ].get<std::string>().c_str();
        } else {
            spdlog::error( "invalid listener: \"port\" is not string" );
            return;
        }
    } else {
        spdlog::error( "invalid listener: \"port\" is not found" );
        return;
    }

    if ( data.contains( "status" ) ) {
        if ( data[ "status" ].is_string() ) {
            status = data[ "status" ].get<std::string>().c_str();
        } else {
            spdlog::error( "invalid listener: \"status\" is not string" );
            return;
        }
    } else {
        spdlog::error( "invalid listener: \"status\" is not found" );
        return;
    }

    listener->Name   = new QTableWidgetItem( name );
    listener->Type   = new QTableWidgetItem( type );
    listener->Host   = new QTableWidgetItem( host );
    listener->Port   = new QTableWidgetItem( port );
    listener->Status = new HcListenerItem();
    listener->Logger = new QTextEdit();

    listener->Name->setFlags( listener->Name->flags() ^ Qt::ItemIsEditable );
    listener->Type->setFlags( listener->Type->flags() ^ Qt::ItemIsEditable );
    listener->Host->setFlags( listener->Host->flags() ^ Qt::ItemIsEditable );
    listener->Port->setFlags( listener->Port->flags() ^ Qt::ItemIsEditable );
    listener->Logger->setProperty( "HcConsole", "true" );
    listener->Logger->setReadOnly( true );

    if ( TableWidget->rowCount() < 1 ) {
        TableWidget->setRowCount( 1 );
    } else {
        TableWidget->setRowCount( TableWidget->rowCount() + 1 );
    }

    listener->Status->setStatus( status );

    const bool isSortingEnabled = TableWidget->isSortingEnabled();
    TableWidget->setSortingEnabled( false );
    TableWidget->setItem( TableWidget->rowCount() - 1, 0, listener->Name );
    TableWidget->setItem( TableWidget->rowCount() - 1, 1, listener->Type );
    TableWidget->setItem( TableWidget->rowCount() - 1, 2, listener->Host );
    TableWidget->setItem( TableWidget->rowCount() - 1, 3, listener->Port );
    TableWidget->setCellWidget( TableWidget->rowCount() - 1, 4, listener->Status );
    TableWidget->setSortingEnabled( isSortingEnabled );

    TableEntries.push_back( listener );
    Listeners.push_back( data );

    /* increase the number of listeners */
    ListenersRunning++;

    updateListenersRunningLabel( ListenersRunning );
}

auto HcPageListener::updateListenersRunningLabel(
    int value
) const -> void {
    ActiveLabel->setText( QString( "Active: %1" ).arg( value ) );
}

auto HcPageListener::addListenerLog(
    const std::string& name,
    const std::string& log
) -> void {
    for ( auto& listener : TableEntries ) {
        if ( listener->Name->text().toStdString() == name ) {
            listener->Logger->append( log.c_str() );
            break;
        }
    }
}

auto HcPageListener::setListenerStatus(
    const std::string& name,
    const std::string& status
) -> void {
    for ( auto& listener : TableEntries ) {
        if ( listener->Name->text().toStdString() == name ) {
            listener->Status->setStatus( status.c_str() );
            break;
        }
    }
}

auto HcPageListener::handleListenerContextMenu(
    const QPoint& pos
) -> void {
    auto Menu = new QMenu( this );
    auto Name = QString();

    /* check if we point to a session table item/agent */
    if ( ! TableWidget->itemAt( pos ) ) {
        return;
    }

    Name = TableWidget->item( TableWidget->currentRow(), 0 )->text();

    Menu->addAction( "Logs" );
    Menu->addAction( "Stop" );
    Menu->addAction( "Restart" );
    Menu->addAction( "Edit" );

    if ( auto action = Menu->exec( TableWidget->horizontalHeader()->viewport()->mapToGlobal( pos ) ) ) {
        for ( auto& listener : TableEntries ) {
            if ( listener->Name->text().compare( Name ) == 0 ) {

                if ( action->text().compare( "Logs" ) == 0 ) {
                    if ( TabWidget->count() == 0 ) {
                        Splitter->setSizes( QList<int>() << 200 << 220 );
                        Splitter->handle( 1 )->setEnabled( true );
                        Splitter->handle( 1 )->setCursor( Qt::SplitVCursor );
                    }

                    TabWidget->addTab( listener->Logger, "[Logger] " + listener->Name->text() );
                } else if ( action->text().compare( "Stop" ) == 0 ) {
                    auto error = listener->stop();

                    if ( error.has_value() ) {
                        spdlog::debug( "listener failure with {}: {}", listener->Name->text().toStdString(), error.value() );
                    }
                } else if ( action->text().compare( "Restart" ) == 0 ) {

                } else if ( action->text().compare( "Edit" ) == 0 ) {

                } else {
                    spdlog::debug( "[ERROR] invalid action from selected listener menu" );
                }

                break;
            }
        }
    }
}

auto HcPageListener::tabCloseRequested(
    int index
) const -> void {
    if ( index == -1 ) {
        return;
    }

    TabWidget->removeTab( index );

    if ( TabWidget->count() == 0 ) {
        Splitter->setSizes( QList<int>() << 0 );
        Splitter->handle( 1 )->setEnabled( false );
        Splitter->handle( 1 )->setCursor( Qt::ArrowCursor );
    }
}

auto HcListener::stop() -> std::optional<std::string>
{
    auto result = httplib::Result();
    auto error  = std::string();
    auto name   = Name->text().toStdString();
    auto data   = json();

    result = Havoc->ApiSend( "/api/listener/stop", {
        { "name", name }
    } );

    if ( result->status != 200 ) {
        if ( result->body.empty() ) {
            goto ERROR_SERVER_RESPONSE;
        }

        if ( ( data = json::parse( result->body ) ).is_discarded() ) {
            goto ERROR_SERVER_RESPONSE;
        }

        if ( ! data.contains( "error" ) ) {
            goto ERROR_SERVER_RESPONSE;
        }

        if ( ! data[ "error" ].is_string() ) {
            goto ERROR_SERVER_RESPONSE;
        }

        return data[ "error" ].get<std::string>();
    }

    return {};

ERROR_SERVER_RESPONSE:
    return std::optional<std::string>( "invalid response from the server" );
}

auto HcListener::start() -> std::optional<std::string>
{
    return {};
}

auto HcListener::restart() -> std::optional<std::string>
{
    return {};
}

auto HcListener::edit() -> std::optional<std::string>
{
    return {};
}