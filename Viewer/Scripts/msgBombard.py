import SystemsExports;
import MessagesExports;

Stub1SysId  = SystemsExports.getSystemByName("StubSystem1");        
Stub2SysId  = SystemsExports.getSystemByName("StubSystem2");

for iDx in range(1000):
    msg = MessagesExports.IntDataMsg(iDx);
    SystemsExports.getSystemChannelIn(Stub2SysId).addMsg(msg);
    SystemsExports.getSystemChannelIn(Stub1SysId).addMsg(msg);
