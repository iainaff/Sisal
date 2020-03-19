import string,os,sys

print
print
data = string.split(open('parser.y').read())
charMap = { "'('" : 'LPAREN_',
            "')'" : 'RPAREN_',
            "'['" : 'LBRACKET_',
            "']'" : 'RBRACKET_',
            "','" : 'COMMA_',
            "':'" : 'COLON_',
            "';'" : 'SEMI_',
            "'.'" : 'PERIOD_',
            }
def convert(s):
    if s in charMap.keys():
        return charMap[s]
    else:
        return s

def addType(s): return 'semanticBase* ' + s

protos = []
for i in range(0,len(data)):
    if data[i][:9] == 'mParser->':
        method = data[i]
        call = string.split(method,'->')[1]
        [name,dollarArgs] = string.split(call,'(')
        if name == 'labelType':
            print dollarArgs
        # Count the number of $ arguments
        n = len(string.split(dollarArgs,'$'))-1
        # Count the total number of arguments
        if ',' in dollarArgs:
            n2 = len(string.split(dollarArgs,','))
        else:
            n2 = 0
        if name == 'labelType':
            print n,n2
        j = i
        while j >= 1 and data[j-1] != ':' and data[j-1] != '|':
            j = j-1
        argNames = data[j:j+n]
        argNames = map(string.upper,argNames)
        argNames = map(convert,argNames)
        for i in range(len(argNames)):
            if argNames[i] in argNames[:i]:
                argNames[i] = argNames[i]+'2'
        args = map(addType,argNames)+(['const char* msg']*(n2-n))
        args = string.join(args,', ')
        proto = '%s(%s)'%(name,args)
        stuff = (name,argNames,proto,n)
        if stuff not in protos:
            protos.append(stuff)

# Save the old one
open('SisalParser.hh.bu','w').write(open('SisalParser.hh').read())

# Read in previous SisalParser.hh, skipping old prototypes
hhLines = open('SisalParser.hh.bu')
hhFile = open('SisalParser.hh','w')
while 1:
    line = hhLines.readline()
    if not line: break
    hhFile.write(line)
    if string.split(line) == ['//','PARSER','RULES']:
        while string.strip(line):
            line = hhLines.readline()
            #print 'X',string.strip(line)
        break
    
# Add replacement prototypes
for name,argNames,proto,n in protos:
    hhFile.write('      virtual semanticBase* %s;\n'%proto)

# Required blank line
hhFile.write('\n')

# Rest of the file
while 1:
    line = hhLines.readline()
    if not line: break
    hhFile.write(line)

# Display a diff for sanity
hhFile.flush()
hhFile.close()
print 'diff',os.popen('diff SisalParser.hh SisalParser.hh.bu').read()

# Only need to cut & paste in new functions
for name,argNames,proto,n in protos:
    p = os.popen('fgrep "%s" SisalParser.cc'%proto)
    p.read()
    if p.close() != None:
        print 'semanticBase* SisalParser::%s {'%proto
        for argName in argNames:
            arg = argName
            if arg[:3] == 'OPT': arg = arg[3:]
            if arg[-1] == '2': arg = arg[:-1]
            if arg[-1:] == '_' or arg[-7:] == 'LITERAL' or arg == 'ID' or arg == 'PUBLIC':
                T = 'token'
            elif arg == 'ERROR':
                continue # Nothing to check
            else:
                T = string.lower(arg)
                T = string.split(T,'_')
                for i in range(len(T)):
                    T[i] = string.upper(T[i][0])+T[i][1:]
                T = string.join(T,'')
            if argName[:3] != 'OPT':
                print '   assert(%s);\t'%argName,
            print ' %s* %s = dynamic_cast<%s*>(%s);'%(T,string.lower(argName),T,argName),
            if argName[:3] != 'OPT':
                print ' assert(%s);'%(string.lower(argName))
            else:
                print ' assert(!%s || %s);'%(argName,string.lower(argName))

        print '   return 0;'
        print '}'
    
