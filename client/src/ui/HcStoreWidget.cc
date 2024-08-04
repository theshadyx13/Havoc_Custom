#include <Havoc.h>
#include <ui/HcStoreWidget.h>

#include <QNetworkAccessManager>
#include <QMouseEvent>
#include <QHBoxLayout>

//
// a small QTextEdit class that handles remote resources such
// as http images and embeds them into the markdown/html renderer
//

class HcTextEdit : public QTextEdit {
public:
    explicit HcTextEdit( QWidget* parent = nullptr ) : QTextEdit( parent ) {}

    QVariant loadResource(
        int         type,
        const QUrl& name
    ) override {
        spdlog::debug( "name = {}", name.toString().toStdString() );
        if ( type == QTextDocument::ImageResource ) {
            //
            // is remote/link image resource
            //
            if ( ! name.isLocalFile() ) {
                auto client = httplib::Client( name.scheme().toStdString() + "://" + name.host().toStdString() );
                auto result = httplib::Result();

                result = client.Get( name.path().toStdString() );

                if ( HttpErrorToString( result.error() ).has_value() ) {
                    spdlog::error( "HcTextEdit error: failed to retrieve {} resource: {}", name.toString().toStdString(), HttpErrorToString( result.error() ).value() );
                    return {};
                }

                return QByteArray( result->body.c_str(), result->body.length() );
            }
        }

        return QTextEdit::loadResource( type, name );
    }
};

class HcMarketPluginItem : public QWidget
{
public:
    QGridLayout* gridLayout       = { 0 };
    QLabel*      LabelDescription = { 0 };
    QLabel*      LabelName        = { 0 };
    QGridLayout* GridLayout       = { 0 };
    QLabel*      LabelInstalled   = { 0 };

    explicit HcMarketPluginItem(
        const QString& name,
        const QString& description,
        QWidget*       parent = nullptr
    ) : QWidget( parent ) {
        gridLayout = new QGridLayout( this );
        gridLayout->setObjectName( "gridLayout" );

        LabelName = new QLabel( this );
        LabelName->setObjectName( "LabelName" );

        LabelDescription = new QLabel( this );
        LabelDescription->setObjectName( "LabelDescription" );

        auto policy = QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
        policy.setHorizontalStretch( 0 );
        policy.setVerticalStretch( 0 );
        policy.setHeightForWidth( LabelDescription->sizePolicy().hasHeightForWidth() );
        LabelDescription->setSizePolicy( policy );

        GridLayout = new QGridLayout();
        GridLayout->setObjectName( u"GridLayout" );

        gridLayout->addWidget( LabelDescription, 1, 0, 1, 1 );
        gridLayout->addWidget( LabelName, 0, 0, 1, 1 );
        gridLayout->addLayout( GridLayout, 0, 1, 2, 1 );
        gridLayout->setColumnStretch( 0, 1 );

        LabelDescription->setText( description );
        LabelName->setText( std::format( "<p><span style=\" font-size:12pt;\">{}</span></p>", name.toStdString() ).c_str() );

        setNotInstalled();

        QMetaObject::connectSlotsByName( this );
    }

    void setInstalled() {
        if ( LabelInstalled ) {
            setNotInstalled();
        }

        LabelInstalled = new QLabel( this );
        LabelInstalled->setObjectName( "LabelInstalled" );
        LabelInstalled->setText( "Installed" );
        LabelInstalled->setProperty( "HcLabelDisplay", "green" );
        LabelInstalled->setFixedHeight( 25 );

        GridLayout->addWidget( LabelInstalled, 0, 0, 1, 1 );
    }

    void setNotInstalled() {
        if ( LabelInstalled ) {
            GridLayout->removeWidget( LabelInstalled );
            delete LabelInstalled;
            LabelInstalled = nullptr;
        }
    }
};

class HcListWidgetDeselect : public QListWidget {

public:
    explicit HcListWidgetDeselect( QWidget* parent ) : QListWidget( parent ) { }

    void mousePressEvent( QMouseEvent *event ) override {
        QListWidget::mousePressEvent( event );

        if ( ! indexAt( event->pos() ).isValid() ) {
            clearSelection();
        }
    }
};

HcStoreWidget::HcStoreWidget( QWidget* parent ) : QWidget( parent ) {
    setObjectName( QString::fromUtf8( "HcStoreWidget" ) );

    gridLayout_3 = new QGridLayout( this );
    gridLayout_3->setObjectName( "gridLayout_3" );
    gridLayout_3->setContentsMargins( 0, 0, 0, 0 );

    splitter = new QSplitter( this );
    splitter->setObjectName( "splitter" );
    splitter->setOrientation( Qt::Horizontal );

    MarketPlaceWidget = new QWidget( splitter );
    MarketPlaceWidget->setObjectName( "MarketPlaceWidget" );

    gridLayout = new QGridLayout( MarketPlaceWidget );
    gridLayout->setObjectName( "gridLayout" );

    MarketPlaceSearch = new QLineEdit( MarketPlaceWidget );
    MarketPlaceSearch->setObjectName( "MarketPlaceSearch" );
    MarketPlaceSearch->setProperty( "HcLineEdit", "dark" );

    MarketPlaceList = new HcListWidgetDeselect( MarketPlaceWidget );
    MarketPlaceList->setObjectName( "MarketPlaceList" );
    MarketPlaceList->setProperty( "HcListWidget", "dark" );
    MarketPlaceList->setFocusPolicy( Qt::NoFocus );

    PluginViewStack = new QStackedWidget( splitter );
    PluginViewStack->setObjectName( "PluginViewStack" );

    splitter->addWidget( MarketPlaceWidget );
    splitter->addWidget( PluginViewStack );

    gridLayout->addWidget( MarketPlaceSearch, 0, 0, 1, 1 );
    gridLayout->addWidget( MarketPlaceList, 1, 0, 1, 1 );

    gridLayout_3->addWidget( splitter, 0, 0, 1, 1 );

    PluginWorker.Thread = new QThread;
    PluginWorker.Worker = new HcStorePluginWorker;
    PluginWorker.Worker->moveToThread( PluginWorker.Thread );

    connect( PluginWorker.Thread, &QThread::started, PluginWorker.Worker, &HcStorePluginWorker::run );
    connect( this, &HcStoreWidget::RegisterRepository, PluginWorker.Worker, &HcStorePluginWorker::RegisterRepository );
    connect( PluginWorker.Worker, &HcStorePluginWorker::AddPlugin, this, &HcStoreWidget::AddPlugin );

    connect( MarketPlaceSearch, &QLineEdit::textEdited, this, &HcStoreWidget::QueryPluginMarket );
    connect( MarketPlaceList, &QListWidget::itemClicked, this, [&]( QListWidgetItem* item ){
        PluginViewStack->setCurrentIndex( item->listWidget()->row( item ) );
    });

    PluginWorker.Thread->start();

    QMetaObject::connectSlotsByName( this );
}

auto HcStoreWidget::AddPlugin(
    const std::string& parent,
    const std::string& repo,
    PluginView*        plugin
) -> void {
    auto plugin_name = std::string();
    auto project_url = repo;
    auto categories  = std::vector<std::string>();
    auto description = std::string();
    auto version     = std::string();
    auto license     = std::string();
    auto readme      = std::string();
    auto author_name = std::string();
    auto author_url  = std::string();

    //
    // parse the json object containing the plugin information
    // such as name, description, readme, categories, etc.
    //

    if ( plugin->object.contains( "name" ) && plugin->object[ "name" ].is_string() ) {
        plugin_name = plugin->object[ "name" ].get<std::string>();
    } else {
        spdlog::error( "AddPlugin error: failed to retrieve plugin name (either not found or not a string)" );
        return;
    }

    if ( plugin->object.contains( "description" ) && plugin->object[ "description" ].is_string() ) {
        description = plugin->object[ "description" ].get<std::string>();
    } else {
        spdlog::error( "AddPlugin error: failed to retrieve plugin description (either not found or not a string)" );
        return;
    }

    if ( plugin->object.contains( "readme" ) && plugin->object[ "readme" ].is_string() ) {
        readme = plugin->object[ "readme" ].get<std::string>();
    } else {
        spdlog::error( "AddPlugin error: failed to retrieve plugin readme (either not found or not a string)" );
        return;
    }

    if ( plugin->object.contains( "author" ) && plugin->object[ "author" ].is_object() ) {
        if ( plugin->object[ "author" ].get<json>().contains( "name" ) &&
             plugin->object[ "author" ].get<json>()[ "name" ].is_string()
        ) {
            author_name = plugin->object[ "author" ].get<json>()[ "name" ].get<std::string>();
        } else {
            spdlog::error( "AddPlugin error: failed to retrieve plugin author name (either not found or not a string)" );
            return;
        }

        if ( plugin->object[ "author" ].get<json>().contains( "url" ) &&
             plugin->object[ "author" ].get<json>()[ "url" ].is_string()
        ) {
            author_url = plugin->object[ "author" ].get<json>()[ "url" ].get<std::string>();
        } else {
            spdlog::error( "AddPlugin error: failed to retrieve plugin author url (either not found or not a string)" );
            return;
        }
    } else {
        spdlog::error( "AddPlugin error: failed to retrieve plugin author (either not found or not an object)" );
        return;
    }

    if ( plugin->object.contains( "categories" ) && plugin->object[ "categories" ].is_array() ) {
        for ( auto & category : plugin->object[ "categories" ].get<std::vector<json>>() ) {
            if ( ! category.is_string() ) {
                spdlog::warn( "AddPlugin warning: plugin category is not a string" );
                continue;
            }

            categories.push_back( category.get<std::string>() );
        }
    }

    //
    // create the Qt objects
    //

    plugin->Widget = new QWidget( PluginViewStack );
    plugin->GridLayout = new QGridLayout( plugin->Widget );
    plugin->GridLayout->setObjectName( "PluginGridLayout" );

    plugin->LayoutCategories = new QHBoxLayout();
    plugin->LayoutCategories->setObjectName( "PluginLayoutCategories" );

    for ( auto& category : categories ) {
        auto tag = new QLabel( category.c_str() );

        tag->setProperty( "HcLabelDisplay", "tag" );
        tag->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Minimum );

        plugin->LayoutCategories->addWidget( tag );
    }

    plugin->ButtonInstall = new QPushButton( plugin->Widget );
    plugin->ButtonInstall->setObjectName( "PluginButtonInstall" );
    plugin->ButtonInstall->setText( "Install" );
    plugin->ButtonInstall->setFixedWidth( 120 );

    plugin->LabelName = new QLabel( plugin->Widget );
    plugin->LabelName->setObjectName( "PluginLabelName" );
    plugin->LabelName->setOpenExternalLinks( true );

    plugin->TextReadme = new HcTextEdit( plugin->Widget );
    plugin->TextReadme->setObjectName( "PluginTextReadme" );
    plugin->TextReadme->setProperty( "HcConsole", "true" );
    plugin->TextReadme->setReadOnly( true );

    plugin->LabelDescription = new QLabel( plugin->Widget );
    plugin->LabelDescription->setObjectName( "PluginLabelDescription" );

    plugin->Spacers[ 0 ] = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    plugin->Spacers[ 1 ] = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    plugin->Spacers[ 2 ] = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    plugin->LabelAuthor = new HcLabelNamed( "Author:", std::format( "<a href=\"{}\" style=\"color: #8BE9FD; text-decoration:none\"><span>{}</span></a>", author_url, author_name ).c_str(), plugin->Widget );
    plugin->LabelAuthor->Text->setObjectName( "PluginLabelAuthor" );
    plugin->LabelAuthor->Text->setProperty( "HcLabelLink", true );
    plugin->LabelAuthor->Text->setOpenExternalLinks( true );
    plugin->LabelAuthor->Text->setFixedHeight( 25 );
    plugin->LabelAuthor->Label->setFixedHeight( 25 );
    plugin->LabelAuthor->Label->setFixedWidth( 70 );

    plugin->GridLayout->addWidget( plugin->LabelName, 0, 0, 1, 1 );
    plugin->GridLayout->addItem( plugin->Spacers[ 0 ], 0, 1, 1, 5 );
    plugin->GridLayout->addWidget( plugin->LabelAuthor, 1, 0, 1, 1 );
    plugin->GridLayout->addLayout( plugin->LayoutCategories, 1, 1, 1, 1 );
    plugin->GridLayout->addItem( plugin->Spacers[ 1 ], 1, 2, 1, 4 );
    plugin->GridLayout->addWidget( plugin->LabelDescription, 2, 0, 1, 6 );
    plugin->GridLayout->addWidget( plugin->ButtonInstall, 3, 0, 1, 1 );
    plugin->GridLayout->addItem( plugin->Spacers[ 2 ], 3, 1, 1, 5 );
    plugin->GridLayout->addWidget( plugin->TextReadme, 4, 0, 1, 6 );

    //
    // display the retrieved plugin information
    //

    plugin->LabelName->setText( std::format( "<a href=\"{}\" style=\"color: #8BE9FD; text-decoration:none; font-size:18pt;\"><span>{}</span></a>", project_url, plugin_name ).c_str() );
    plugin->LabelDescription->setText( description.c_str() );
    plugin->TextReadme->setMarkdown( readme.c_str() );

    //
    // add plugin object to the vector array and to the stack widget
    //

    Plugins.push_back( plugin );
    PluginViewStack->addWidget( plugin->Widget );

    //
    // insert the plugin into the market list
    //

    AddPluginToMarketList( plugin );

    //
    // each time a plugin gets added revert
    // back to the first plugin to be viewed
    //

    MarketPlaceList->setCurrentRow( 0 );
    PluginViewStack->setCurrentIndex( 0 );
}

auto HcStoreWidget::AddPluginToMarketList(
    PluginView *plugin
) -> void {
    plugin->ListItem   = new QListWidgetItem;
    plugin->ListWidget = new HcMarketPluginItem( plugin->object[ "name" ].get<std::string>().c_str(), plugin->object[ "description" ].get<std::string>().c_str(), this );
    plugin->ListItem->setSizeHint( plugin->ListWidget->sizeHint() );

    MarketPlaceList->addItem( plugin->ListItem );
    MarketPlaceList->setItemWidget( plugin->ListItem, plugin->ListWidget );
}

auto HcStoreWidget::QueryPluginMarket(
    const QString& query
) -> void {
    //
    // TODO: do tokenized search and string search
    //       tokenized (max results / less accurate):
    //          "unguard-eat by 5pider" -> "unguard-eat" "by" "5pider"
    //          and each token will be compared against the entire pluging metadata
    //       complete string search (less results / max accurate):
    //          the string "unguard-eat by 5pider" will be searched across plugins
    //          if they contain the exact string or not
    //

    MarketPlaceList->clear();

    if ( query.isEmpty() ) {
        for ( auto& plugin : Plugins ) {
            AddPluginToMarketList( plugin );
        }
        return;
    }

    for ( auto& plugin : Plugins ) {
        if ( PluginQueryContainMeta( plugin, query ) ) {
            AddPluginToMarketList( plugin );
        }
    }
}

auto HcStoreWidget::PluginQueryContainMeta(
    const PluginView* plugin,
    const QString&    query
) -> bool {
    auto name        = plugin->object[ "name" ].get<std::string>();
    auto description = plugin->object[ "description" ].get<std::string>();
    auto author      = plugin->object[ "author" ][ "name" ].get<std::string>();
    auto search      = query.toStdString();

    if ( search.find( name ) != std::string::npos ) {
        return true;
    }

    if ( search.find( description ) != std::string::npos ) {
        return true;
    }

    if ( search.find( author ) != std::string::npos ) {
        return true;
    }

    if ( plugin->object.contains( "categories" ) && plugin->object[ "categories" ].is_array() ) {
        for ( auto & category : plugin->object[ "categories" ].get<std::vector<json>>() ) {
            if ( ! category.is_string() ) {
                continue;
            }

            if ( search.find( category ) != std::string::npos ) {
                return true;
            }
        }
    }

    return false;
}

auto HcStoreWidget::HttpGet(
    const std::string& url
) -> std::optional<std::string> {
    auto _url   = QUrl( url.c_str() );
    auto client = httplib::Client( _url.scheme().toStdString() + "://" + _url.host().toStdString() );
    auto result = httplib::Result();

    result = client.Get( _url.path().toStdString() );

    if ( HttpErrorToString( result.error() ).has_value() ) {
        spdlog::error( "AddPlugin error: failed to send plugin add request: {}", HttpErrorToString( result.error() ).value() );
        return std::nullopt;
    }

    return ( result->body );
}


