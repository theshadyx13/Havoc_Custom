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

        setInstalled();

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

    connect( MarketPlaceSearch, &QLineEdit::textEdited, this, &HcStoreWidget::QueryPluginMarket );

    QMetaObject::connectSlotsByName( this );
}

auto HcStoreWidget::AddPlugin(
    const QString& repo
) -> void {
    auto client = httplib::Client( "https://raw.githubusercontent.com" );
    auto result = httplib::Result();
    auto object = json();

    //
    // send the request to our endpoint
    //
    result = client.Get( repo.toStdString() + "/main/plugin.json" );

    if ( HttpErrorToString( result.error() ).has_value() ) {
        spdlog::error( "AddPlugin error: failed to send plugin add request: {}", HttpErrorToString( result.error() ).value() );
        return;
    }

    try {
        if ( ( object = json::parse( result->body ) ).is_discarded() ) {
            spdlog::error( "AddPlugin failed to parse plugin info of repo {}", repo.toStdString() );
            return;
        }

        AddPluginObject( repo.toStdString(), object );
    } catch ( std::exception& e ) {
        spdlog::error( "AddPluginObject exception: {}", e.what() );
    }
}

auto HcStoreWidget::AddPluginObject(
    const std::string& repo,
    const json&        object
) -> void {
    auto plugin_name = std::string();
    auto plugin_id   = std::string();
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

    if ( object.contains( "name" ) && object[ "name" ].is_string() ) {
        plugin_name = object[ "name" ].get<std::string>();
    } else {
        spdlog::error( "ViewPlugin error: failed to retrieve plugin name (either not found or not a string)" );
        return;
    }

    if ( object.contains( "plugin_id" ) && object[ "plugin_id" ].is_string() ) {
        plugin_id = object[ "plugin_id" ].get<std::string>();
    } else {
        spdlog::error( "ViewPlugin error: failed to retrieve plugin id (either not found or not a string)" );
        return;
    }

    if ( object.contains( "description" ) && object[ "description" ].is_string() ) {
        description = object[ "description" ].get<std::string>();
    } else {
        spdlog::error( "ViewPlugin error: failed to retrieve plugin description (either not found or not a string)" );
        return;
    }

    if ( object.contains( "readme" ) || object.contains( "readme_path" ) ) {
        if ( ! object.contains( "readme_path" ) && object.contains( "readme" ) && object[ "readme" ].is_string() ) {
            readme = object[ "readme" ].get<std::string>();
        } else if ( object.contains( "readme_path" ) && object[ "readme_path" ].is_string() ) {
            auto client = httplib::Client( "https://raw.githubusercontent.com" );
            auto result = httplib::Result();

            result = client.Get( repo + "/main/" + object[ "readme_path" ].get<std::string>() );

            if ( HttpErrorToString( result.error() ).has_value() ) {
                spdlog::error( "ViewPlugin error: failed to send plugin readme request: {}", HttpErrorToString( result.error() ).value() );
                return;
            }

            readme = result->body;
        } else {
            spdlog::error( "ViewPlugin error: failed to retrieve plugin readme (either not found or not a string)" );
            return;
        }
    } else {
        spdlog::error( "ViewPlugin error: failed to retrieve plugin readme name (not found)" );
        return;
    }

    if ( object.contains( "author" ) && object[ "author" ].is_object() ) {
        if ( object[ "author" ].get<json>().contains( "name" ) &&
             object[ "author" ].get<json>()[ "name" ].is_string()
        ) {
            author_name = object[ "author" ].get<json>()[ "name" ].get<std::string>();
        } else {
            spdlog::error( "ViewPlugin error: failed to retrieve plugin author name (either not found or not a string)" );
            return;
        }

        if ( object[ "author" ].get<json>().contains( "url" ) &&
             object[ "author" ].get<json>()[ "url" ].is_string()
        ) {
            author_url = object[ "author" ].get<json>()[ "url" ].get<std::string>();
        } else {
            spdlog::error( "ViewPlugin error: failed to retrieve plugin author url (either not found or not a string)" );
            return;
        }
    } else {
        spdlog::error( "ViewPlugin error: failed to retrieve plugin author (either not found or not an object)" );
        return;
    }

    if ( object.contains( "categories" ) && object[ "categories" ].is_array() ) {
        for ( auto & category : object[ "categories" ].get<std::vector<json>>() ) {
            if ( ! category.is_string() ) {
                spdlog::warn( "ViewPlugin warning: plugin category is not a string" );
                continue;
            }

            categories.push_back( category.get<std::string>() );
        }
    }

    //
    // create the Qt objects
    //

    auto plugin = new PluginView;

    plugin->repo      = repo;
    plugin->object    = object;
    plugin->plugin_id = plugin_id;
    plugin->installed = false;

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

    plugin->LabelName->setText( std::format( "<p><span style=\"font-size:18pt;\">{}</span></p>", plugin_name ).c_str() );
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