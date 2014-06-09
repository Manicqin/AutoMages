# import SystemsExports;
# import MessagesExports;
# 
# def printData(msg):
#     if  (msg.getItemTypeId() == MessagesExports.StringDataMsg.getItemTypeId() or 
#          msg.getItemTypeId() == MessagesExports.FloatDataMsg.getItemTypeId() or
#          msg.getItemTypeId() == MessagesExports.IntDataMsg.getItemTypeId()):
#             SystemsExports.LogFunc(str(msg.getData()) + " " + MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()) + " " + str(msg.getItemTypeId()));
#     elif (msg.getItemTypeId() == MessagesExports.StringComposerMsg.getItemTypeId()):
#             SystemsExports.LogFunc(MessagesExports.MessagesRegistry.getItemName(msg.getMessageType()) + " " + str(msg.getItemTypeId()));
#             for txt in msg.getData():
#                 SystemsExports.LogFunc("    " + txt);
# 
# printData(MessagesExports.StringDataMsg("StringDataMsg"));
# printData(MessagesExports.IntDataMsg(10));
# printData(MessagesExports.FloatDataMsg(10.1));
# 
# vecMsg = MessagesExports.StringComposerMsg();
# vecMsg.addData("aaa");
# vecMsg.addData("bbb");
# vecMsg.addData("ccc");
# printData(vecMsg);
# 
# test = MessagesExports.StringDataMsg("Sent from python!!!");
# SystemsExports.LogFunc("1");
# sysChan.addMsg(test.createSharedPtr());
# SystemsExports.LogFunc("2");
# sysChan.addMsg(vecMsg.createSharedPtr());
# #SystemsExports.getSystemChannelIn(41).addMsg(test2);
# 
# 
# SystemsExports.LogFunc("success");

import xml.etree


rank = 0;

def printList(x):
    global rank
    rank = rank +1;
    for pair in x:
        if isinstance(pair, (list, tuple)):
            printList(pair)
        else:
            if isinstance(pair, (dict)):
                print pair.keys() , pair.values() , rank
    rank = rank - 1
a = ({'a':'s'},{'s':'d'},({'b':'s'},{'aa':'ss'}),{'ss':'342'})
printList(a) 