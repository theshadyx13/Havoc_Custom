from _pyhavoc import agent
import threading
import time

class HcAgent:
    def __init__( self, uuid: str, type: str, meta: dict ):
        self.uuid = uuid
        self.type = type
        self.meta = meta

        self.__threads = []

        return

    def agent_type( self ) -> str:
        return self.type

    def agent_uuid( self ) -> str:
        return self.uuid

    def agent_meta( self ) -> dict:
        return self.meta

    ##
    ## prints the given text to the console
    ##
    def console_print( self, text: str ) -> None:
        agent.HcAgentConsoleWrite( self.agent_uuid(), text )
        return

    ##
    ## sends the console output also across
    ## clients and to the teamserver
    ##
    def console_log( self, text: str ) -> None:
        self.console_print( text )
        return

    def input_dispatch( self, input: str ):
        pass

    def _input_dispatch( self, input: str ):

        thread = threading.Thread( target=self.input_dispatch, args=(input,) )
        thread.start()

        HcWaitForThread( thread )

        return

def HcAgentRegisterInterface( type: str ):

    def _register( interface ):
        agent.HcAgentRegisterInterface( type, interface )

    return _register