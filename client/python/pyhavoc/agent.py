from _pyhavoc import agent
import threading
import time

def HcAgentRegisterInterface( type: str ):

    def _register( interface ):
        agent.HcAgentRegisterInterface( type, interface )

    return _register

def HcAgentExecute(
    uuid: str,
    data: dict,
    wait: bool = False
) -> dict:
    return agent.HcAgentExecute( uuid, data, wait )

class HcAgent:
    def __init__( self, uuid: str, type: str, meta: dict ):
        self.uuid = uuid
        self.type = type
        self.meta = meta

        return

    def agent_type( self ) -> str:
        return self.type

    def agent_uuid( self ) -> str:
        return self.uuid

    def agent_meta( self ) -> dict:
        return self.meta

    def agent_execute(
        self,
        data: dict,
        wait_to_finish: bool = False
    ) -> dict:
        """
        send the given data to the havoc
        server implant plugin to process

        :param data:
            data to send

        :param wait_to_finish:
            should it wait til it receives back a response ?

        :return:
            returned data from the executed command
        """

        return agent.HcAgentExecute( self.agent_uuid(), data, wait_to_finish )

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

        ##
        ## invoke the main input dispatcher
        ##
        self.input_dispatch( input=input )

        return