import sys,string

try:
    fp = open(sys.argv[1])
except:
    fp = open('parser.y')

s = fp.read().split('%%')

def consumeComments(x):
    while x and x[0] == '/*':
        while x and x[0] != '*/':
            x = x[1:]
        x = x[1:]
    return x

character = {"'('" : 'lparen',
             "')'" : 'rparen',
             "','" : 'comma',
             "';'" : 'semicolon',
             "':'" : 'colon',
             "'+'" : 'plus',
             "'-'" : 'minus',
             "'*'" : 'times',
             "'/'" : 'slash',
             }
current = None
tokens = s[1].split()
all = []
while tokens:
    tokens = consumeComments(tokens)

    # Read name & :
    current = tokens[0]
    tokens = tokens[1:]
    all.append( (current,[]) )
    assert tokens[0] == ':','expected :'
    tokens = tokens[1:]

    # Read a clause
    clause = []
    while tokens:
        if tokens[0] not in [';','|','{']:
            try:
                name = character[tokens[0]]
            except:
                name = tokens[0]
            # Adjust name for duplicates...
            while name in clause:
                name += "_"
            clause.append(name)
            tokens = tokens[1:]
        else:
            # Skip any action
            if tokens[0] == '{':
                tokens = tokens[1:]
                assert tokens[0][0] in string.letters
                tokens = tokens[1:]
                assert tokens[0] == '}'
                tokens = tokens[1:]

            # if ; then done
            elif tokens[0] == ';':
                all[-1][1].append(clause)
                tokens = tokens[1:]
                break

            # if |, then another clause
            elif tokens[0] == '|':
                all[-1][1].append(clause)
                tokens = tokens[1:]
                clause = []
            else:
                raise 'oops'

print """
class mySALParser:
    def token(self,text,fileName,startLine,startColumn,endLine,endColumn):
        return text
"""
for label,clauses in all:

    # Handle any error clauses and remove
    if ['error'] in clauses:
        print '    def %s_error( self, bad ):'%label
        print '        raise RuntimeError,"%s error"'%label
        clauses = filter(lambda x: x != ['error'], clauses)

    # If only one clause, use simple form of name
    if len(clauses) == 1:
        namer = lambda x,label=label: label
    else:
        namer = lambda x,label=label: "%s_%d"%(label,x)
        
    lens = map(len,clauses)
    unique = []
    for L in lens:
        if L not in unique:
            unique.append(L)

    for L in unique:
        firstClause = None
        lastClause = None
        for c in clauses:
            if len(c) == L:
                lastClause = c
                if firstClause == None:
                    firstClause = c

        # Decide if we need to remap the clause
        if firstClause == lastClause:
            arguments = firstClause
        else:
            arguments = map(lambda x: 'arg%d'%x, range(1,L+1))
        
        print "    def %s( self,"%namer(L),
        print string.join(arguments,','),
        print '):'
        if len(arguments) == 0:
            print '        return None'
        elif len(arguments) == 1:
            print '        return',arguments[0]
        else:
            print '        raise RuntimeError,"%s"'%label
