#include <Havoc.h>
#include <ui/HcPageScript.h>

HcPagePlugins::HcPagePlugins()
{
    if ( objectName().isEmpty() ) {
        setObjectName( "PagePlugins" );
    }

    gridLayout = new QGridLayout( this );
    gridLayout->setObjectName( "gridLayout" );

    TabWidget = new QTabWidget( this );
    TabWidget->setObjectName( "TabWidget" );
    TabWidget->setProperty( "HcPageTab", "true" );
    TabWidget->tabBar()->setProperty( "HcPageTab", "true" );
    TabWidget->setFocusPolicy( Qt::NoFocus );

    TabPluginManager = new QWidget();
    TabPluginManager->setObjectName( "TabPluginManager" );

    gridLayout_2 = new QGridLayout( TabPluginManager );
    gridLayout_2->setObjectName( "gridLayout_2" );

    ButtonLoad = new QPushButton( TabPluginManager );
    ButtonLoad->setObjectName( "ButtonLoad" );
    ButtonLoad->setProperty( "HcButton", "true" );

    horizontalSpacer = new QSpacerItem( 972, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    LabelLoadedPlugins = new QLabel( TabPluginManager );
    LabelLoadedPlugins->setObjectName( "LabelLoadedPlugins" );
    LabelLoadedPlugins->setProperty( "HcLabelDisplay", "true" );

    splitter = new QSplitter( TabPluginManager );
    splitter->setObjectName( "splitter" );
    splitter->setOrientation( Qt::Vertical );

    TablePluginsWidget = new QTableWidget( splitter );
    TablePluginsWidget->setObjectName( "TablePluginsWidget" );
    TablePluginsWidget->setProperty( "HcPageTab", "true" );

    if ( TablePluginsWidget->columnCount() < 1 ) {
        TablePluginsWidget->setColumnCount( 1 );
    }

    TablePluginsWidget->setHorizontalHeaderItem( 0, new QTableWidgetItem( "Path" ) );

    /* table settings */
    TablePluginsWidget->setEnabled( true );
    TablePluginsWidget->setShowGrid( false );
    TablePluginsWidget->setSortingEnabled( false );
    TablePluginsWidget->setWordWrap( true );
    TablePluginsWidget->setCornerButtonEnabled( true );
    TablePluginsWidget->horizontalHeader()->setVisible( true );
    TablePluginsWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    TablePluginsWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    TablePluginsWidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
    TablePluginsWidget->horizontalHeader()->setStretchLastSection( true );
    TablePluginsWidget->verticalHeader()->setVisible( false );
    TablePluginsWidget->setFocusPolicy( Qt::NoFocus );

    PyConsole = new HcPyConsole( splitter );

    splitter->addWidget( TablePluginsWidget );
    splitter->addWidget( PyConsole );

    TabPluginStore = new HcStoreWidget();
    TabPluginStore->setObjectName( "TabPluginStore" );
    
    gridLayout_2->addWidget( ButtonLoad, 0, 0, 1, 1 );
    gridLayout_2->addItem( horizontalSpacer, 0, 1, 1, 1 );
    gridLayout_2->addWidget( LabelLoadedPlugins, 0, 2, 1, 1 );
    gridLayout_2->addWidget( splitter, 1, 0, 1, 3 );
    gridLayout_2->setContentsMargins( 0, 0, 0, 0 );

    gridLayout->addWidget( TabWidget, 0, 0, 1, 1 );

    TabWidget->addTab( TabPluginManager, "Manager" );
    TabWidget->addTab( TabPluginStore, "Store" );

    retranslateUi();

    //
    // signals
    //
    QObject::connect( this, & HcPagePlugins::SignalConsoleWrite, PyConsole, & HcPyConsole::append );

    QObject::connect( ButtonLoad, &QPushButton::clicked, this, [&] () {
        auto dialog    = QFileDialog();
        auto plugin    = QUrl();
        auto exception = std::string();

        dialog.setStyleSheet( Havoc->getStyleSheet() );
        dialog.setDirectory( QDir::homePath() );

        if ( LoadCallback.has_value() ) {
            if ( dialog.exec() == QFileDialog::Accepted ) {
                plugin = dialog.selectedUrls().value( 0 ).toLocalFile();
                if ( ! plugin.toString().isNull() ) {
                    try {
                        py11::gil_scoped_acquire gil;
                        LoadCallback.value()( py11::str( plugin.toString().toStdString() ) );
                    } catch ( py11::error_already_set &eas ) {
                        exception = eas.what();
                    }

                    if ( ! exception.empty() ) {
                        //
                        // print it to the python console
                        //
                        PyConsole->append( exception.c_str() );

                        //
                        // print it to the terminal console
                        // as well
                        //
                        spdlog::error( "python callstack: \n{}", exception );
                    }
                }
            }
        } else {
            Helper::MessageBox(
                QMessageBox::Warning,
                "Script Manager",
                "No load script handler has been registered"
            );
        }
    } );

    TabWidget->setCurrentIndex( 0 );

    QMetaObject::connectSlotsByName( this );
}

auto HcPagePlugins::retranslateUi() -> void {
    setWindowTitle( "PagePlugins" );
    setStyleSheet( Havoc->getStyleSheet() );
    ButtonLoad->setText( "Load Plugin" );
    LabelLoadedPlugins->setText( "Loaded: 0" );
}

auto HcPagePlugins::LoadScript(
    const std::string& path
) -> void {

    if ( LoadCallback.has_value() ) {
        try {
            LoadCallback.value()( path );
        } catch ( py11::error_already_set &eas ) {
            spdlog::error( "failed to load script {}: \n{}", path, eas.what() );
        }
    } else {
        spdlog::debug( "PageScripts->LoadCallback not set" );
    }
}

HcPyConsole::HcPyConsole(
    QWidget* parent
) : QTextEdit( parent ) {
    setObjectName( "HcPyConsole" );
    setReadOnly( true );
    setProperty( "HcConsole", "true" );
}