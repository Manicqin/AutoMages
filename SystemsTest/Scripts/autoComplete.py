"""
    Help macro to auto Complete
    Recieves wordToComplete as text
    you can set resultsPerLine
"""
    
import SystemsExports;
import MessagesExports;
import rlcompleter 
import readline
"""
def formatRow(arrOfStrings):
        print ''.join([col.center(10," ") for col in arrOfStrings])
"""        
def getAutoCompleteList(x):
    iDx = 0
    retVal = ""
    autoCompleteRet = []
    while (retVal is not None):
            retVal = readline.get_completer()(x,iDx)
            if (retVal is not None):
                    pos = retVal.rfind(".")
                    if(pos != -1):
                            retVal = retVal[pos:]
                    autoCompleteRet.append(retVal)
            iDx = iDx + 1
    return autoCompleteRet
def getFirstAutoComplete(x):
    retVal = ""
    retVal = readline.get_completer()(x,0)    
    return retVal;


lineWidth = 75;
wordsPerLine = 3;
maxWordSizeWithPadding = lineWidth / wordsPerLine;
maxWordSizeWithoutPadding = maxWordSizeWithPadding - 2;
preWord = (maxWordSizeWithoutPadding) / 2;
suWord = (maxWordSizeWithoutPadding - 3) - preWord;
delimiters = " "
"""
debug = false
if (debug):
        print "lineWidth " + str(lineWidth)
        print "wordsPerLine " + str(wordsPerLine)
        print "maxWordSizeWithoutPadding " + str(maxWordSizeWithoutPadding)
        print "maxWordSizeWithPadding " + str(maxWordSizeWithPadding)
        print "preWord " + str(preWord)
        print "suWord " + str(suWord)
"""

print 'a';
print wordToComplete;
test = MessagesExports.StringDataMsg(getFirstAutoComplete(wordToComplete));
sysChan.addMsg(test.createSharedPtr());
print 'b';
autoCompleteRet = getAutoCompleteList(wordToComplete)                                      
autoCompleteRet.sort(key= lambda val: (val,len(val)))
autoCompleteRet2 = [[]]

iDx = 0
for val in autoCompleteRet:
        sizeOfStr = len(val)
        if sizeOfStr > maxWordSizeWithoutPadding:
                val = (val[0:preWord]+"..."+val[0-suWord:])
        if((len(autoCompleteRet2[iDx])+1)*maxWordSizeWithoutPadding < lineWidth):
                autoCompleteRet2[iDx].append(val.ljust(maxWordSizeWithPadding,delimiters))
                        
        else:
                iDx = iDx + 1
                autoCompleteRet2.append([])
                autoCompleteRet2[iDx].append(val.ljust(maxWordSizeWithPadding,delimiters))
                        
for var in autoCompleteRet2:
    SystemsExports.LogFunc( "".join(var));
        

"""
if (len(formatRet) == resultsPerLine):
        formatRow(formatRet)
        formatRet = []

	

if (len(formatRet) != 0):
        formatRow(formatRet)
"""
