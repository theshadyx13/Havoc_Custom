import pyhavoc
import os

from os.path import exists

@pyhavoc.agent.HcKaineRegister
class KnObjectExecute( pyhavoc.agent.HcKaineCommand ):
    ##
    ## specify the command metadata
    ##
    def __init__( self, *args, **kwargs ):
        super().__init__( *args, **kwargs )

        self.command     = "object-execute"
        self.description = "execute an object file in memory"

        return

    ##
    ## this function gets invoked once the
    ## operator executes the command
    ##
    def execute( self, args: list[str] ):
        ##
        ## check if object file exists
        ##
        file_path = ' '.join( args )
        if exists( file_path ) is False:
            self.agent().console_log( f"object file not found: {file_path}" )
            return

        ##
        ## read object file from disk
        ##
        handle = open( file_path, 'rb' )
        object = handle.read()
        handle.close()

        ##
        ## inform the operator that we generated
        ## a task to execute the object file
        ##
        self.agent().console_log( f"tasked agent to execute object file: { file_path }" )

        ##
        ## invoke object file
        ##
        ctx = self.agent().object_execute( object, wait_to_finish=True )

        ##
        ## check if status is STATUS_SUCCESS
        ##
        if ctx[ 'status' ] == 0:
            self.agent().console_log( f"successful executed object file" )
        else:
            self.agent().console_log( f"failed to execute object file [status: { ctx[ 'status' ] }] [error: { ctx[ 'return' ] }]" )

        return
