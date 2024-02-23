import threading
import time

##
## init all the files
##
from . import scriptmngr
from . import core
from . import ui
from . import agent

def HcWorkerThread() -> None:
    i = 0
    while True:
        time.sleep( 3 )
        print( f"[{i}] HcWorkerThread running..." )
        i += 1

    return

IoPythonWorker = core.HcIoPythonWorker()
IoPythonWorker.add_worker( HcWorkerThread )
# IoPythonWorker.start()