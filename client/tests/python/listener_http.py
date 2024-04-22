import pyhavoc

from PySide6.QtCore import *  
from PySide6.QtGui import *  
from PySide6.QtWidgets import *

class HcWidgetFile( QWidget ):

    PathMaxLen: int = 40

    def __init__( self, *args, **kwargs ):
        super().__init__( *args, **kwargs )

        buttonSizeMax = QSize( 70, 16777215 )
        buttonSizeMin = QSize( 70, 0 )

        self.file_path: str = ""

        self.setContentsMargins( 0, 0, 0, 0 )
        self.setObjectName( u"HcWidgetFile" )

        self.gridLayout = QGridLayout( self )
        self.gridLayout.setObjectName( u"gridLayout" )
        self.gridLayout.setContentsMargins( 0, 0, 0, 0 )

        self.Label = QLabel( self )
        self.Label.setObjectName( u"Label" )
        self.Label.setProperty( "HcLabelDisplay", "true" )
        self.Label.setText( u"(empty)" )

        self.ButtonLoad = QPushButton( self )
        self.ButtonLoad.setObjectName( u"ButtonLoad" )
        self.ButtonLoad.setText( u"Load" )
        self.ButtonLoad.setMaximumSize( buttonSizeMax )
        self.ButtonLoad.setMinimumSize( buttonSizeMin )
        self.ButtonLoad.setProperty( "HcButton", "true" )
        self.ButtonLoad.clicked.connect( self.button_load )

        self.ButtonRemove = QPushButton( self )
        self.ButtonRemove.setObjectName( u"ButtonRemove" )
        self.ButtonRemove.setText( u"Remove" )
        self.ButtonRemove.setMaximumSize( buttonSizeMax )
        self.ButtonRemove.setMinimumSize( buttonSizeMin )
        self.ButtonRemove.setProperty( "HcButton", "true" )
        self.ButtonRemove.clicked.connect( self.button_remove )

        self.gridLayout.addWidget( self.Label,        0, 0, 1, 1 )
        self.gridLayout.addWidget( self.ButtonLoad,   0, 1, 1, 1 )
        self.gridLayout.addWidget( self.ButtonRemove, 0, 2, 1, 1 )

        QMetaObject.connectSlotsByName( self )

        return

    def button_load( self ) -> None:
        file_dialog = QFileDialog()
        file_dialog.setStyleSheet( pyhavoc.ui.HcUiGetStyleSheet() )

        if file_dialog.exec() == QDialog.Accepted:
            self.file_path = file_dialog.selectedFiles()[ 0 ]

            path = ""

            if len( self.file_path ) > self.PathMaxLen:
                path = "..."
                path = path + self.file_path[ len( self.file_path ) - self.PathMaxLen : len( self.file_path ) ]
            else:
                path = self.file_path

            self.Label.setText( path )

        file_dialog.deleteLater()

        return

    def button_remove( self ) -> None:

        self.file_path = ""
        self.Label.setText( "(empty)" )

        return

    def file( self ) -> str:

        if len( self.file_path ) != 0:
            file    = open( self.file_path, 'r' )
            content = file.read()

            file.close()

            return content

        return ''


class HcWidgetList( QWidget ):

    def __init__( self, *args, **kwargs ):
        super().__init__( *args, **kwargs )

        button_size = QSize( 100, 25 )

        self.setObjectName( u"HcWidgetList" )

        self.GridLayout = QGridLayout( self )
        self.GridLayout.setObjectName( u"gridLayout" )
        self.GridLayout.setContentsMargins( 0, 0, 0, 0 )

        self.GroupList = QGroupBox( self )
        self.GroupList.setObjectName( u"GroupList" )
        self.GroupList.setTitle( u"" )
        self.FormLayout = QFormLayout( self.GroupList )

        self.ButtonAdd = QPushButton( self )
        self.ButtonAdd.setObjectName( u"ButtonAdd" )
        self.ButtonAdd.setText( u"Add" )
        self.ButtonAdd.setProperty( "HcButton", "true" )
        self.ButtonAdd.setMaximumSize( button_size )
        self.ButtonAdd.setMinimumSize( button_size )
        self.ButtonAdd.setText( u"Add" )
        self.ButtonAdd.clicked.connect( self.button_add )

        self.ButtonClear = QPushButton( self )
        self.ButtonClear.setObjectName( u"ButtonClear" )
        self.ButtonClear.setProperty( "HcButton", "true" )
        self.ButtonClear.setMaximumSize( button_size )
        self.ButtonClear.setMinimumSize( button_size )
        self.ButtonClear.setText( u"Clear" )
        self.ButtonClear.clicked.connect( self.button_clear )

        self.GridLayout.addWidget( self.GroupList,   0, 0, 3, 1 )
        self.GridLayout.addWidget( self.ButtonAdd,   0, 1, 1, 1 )
        self.GridLayout.addWidget( self.ButtonClear, 1, 1, 1, 1 )

        self.InputList : list[QLineEdit] = []

        QMetaObject.connectSlotsByName( self )

        return

    def list(self) -> list[str]:
        array: list[str] = []

        for i in self.InputList:
            array.append( i.text() )

        return array

    def add_input( self, value: str = "" ) -> None:
        line_edit = QLineEdit()
        line_edit.setFocus()
        line_edit.setText( value )

        self.FormLayout.setWidget( len( self.InputList ), QFormLayout.FieldRole, line_edit )

        self.InputList.append( line_edit )

        return

    def button_add( self ) -> None:
        self.add_input()
        return

    def button_clear( self ) -> None:

        for i in self.InputList:
            i.deleteLater()

        self.InputList = []
        return

@pyhavoc.ui.HcUiListenerRegisterView( "HTTP" )
class HcListenerHttp( pyhavoc.ui.HcListenerView ):

    def __init__( self, *args, **kwargs ):
        super().__init__( *args, **kwargs )

        ##
        ## base widgets
        ##
        self.layout     : QGridLayout = None
        self.tab_widget : QTabWidget  = None

        ##
        ## page widgets
        ##
        self.page_option  : QWidget = None
        self.page_proxy   : QWidget = None
        self.page_server  : QWidget = None
        self.page_client  : QWidget = None
        self.page_preview : QWidget = None
        self.page_help    : QWidget = None

        ##
        ## page options widgets
        ##
        self.opt_layout            : QGridLayout  = None
        self.opt_label_hostbind    : QLabel       = None
        self.opt_combo_hostbind    : QComboBox    = None
        self.opt_label_port        : QLabel       = None
        self.opt_input_port        : QLineEdit    = None
        self.opt_label_hosts       : QLabel       = None
        self.opt_list_hosts        : HcWidgetList = None
        self.opt_space_hosts       : QSpacerItem  = None
        self.opt_label_rotation    : QLabel       = None
        self.opt_combo_rotation    : QComboBox    = None
        self.opt_label_useragent   : QLabel       = None
        self.opt_input_useragent   : QLineEdit    = None
        self.opt_label_uris        : QLabel       = None
        self.opt_list_uris         : HcWidgetList = None
        self.opt_space_uris        : QSpacerItem  = None
        self.opt_label_hostheader  : QLabel       = None
        self.opt_input_hostheader  : QLineEdit    = None
        self.opt_label_secure      : QLabel       = None
        self.opt_combo_secure      : QComboBox    = None
        self.opt_label_server_cert : QLabel       = None
        self.opt_file_server_cert  : HcWidgetFile = None
        self.opt_label_server_key  : QLabel       = None
        self.opt_file_server_key   : HcWidgetFile = None

        ##
        ## page proxy widgets
        ##
        self.pxy_layout       : QGridLayout = None
        self.pxy_label_enable : QLabel      = None
        self.pxy_combo_enable : QComboBox   = None
        self.pxy_label_type   : QLabel      = None
        self.pxy_combo_type   : QComboBox   = None
        self.pxy_label_host   : QLabel      = None
        self.pxy_input_host   : QLineEdit   = None
        self.pxy_label_port   : QLabel      = None
        self.pxy_input_port   : QLineEdit   = None
        self.pxy_label_user   : QLabel      = None
        self.pxy_input_user   : QLineEdit   = None
        self.pxy_label_pass   : QLabel      = None
        self.pxy_input_pass   : QLineEdit   = None
        self.pxy_spacer       : QSpacerItem = None

        ##
        ## page server widgets
        ##
        self.srv_layout        : QGridLayout  = None
        self.srv_label_headers : QLabel       = None
        self.srv_list_headers  : HcWidgetList = None
        self.srv_spacer        : QSpacerItem  = None
        self.srv_label_prepend : QLabel       = None
        self.srv_text_prepend  : QTextEdit    = None
        self.srv_label_append  : QLabel       = None
        self.srv_text_append   : QTextEdit    = None

        ##
        ## page server widgets
        ##
        self.cli_layout        : QGridLayout  = None
        self.cli_label_headers : QLabel       = None
        self.cli_list_headers  : HcWidgetList = None
        self.cli_spacer        : QSpacerItem  = None
        self.cli_label_prepend : QLabel       = None
        self.cli_text_prepend  : QTextEdit    = None
        self.cli_label_append  : QLabel       = None
        self.cli_text_append   : QTextEdit    = None

        ##
        ## page preview widgets
        ##


    ##
    ## main function to create and
    ## render the widgets for the listener
    ##
    def main( self ):
        ##
        ## create base widget
        ##
        self.layout = QGridLayout( self.listener_widget() )
        self.layout.setContentsMargins( 0, 0, 0, 0 )

        self.tab_widget = QTabWidget()
        self.tab_widget.tabBar().setProperty( "HcProtocolTab", "true" )
        self.tab_widget.tabBar().setExpanding( True )
        self.tab_widget.tabBar().setDocumentMode( True )

        self.layout.addWidget( self.tab_widget, 0, 0, 1, 1 )

        ##
        ## add pages
        ##
        self.option_create()
        self.proxy_create()
        self.server_create()
        self.client_create()
        self.preview_create()
        self.help_create()

        self.set_defaults()

        return

    def option_create( self ):

        self.page_option = QWidget()
        self.page_option.setProperty( "HcWidgetDark", "true" )

        self.opt_layout  = QGridLayout( self.page_option )

        self.opt_label_hostbind = QLabel( "Host (Bind): " )
        self.opt_combo_hostbind = QComboBox()

        self.opt_label_port = QLabel( "Port: " )
        self.opt_input_port = QLineEdit()
        self.opt_input_port.setValidator( QIntValidator( 0, 65535 ) )

        self.opt_label_hosts = QLabel( "Hosts: " )
        self.opt_list_hosts  = HcWidgetList()
        self.opt_space_hosts = QSpacerItem( 20, 125, QSizePolicy.Minimum, QSizePolicy.Expanding )

        self.opt_label_uris = QLabel( "Uris: " )
        self.opt_list_uris  = HcWidgetList()
        self.opt_space_uris = QSpacerItem( 20, 125, QSizePolicy.Minimum, QSizePolicy.Expanding )

        self.opt_label_rotation = QLabel( "Host Rotation: " )
        self.opt_combo_rotation = QComboBox()
        self.opt_combo_rotation.addItem( "round-robin" )
        self.opt_combo_rotation.addItem( "random" )

        self.opt_label_useragent = QLabel( "User Agent: " )
        self.opt_input_useragent = QLineEdit()

        self.opt_label_hostheader = QLabel( "Host Header: " )
        self.opt_input_hostheader = QLineEdit()

        self.opt_label_secure = QLabel( "Secure: " )
        self.opt_combo_secure = QComboBox()
        self.opt_combo_secure.addItem( "true" )
        self.opt_combo_secure.addItem( "false" )

        self.opt_label_server_cert = QLabel( "Server Cert: " )
        self.opt_file_server_cert  = HcWidgetFile()
        self.opt_label_server_key  = QLabel( "Server Key : " )
        self.opt_file_server_key   = HcWidgetFile()

        self.opt_layout.addWidget( self.opt_label_hostbind,    0, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_combo_hostbind,    0, 1, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_port,        1, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_input_port,        1, 1, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_hosts,       2, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_list_hosts,        2, 1, 2, 1 )
        self.opt_layout.addItem( self.opt_space_hosts,         3, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_uris,        4, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_list_uris,         4, 1, 2, 1 )
        self.opt_layout.addItem( self.opt_space_uris,          5, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_rotation,    6, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_combo_rotation,    6, 1, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_useragent,   7, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_input_useragent,   7, 1, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_hostheader,  8, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_input_hostheader,  8, 1, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_secure,      9, 0, 1, 1 )
        self.opt_layout.addWidget( self.opt_combo_secure,      9, 1, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_server_cert, 10,0, 1, 1 )
        self.opt_layout.addWidget( self.opt_file_server_cert,  10,1, 1, 1 )
        self.opt_layout.addWidget( self.opt_label_server_key,  11,0, 1, 1 )
        self.opt_layout.addWidget( self.opt_file_server_key,   11,1, 1, 1 )

        self.tab_widget.addTab( self.page_option, "Options" )

        return

    def proxy_create( self ):

        self.page_proxy = QWidget()
        self.page_proxy.setProperty( "HcWidgetDark", "true" )

        self.pxy_layout = QGridLayout( self.page_proxy )

        self.pxy_label_enable = QLabel( "Proxy enable: " )
        self.pxy_combo_enable = QComboBox()
        self.pxy_combo_enable.addItem( "false" )
        self.pxy_combo_enable.addItem( "true" )

        self.pxy_label_type = QLabel( "Type: " )
        self.pxy_combo_type = QComboBox()
        self.pxy_combo_type.addItem( "https" )
        self.pxy_combo_type.addItem( "http" )

        self.pxy_label_host = QLabel( "Host: " )
        self.pxy_input_host = QLineEdit()

        self.pxy_label_port = QLabel( "Port: " )
        self.pxy_input_port = QLineEdit()

        self.pxy_label_user = QLabel( "Username: " )
        self.pxy_input_user = QLineEdit()

        self.pxy_label_pass = QLabel( "Password: " )
        self.pxy_input_pass = QLineEdit()

        self.pxy_spacer = QSpacerItem( 20, 125, QSizePolicy.Minimum, QSizePolicy.Expanding )

        self.pxy_layout.addWidget( self.pxy_label_enable, 0, 0, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_combo_enable, 0, 1, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_label_type,   1, 0, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_combo_type,   1, 1, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_label_host,   2, 0, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_input_host,   2, 1, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_label_port,   3, 0, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_input_port,   3, 1, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_label_user,   4, 0, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_input_user,   4, 1, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_label_pass,   5, 0, 1, 1 )
        self.pxy_layout.addWidget( self.pxy_input_pass,   5, 1, 1, 1 )

        self.pxy_layout.addItem( self.pxy_spacer, 6, 0, 1, 2 )

        self.tab_widget.addTab( self.page_proxy, "Proxy" )

        return

    def server_create( self ):

        self.page_server = QWidget()
        self.page_server.setProperty( "HcWidgetDark", "true" )

        self.srv_layout = QGridLayout( self.page_server )

        self.srv_label_headers = QLabel( "Headers: " )
        self.srv_list_headers  = HcWidgetList()
        self.srv_spacer        = QSpacerItem( 20, 125, QSizePolicy.Minimum, QSizePolicy.Expanding )

        self.srv_label_prepend = QLabel( "Prepend: " )
        self.srv_text_prepend  = QTextEdit()

        self.srv_label_append  = QLabel( "Append: " )
        self.srv_text_append   = QTextEdit()

        self.srv_layout.addWidget( self.srv_label_headers, 0, 0, 1, 1 )
        self.srv_layout.addWidget( self.srv_list_headers,  0, 1, 2, 1 )
        self.srv_layout.addWidget( self.srv_label_prepend, 2, 0, 1, 1 )
        self.srv_layout.addWidget( self.srv_text_prepend,  2, 1, 1, 1 )
        self.srv_layout.addWidget( self.srv_label_append,  3, 0, 1, 1 )
        self.srv_layout.addWidget( self.srv_text_append,   3, 1, 1, 1 )

        self.srv_layout.addItem( self.srv_spacer, 1, 0, 1, 1 )

        self.tab_widget.addTab( self.page_server, "Server" )

        return

    def client_create( self ):

        self.page_client = QWidget()
        self.page_client.setProperty( "HcWidgetDark", "true" )

        self.cli_layout = QGridLayout( self.page_client )

        self.cli_label_headers = QLabel( "Headers: " )
        self.cli_list_headers  = HcWidgetList()
        self.cli_spacer        = QSpacerItem( 20, 125, QSizePolicy.Minimum, QSizePolicy.Expanding )

        self.cli_label_prepend = QLabel( "Prepend: " )
        self.cli_text_prepend  = QTextEdit()

        self.cli_label_append  = QLabel( "Append: " )
        self.cli_text_append   = QTextEdit()

        self.cli_layout.addWidget( self.cli_label_headers, 0, 0, 1, 1 )
        self.cli_layout.addWidget( self.cli_list_headers,  0, 1, 2, 1 )
        self.cli_layout.addWidget( self.cli_label_prepend, 2, 0, 1, 1 )
        self.cli_layout.addWidget( self.cli_text_prepend,  2, 1, 1, 1 )
        self.cli_layout.addWidget( self.cli_label_append,  3, 0, 1, 1 )
        self.cli_layout.addWidget( self.cli_text_append,   3, 1, 1, 1 )

        self.cli_layout.addItem( self.cli_spacer, 1, 0, 1, 1 )

        self.tab_widget.addTab( self.page_client, "Client" )

        return

    def preview_create( self ):

        self.page_preview = QWidget()

        self.tab_widget.addTab( self.page_preview, "Preview" )

        return

    def help_create( self ):

        self.page_help = QWidget()

        self.tab_widget.addTab( self.page_help, "Help" )

        return

    def set_defaults( self ) -> None:

        listener_data = pyhavoc.core.HcListenerProtocolData( self.listener_name() )
        if listener_data != None: 
            interfaces = listener_data[ "interfaces" ]
            if interfaces != None:
                for interface in interfaces:
                    self.opt_combo_hostbind.addItem( interface )
            else:
                self.opt_combo_hostbind.addItem( "0.0.0.0" )
                self.opt_combo_hostbind.addItem( "127.0.0.1" )
        else:
            self.opt_combo_hostbind.addItem( "0.0.0.0" )
            self.opt_combo_hostbind.addItem( "127.0.0.1" )

        self.opt_input_port.setText( "443" )
        self.opt_input_port.setCursorPosition( 0 )

        self.opt_input_useragent.setText( "Mozilla/5.0 (X11; CrOS x86_64 15359.58.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.5615.134 Safari/537.36" )
        self.opt_input_useragent.setCursorPosition( 0 )

        return


    ##
    ## sanity check the given input
    ## return:
    ##  true  -> successful checked the input and nothing is wrong
    ##  false -> failed to check and something went wrong
    ##
    def sanity_check( self ) -> bool:

        if len( self.opt_input_port.text() ) == 0:
            pyhavoc.ui.HcUiMessageBox( QMessageBox.Critical, "Listener error", "port is emtpy" )
            return False

        if len( self.opt_input_useragent.text() ) == 0:
            pyhavoc.ui.HcUiMessageBox( QMessageBox.Critical, "Listener error", "user agent is empty" )
            return False

        return True

    ##
    ## pressing "save" action
    ##
    def save( self ) -> dict:
        return {
            "Options": {
                "Host Bind"    : self.opt_combo_hostbind.currentText(),
                "Port"         : self.opt_input_port.text(),
                "Hosts"        : self.opt_list_hosts.list(),
                "HostRotation" : self.opt_combo_rotation.currentText(),
                "User Agent"   : self.opt_input_useragent.text(),
                "Secure"       : self.opt_combo_secure.currentText(),
                "Server Cert"  : self.opt_file_server_cert.file(),
                "Server Key"   : self.opt_file_server_key.file(),
            },

            "Proxy": {
                "Enabled": self.pxy_combo_enable.currentText(),
                "Type"   : self.pxy_combo_type.currentText(),
                "Host"   : self.pxy_input_host.text(),
                "Port"   : self.pxy_input_port.text(),
                "User"   : self.pxy_input_user.text(),
                "Pass"   : self.pxy_input_pass.text(),
            },

            "Response": {
                "Headers": self.srv_list_headers.list(),
                "Prepend": self.srv_text_prepend.toPlainText(),
                "Append" : self.srv_text_prepend.toPlainText(),
            },

            "Request": {
                "Headers": self.cli_list_headers.list(),
                "Prepend": self.cli_text_prepend.toPlainText(),
                "Append" : self.cli_text_prepend.toPlainText(),
            }
        }