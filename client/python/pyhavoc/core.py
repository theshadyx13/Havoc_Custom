import threading

##
## import havoc specific libs
##
from _pyhavoc import core

class HcIoPythonWorker:

    def __init__( self ):

        self.threads = []

        pass

    def add_worker( self, function ):
        thread = threading.Thread( target=function )
        thread.start()
        self.threads.append( thread )

        return

    def add_thread( self, thread ):
        thread.start()
        self.threads.append( thread )
        return

    def start( self ):

        while True:
            for thread in self.threads:
                thread.join()

        return

def HcListenerProtocolData(
    protocol: str
) -> dict:
    return core.HcListenerProtocolData( protocol )

def HcListenerAll() -> list[str]:
    return core.HcListenerAll()
