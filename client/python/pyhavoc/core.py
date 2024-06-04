import threading

##
## import havoc specific libs
##
from _pyhavoc import core

def HcListenerProtocolData(
    protocol: str
) -> dict:
    return core.HcListenerProtocolData( protocol )

def HcListenerAll() -> list[str]:
    return core.HcListenerAll()

def HcListenerQueryType( name: str ) -> str:
    return core.HcListenerQueryType( name )
