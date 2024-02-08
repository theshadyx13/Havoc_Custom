#include <Havoc.h>
#include <ui/HcPageAgent.h>

HcPageAgent::HcPageAgent(QWidget* parent ) : QWidget(parent ) {

    if ( objectName().isEmpty() ) {
        setObjectName( QString::fromUtf8( "PageAgent" ) );
    }

    gridLayout = new QGridLayout( this );
    gridLayout->setObjectName( "gridLayout" );

    ComboAgentView = new QComboBox( this );
    ComboAgentView->setObjectName( "ComboAgentView" );
    ComboAgentView->view()->setProperty( "ComboBox", "true" );

    Splitter = new QSplitter( this );
    Splitter->setObjectName( QString::fromUtf8( "Splitter" ) );
    Splitter->setOrientation( Qt::Vertical );

    AgentTable = new QTableWidget( Splitter );
    AgentTable->setObjectName( "AgentTable" );

    TitleAgentID      = new QTableWidgetItem( "UUID" );
    TitleInternal     = new QTableWidgetItem( "Internal" );
    TitleUsername     = new QTableWidgetItem( "User" );
    TitleHostname     = new QTableWidgetItem( "Host" );
    TitleSystem       = new QTableWidgetItem( "System" );
    TitleProcessID    = new QTableWidgetItem( "Pid" );
    TitleProcessName  = new QTableWidgetItem( "Process" );
    TitleProcessArch  = new QTableWidgetItem( "Arch" );
    TitleThreadID     = new QTableWidgetItem( "Tid" );
    TitleNote         = new QTableWidgetItem( "Note" );
    TitleLastCallback = new QTableWidgetItem( "Last" );

    if ( AgentTable->columnCount() < 11 ) {
        AgentTable->setColumnCount( 11 );
    }

    /* TODO: get how we should add this from the settings
     * for now we just do a default one */
    AgentTable->setHorizontalHeaderItem( 0,  TitleAgentID      );
    AgentTable->setHorizontalHeaderItem( 1,  TitleInternal     );
    AgentTable->setHorizontalHeaderItem( 2,  TitleUsername     );
    AgentTable->setHorizontalHeaderItem( 3,  TitleHostname     );
    AgentTable->setHorizontalHeaderItem( 4,  TitleProcessName  );
    AgentTable->setHorizontalHeaderItem( 5,  TitleProcessID    );
    AgentTable->setHorizontalHeaderItem( 6,  TitleThreadID     );
    AgentTable->setHorizontalHeaderItem( 7,  TitleProcessArch  );
    AgentTable->setHorizontalHeaderItem( 8,  TitleSystem       );
    AgentTable->setHorizontalHeaderItem( 9,  TitleLastCallback );
    AgentTable->setHorizontalHeaderItem( 10, TitleNote         );

    /* table settings */
    AgentTable->setEnabled( true );
    AgentTable->setShowGrid( false );
    AgentTable->setSortingEnabled( false );
    AgentTable->setWordWrap( true );
    AgentTable->setCornerButtonEnabled( true );
    AgentTable->horizontalHeader()->setVisible( true );
    AgentTable->setSelectionBehavior( QAbstractItemView::SelectRows );
    AgentTable->setContextMenuPolicy( Qt::CustomContextMenu );
    AgentTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeMode::Stretch );
    AgentTable->horizontalHeader()->setStretchLastSection( true );
    AgentTable->verticalHeader()->setVisible( false );
    AgentTable->setFocusPolicy( Qt::NoFocus );

    AgentTab = new QTabWidget( Splitter );
    AgentTab->setObjectName( "AgentTab" );
    AgentTab->setTabsClosable( true );
    AgentTab->setMovable( true );

    AgentDisplayerSessions = new QLabel( this );
    AgentDisplayerSessions->setObjectName( "LabelDisplaySessions" );
    AgentDisplayerSessions->setProperty( "HcLabelDisplay", "true" );

    AgentDisplayerTargets = new QLabel( this );
    AgentDisplayerTargets->setObjectName( "LabelDisplayTargets" );
    AgentDisplayerTargets->setProperty( "HcLabelDisplay", "true" );

    AgentDisplayerPivots = new QLabel( this );
    AgentDisplayerPivots->setObjectName( "LabelDisplayPivots" );
    AgentDisplayerPivots->setProperty( "HcLabelDisplay", "true" );

    AgentDisplayerElevated = new QLabel( this );
    AgentDisplayerElevated->setObjectName( "LabelDisplayElevated" );
    AgentDisplayerElevated->setProperty( "HcLabelDisplay", "true" );

    horizontalSpacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

    Splitter->addWidget( AgentTable );
    Splitter->addWidget( AgentTab );
    Splitter->handle( 1 )->setEnabled( SplitterMoveToggle ); /* disabled by default */

    gridLayout->addWidget( ComboAgentView,         0, 0, 1, 1 );
    gridLayout->addWidget( Splitter,               1, 0, 1, 6 );
    gridLayout->addWidget( AgentDisplayerSessions, 0, 2, 1, 1 );
    gridLayout->addWidget( AgentDisplayerTargets,  0, 1, 1, 1 );
    gridLayout->addWidget( AgentDisplayerPivots,   0, 3, 1, 1 );
    gridLayout->addWidget( AgentDisplayerElevated, 0, 4, 1, 1 );
    gridLayout->addItem( horizontalSpacer,         0, 5, 1, 1 );

    retranslateUi( );

    QMetaObject::connectSlotsByName( this );
}

HcPageAgent::~HcPageAgent() = default;

auto HcPageAgent::retranslateUi() -> void {
    setStyleSheet( Havoc->getStyleSheet() );

    AgentDisplayerElevated->setText( "Elevated: 0" );
    AgentDisplayerSessions->setText( "Sessions: 0" );
    AgentDisplayerTargets->setText( "Targets: 0" );
    AgentDisplayerPivots->setText( "Pivots: 0" );
    ComboAgentView->addItems( QStringList() << "Sessions" << "Sessions Graph" << "Targets" );
}

auto HcPageAgent::addTab(
    const QString& name,
    QWidget*       widget
) -> void {

}

inline auto HcTableWidget(
    const QString&     value,
    const Qt::ItemFlag flags = Qt::ItemIsEditable
) -> QTableWidgetItem* {
    auto item = new QTableWidgetItem( value );

    item->setTextAlignment( Qt::AlignCenter );
    item->setFlags( item->flags() ^ flags );

    return item;
}

auto HcPageAgent::addAgent(
    const json& metadata
) -> void {
    auto uuid    = QString();
    auto arch    = QString();
    auto user    = QString();
    auto host    = QString();
    auto local   = QString();
    auto path    = QString();
    auto process = QString();
    auto pid     = QString();
    auto tid     = QString();
    auto system  = QString();
    auto last    = QString();
    auto row     = AgentTable->rowCount();
    auto sort    = AgentTable->isSortingEnabled();

    uuid    = QString( metadata[ "uuid" ].get<std::string>().c_str() );
    user    = QString( metadata[ "meta" ][ "user" ].get<std::string>().c_str() );
    arch    = QString( metadata[ "meta" ][ "arch" ].get<std::string>().c_str() );
    host    = QString( metadata[ "meta" ][ "host" ].get<std::string>().c_str() );
    local   = QString( metadata[ "meta" ][ "local ip" ].get<std::string>().c_str() );
    path    = QString( metadata[ "meta" ][ "process path" ].get<std::string>().c_str() );
    process = QString( metadata[ "meta" ][ "process name"].get<std::string>().c_str() );
    pid     = QString::number( metadata[ "meta" ][ "pid" ].get<int>() );
    tid     = QString::number( metadata[ "meta" ][ "tid" ].get<int>() );
    system  = QString( metadata[ "meta" ][ "system" ].get<std::string>().c_str() );
    last    = QString( metadata[ "meta" ][ "last callback" ].get<std::string>().c_str() );

    auto agent = new HcAgent{
        .data = metadata,
        .ui = {
            .Uuid        = HcTableWidget( uuid ),
            .Internal    = HcTableWidget( local ),
            .Username    = HcTableWidget( user ),
            .Hostname    = HcTableWidget( host ),
            .ProcessPath = HcTableWidget( path ),
            .ProcessName = HcTableWidget( process ),
            .ProcessId   = HcTableWidget( pid ),
            .ThreadId    = HcTableWidget( tid ),
            .Arch        = HcTableWidget( arch ),
            .System      = HcTableWidget( system ),
            .Note        = HcTableWidget( "", Qt::NoItemFlags ),
            .Last        = HcTableWidget( last ),
        }
    };

    agents.push_back( agent );

    AgentTable->setRowCount( row + 1 );
    AgentTable->setSortingEnabled( false );
    AgentTable->setItem( row, 0,  agent->ui.Uuid        );
    AgentTable->setItem( row, 1,  agent->ui.Internal    );
    AgentTable->setItem( row, 2,  agent->ui.Username    );
    AgentTable->setItem( row, 3,  agent->ui.Hostname    );
    AgentTable->setItem( row, 4,  agent->ui.ProcessName );
    AgentTable->setItem( row, 5,  agent->ui.ProcessId   );
    AgentTable->setItem( row, 6,  agent->ui.ThreadId    );
    AgentTable->setItem( row, 7,  agent->ui.Arch        );
    AgentTable->setItem( row, 8,  agent->ui.System      );
    AgentTable->setItem( row, 9,  agent->ui.Last        );
    AgentTable->setItem( row, 10, agent->ui.Note        );
    AgentTable->setSortingEnabled( sort );

    AgentDisplayerTargets->setText( QString( "Targets: %1" ).arg( agents.size() ) ); /* TODO: all targets (only show one host) */
    AgentDisplayerSessions->setText( QString( "Sessions: %1" ).arg( agents.size() ) ); /* TODO: only set current alive beacons/sessions */
    AgentDisplayerPivots->setText( "Pivots: 0" );
    AgentDisplayerElevated->setText( "Elevated: 0" );
}
