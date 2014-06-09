import inspect
import SystemsExports;
import OISExports;
import MessagesExports;

def logger(func):
    def inner(*args, **kwargs):
        #print( "%s Arguments were: %s, %s" % (func.__name__,args, kwargs))
        return func(*args, **kwargs)
    return inner

class SystemWrapperBase:
    @logger
    def __init__(self,systemId):
       print("SystemWrapper init system",systemId)
       self.systemId = systemId;

       print (str(inspect.getfile(inspect.currentframe())) + " init")

    @logger
    def registerOperation(self,msgId , func):
        SystemsExports.registerOperation(self.getSystemId(),func,msgId);
        

    def execute(self, msg):
        operation = self.operations[msg.getItemTypeId()];
        operation(msg);

    def getSystemId(self):
        return self.systemId;

    operations = dict();
    systemId = 0;
