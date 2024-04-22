##
## import havoc specific libs
##
from _pyhavoc import core
from _pyhavoc import ui

##
## import qt ui library
##
from PySide6 import *
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

def HcUiGetWidgetByObjectName(
    object_name: str
) -> QWidget:

    for widget in QApplication.instance().allWidgets():
        if str( widget.objectName() ) == object_name:
            return widget

    return None

class HcPayloadView:

    def __init__( self, name: str ):
        self._hc_name = name

    ##
    ## get our builder widget
    ##
    def _hc_builder_widget( self ) -> QWidget:

        widget = HcUiGetWidgetByObjectName( "HcPageBuilder.Builder." + self._hc_name )

        if widget is None:
            raise "HcPageBuilder builder widget not found: " + "HcPageBuilder.Builder." + self._hc_name

        return widget

    ##
    ## main entrypoint what the
    ## Havoc client is going to call
    ##
    def _hc_main( self ) -> None:
        self.main( self._hc_builder_widget() )

    ##
    ## main function to be executed
    ## should create the widgets inputs
    ## for the payload builder
    ##
    def main( self, widget: QWidget ):
        pass

    ##
    ## sanity check the given input
    ## return:
    ##  true  -> successful checked the input and nothing is wrong
    ##  false -> failed to check and something went wrong
    ##
    def sanity_check( self ) -> bool:
        return True

    ##
    ## refresh the widget based on following things:
    ##  - new listener started
    ##  - new script loaded
    ##  - new agent connected
    ##
    def refresh( self ) -> None:
        pass

    ##
    ## pressing "generate" action or
    ## while saving a profile
    ##
    def generate( self, profile_save: bool = False ) -> dict:
        pass

    ##
    ## load profile
    ##
    def profile_load( self, profile: dict ) -> bool:
        pass

class HcListenerView:

    def __init__( self ):
        self._hc_name: str = None

    ##
    ## main entrypoint what the
    ## Havoc client is going to call
    ##
    def _hc_main( self ):
        self.main()

    ##
    ## set the protocol name
    ##
    def _hc_set_name( self, name: str ):
        self._hc_name = name

    ##
    ## return given listener protocol name
    ##
    def listener_name( self ) -> str:
        return self._hc_name

    ##
    ## get our protocol widget
    ##
    def listener_widget( self ) -> QWidget:

        widget = HcUiGetWidgetByObjectName( "HcListenerDialog.Protocol." + self._hc_name )

        if widget is None:
            raise "HcListenerDialog protocol widget not found: " + "HcListenerDialog.Protocol." + self._hc_name

        return widget

    ##
    ## main function to be executed
    ## should create the widgets inputs
    ## for the listener
    ##
    def main( self ):
        pass

    ##
    ## sanity check the given input
    ## return:
    ##  true  -> successful checked the input and nothing is wrong
    ##  false -> failed to check and something went wrong
    ##
    def sanity_check( self ) -> bool:
        return True

    ##
    ## pressing "save" action
    ##
    def save( self ) -> dict:
        pass

def HcUiPayloadBuilderObjName() -> str:
    return ui.HcUiPayloadBuilderObjName()

def HcUiListenerObjName() -> str:
    return ui.HcUiListenerObjName()

def HcUiGetStyleSheet() -> str:
    return ui.HcUiGetStyleSheet()

def HcUiMessageBox( icon: QMessageBox.Icon, title: str, text: str ) -> None:
    ##
    ## heh a dirty fix. maybe there is a better way to
    ## do it but this is enough for now.
    ##
    i = 0
    if icon is QMessageBox.Information:
        i = 1
    elif icon is QMessageBox.Warning:
        i = 2
    elif icon is QMessageBox.Critical:
        i = 3
    elif icon is QMessageBox.Question:
        i = 4

    ui.HcUiMessageBox( i, title, text )

    return

def HcUiListenerRegisterView( protocol: str ):

    def _register( listener_view ):
        ui.HcUiListenerRegisterView( protocol, listener_view )

    return _register

def HcUiBuilderRegisterView( payload: str ):

    def _register( builder_view ):
        ui.HcUiBuilderRegisterView( payload, builder_view )

    return _register
