import mysal,mysalparser
import string,sys,time


##################################################################
#                      CLASS FUNCTIONRECORD                      #
##################################################################
class FunctionRecord:
    def __init__(self,id,types,defined=0):
	self.id		= id
	self.types	= types
	self.defined	= defined
	return

##################################################################
#                     CLASS SYMBOLTABLEENTRY                     #
##################################################################
class SymbolTableEntry:
    def __init__(self,name,unique,type=None):
	self.name	= name
	self.unique	= unique
        self.type       = type
	return

    def setType(self,type):
        self.type = type

    def __str__(self):
        return str(self.unique)

    def __repr__(self):
        return '<st %s : %s>'%(self.name,self.type)


##################################################################
#                       CLASS SYMBOLTABLE                        #
##################################################################
class SymbolTable:
    def __init__(self):
        self.__symbols = {}
        
    def lookup(self,name):
        sym = self.__symbols[str(name)]
        return sym

    def new(self,*args,**kw):
        name = str(kw['name'])
        newSymbol = apply(SymbolTableEntry,args,kw)
        if name in self.__symbols.keys():
            raise ValueError
        self.__symbols[name] = newSymbol
        return
        

##################################################################
#                     CLASS SYMBOLTABLESTACK                     #
##################################################################
class SymbolTableStack:
    def __init__(self):
        self.__stack = []
        return
    
    def open(self):
        self.__stack.append(SymbolTable())
        return
    
    def close(self):
        try:
            del self.__stack[-1]
        except:
            pass

    def __getattr__(self,a):
        assert self.__stack
        return getattr(self.__stack[-1],a)

    def lookup(self,name):
        for i in range(1,len(self.__stack)+1):
            try: return self.__stack[-i].lookup(name)
            except: pass
        raise KeyError,name

##################################################################
#                          CLASS TOKEN                           #
##################################################################
class token:
    def __init__(self,text='',fileName='unknown',startLine=0,startColumn=0,endLine=0,endColumn=0):
        self.text	= text
        self.fileName	= fileName
        self.startLine	= startLine
        self.startColumn= startColumn
        self.endLine	= endLine
        self.endColumn	= endColumn
        return

    def __str__(self):
        return self.text
    def __repr__(self):
        return '<token %s>'%self.text
    def startFrom(self,mark):
        assert isinstance(mark,token)
        self.fileName = mark.fileName
        self.startLine = mark.startLine
        self.startColumn = mark.startColumn
        return
    def endFrom(self,mark):
        assert isinstance(mark,token)
        self.endLine	= mark.endLine
        self.endColumn	= mark.endColumn
        return
    def clone(self,mark):
        assert isinstance(mark,token)
        self.startFrom(mark)
        self.endFrom(mark)
        return

##################################################################
#                           CLASS EXPR                           #
# expressions are a code set to lay down and a
# list of names
##################################################################
class expr(token):
    def __init__(self,code="",values=[],start=None,end=None,clone=None):
        token.__init__(self)
        self.code	= code
        self.values	= values
        if start: self.startFrom(start)
        if end: self.endFrom(end)
        if clone: self.clone(clone)
        return
    def indent(self):
        s = self.code.split('\n')
        sIndent = map(lambda x: '   '+x, filter(None,s))
        self.code = string.join(sIndent,'\n')
        return self.code
    def append(self,other):
        if isinstance(other,expr):
            self.code += '\n'
            self.code += other.code
            self.values += other.values
        else:
            self.code += '\n'
            self.code += other
        return self
    def value(self):
        assert len(self) == 1
        return self.values[0]
    def typeString(self):
        return str(self.value().type)
    def rhs(self):
        return string.join(map(str,self.values),',')
    def types(self):
        return map(lambda x: x.type, self.values)
    def __len__(self):
        return len(self.values)
    def __repr__(self):
        return "<%s = %s %s:%d>"%(self.values,repr(self.code),self.fileName,self.startLine)
    def __getitem__(self,i):
        return self.values[i]


##################################################################
#                   CLASS MYSALIMPLEMENTATION                    #
##################################################################
class mySALImplementation(mysalparser.mySALParser):
    # Class static
    token = token
    expr = expr

    def __init__(self,file):
        self.__file = file
        self.__symbols = SymbolTableStack()
        self.__known = {}
        self.__functions = {}
        self.__boolean = self.token('boolean','system',0,0,0,0)
        self.__integer = self.token('integer','system',0,0,0,0)
        self.__double = self.token('double','system',0,0,0,0)
        self.__zero = self.token('0','system',0,0,0,0)
        return

    # -----------------------------------------------
    # Mangle to unique function name
    # -----------------------------------------------
    def mangle(self,function,types):
        if str(function) == 'main': return 'main'
        types = map(str,types)
        return '_%s_of_%s'%(function,string.join(types,'_'))

    # -----------------------------------------------
    # We need to be able to drive the file reader
    # -----------------------------------------------
    def name(self):
        return self.__file.name

    def read(self,n):
        return self.__file.read(n)

    # -----------------------------------------------
    # Convenient facility to generate unique "readable"
    # names
    # -----------------------------------------------
    def unique(self,base):
        keys = self.__known.keys()
        i = 0
        name = str(base)
        while name in keys:
            i += 1
            name = '%s_%d'%(base,i) 
        self.__known[name] = base
        return name

    def temporary(self,dummy=0):
        name = self.unique('_t')
        self.setName(name,name)
        expr = self.lookupName(name)
        return expr

    def uniqueTemporaries(self,n):
        return map(self.temporary, range(n))

    # -----------------------------------------------
    # This is the standard typer for arithmetic ops
    # -----------------------------------------------
    def type_arithmetic(self,A,op,B):
        typeA = str(A.value().type)
        typeB = str(B.value().type)
        ok = { ('integer','integer') : 'integer',
               ('integer','double') : 'double',
               ('double','double') : 'double',
               ('double','integer') : 'double',
               }
        try:
            result = ok[(typeA,typeB)]
        except:
            self.error(op,msg='invalid operand types (%s,%s)'%(typeA,typeB))
            result = A.value().type
        return result

    def type_equals(self,A,op,B):
        typeA = str(A.value().type)
        typeB = str(B.value().type)
        ok = { ('integer','integer') : 'boolean',
               ('integer','double') : 'boolean',
               ('double','double') : 'boolean',
               ('double','integer') : 'boolean',
               ('boolean','boolean') : 'boolean',
               }
        try:
            result = ok[(typeA,typeB)]
        except:
            self.error(op,msg='invalid operand types (%s,%s)'%(typeA,typeB))
            result = self.__boolean
        return result

    def type_compare(self,A,op,B):
        typeA = str(A.value().type)
        typeB = str(B.value().type)
        ok = { ('integer','integer') : 'boolean',
               ('integer','double') : 'boolean',
               ('double','double') : 'boolean',
               ('double','integer') : 'boolean',
               }
        try:
            result = ok[(typeA,typeB)]
        except:
            self.error(op,msg='invalid operand types (%s,%s)'%(typeA,typeB))
            result = self.__boolean
        return result

    # -----------------------------------------------
    # Write in standard error format
    # -----------------------------------------------
    def error(self,x,msg='',*args):
        sys.stderr.write( '\n%s:%d: Error, %s\n'%(x.fileName,x.startLine,msg) )
        return

    # -----------------------------------------------
    # We use the prologue as a stub to get bootstrapped
    # into some names
    # -----------------------------------------------
    def prologue(self):
        print 'import sisalrt'

    # -----------------------------------------------
    # We maintain a dictionary of known names in
    # each scope as we open it
    # -----------------------------------------------
    def openScope(self):
        self.__symbols.open()
        return

    def closeScope(self):
        self.__symbols.close()
        return

    def lookupName(self,name):
        return self.__symbols.lookup(name)
            
    def setName(self,name,unique):
        self.__symbols.new(name=name,unique=unique)

    # -----------------------------------------------
    # Id lists map names to ID tokens
    # -----------------------------------------------
    def idList_1(self,id_):
        # Enter value into scope
        u = self.unique(id_)
        self.setName(id_,u)
        return [u]

    def idList_3(self,idList,comma,id_):
        idList += self.idList_1(id_)
        return idList

    # -----------------------------------------------
    # Paramemter lists map over types
    # -----------------------------------------------
    def parameters_1( self, parameter ):
        return parameter
    def parameters_3( self, parameters,semi,parameter ):
        import types
        if type(parameter) == types.ListType:
            parameters += parameter
        else:
            parameters.append(parameter)
        return parameters
    def parameter( self, idList,colon,type ):
        syms = []
        for id in idList:
            sym = self.lookupName(id)
            sym.setType(type)
            syms.append(sym)
        return syms
    def optParameters_0(self):
        return []

    # -----------------------------------------------
    # We use the typelist here just to figure out
    # return count
    # -----------------------------------------------
    def typeList_1( self, type ):
        return [type]
    def typeList_3( self, typeList,comma,type ):
        typeList.append(type)
        return typeList

    # -----------------------------------------------
    # Very simple expression, e.g.     x
    # -----------------------------------------------
    def idExpr(self,id_):
        try:
            id = self.lookupName(id_)
            expr = self.expr(code='',values=[id],clone=id_)
        except KeyError:
            self.error(id_, msg="Unknown identifier %s"%id_)
            expr = self.integerConst(self.__zero)
        return expr

    # -----------------------------------------------
    # Function call
    # -----------------------------------------------
    def functionCall( self, id,lparen,expression,rparen ):
        # Generate the mangled name for lookup
        name = self.mangle(id,expression.types())

        # It should be defined by now...
        if not self.__functions.has_key(name):
            self.__functions[name] = FunctionRecord(id, [self.__integer] ) # signature and body count
            self.error(id,msg="Undefined function %s"%id)

        # We need to hold these results
        results = self.uniqueTemporaries(len(self.__functions[name].types))
        lhs = string.join(map(str,results),',')

        # Set return types
        for result,type in map(None, results,self.__functions[name].types):
            result.setType(type)

        # Do the work for the current expression
        value = self.expr(expression.code,results,start=id,end=rparen)
        value.append('%s = %s(%s)'%(lhs,name,expression.rhs()))

        return value
    
    def optExpression_0( self ):
        return self.expr()

    # -----------------------------------------------
    # Handle constant values as special expressions
    # with no code
    # -----------------------------------------------
    def setConst(self,literal,type):
        try:
            self.setName(literal,literal)
        except:
            pass
        value = self.lookupName(literal)
        value.setType(type)
        expr = self.expr(code='',values=[value],clone=literal)
        return expr

    def integerConst(self,integerLiteral):
        return self.setConst(integerLiteral,self.__integer)

    def doubleConst(self,doubleLiteral):
        return self.setConst(doubleLiteral,self.__double)

    # -----------------------------------------------
    # Arithmetics just use either inline or special
    # name functions
    # -----------------------------------------------
    intrinsics = {
        'mod': ('_modulus', type_arithmetic),
        'div': ('_divide', type_arithmetic),
        '+': ('+', type_arithmetic),
        '-': ('-', type_arithmetic),
        '*': ('*', type_arithmetic),
        '/': ('/', type_arithmetic),
        '==': ('==', type_equals),
        '<': ('<', type_compare),
        '<=': ('<=', type_compare),
        '>': ('>', type_compare),
        '>=': ('>=', type_compare),
        }

    def infix_3(self, A, op, B):
        # These work only on unary expressions
        assert len(A) == 1
        assert len(B) == 1
        
        # The code I want is t = A op B or maybe t = op(A,B)
        function, typer = self.intrinsics[str(op)]
        if str(op) == function:
            work = '(%s %s %s)'%(A.value(),op,B.value())
        else:
            work = '%s(%s,%s)'%(function,A.value(),B.value())

        # Get a temporary and set its type
        type = typer(self, A,op,B)
        expr = self.temporary()
        expr.setType(type)

        code = A.code + '\n' + B.code + "\n%s = %s"%(expr, work)
        result = self.expr(code,[expr],start=A,end=B)
        return result

    # -----------------------------------------------
    # If is really easy, just check the test and do the
    # true or false part accordingly
    # -----------------------------------------------
    def ifExpr( self, if_,boolExpr,then_,truePart,else_,falsePart,end_,if__ ):
        if len(boolExpr) != 1:
            self.error(boolExpr,msg="non-boolean test")
            return self.integerConst(self.__zero)
        if len(truePart) != len(falsePart):
            self.error(end_,msg="mis-matched arity in true (%d)/false (%d)  branches"%(len(truePart),len(falsePart)))
            return self.integerConst(self.__zero)
        if str(boolExpr.typeString()) != 'boolean':
            self.error(boolExpr,msg='test is not a boolean')

        # The true and false branches must have the same types so
        # that the result values can be given known types
        names = self.uniqueTemporaries(len(truePart))
        for i in range(0,len(truePart)):
            if str(truePart[i].type) != str(falsePart[i].type):
                self.error(end_,msg='position %d mismatched types %s in true part, %s in false part'%(i+1,truePart[i].type,falsePart[i].type))
            names[i].setType(truePart[i].type)

        lhs = string.join(map(str,names),',')
        truePart.append('%s = %s'%(lhs,truePart.rhs()))
        falsePart.append('%s = %s'%(lhs,falsePart.rhs()))
        code = boolExpr.code + '\n' + 'if %s:\n'%boolExpr.value()\
               +truePart.indent()\
               +'\nelse:\n'\
               +falsePart.indent()
        expr = self.expr(code,names,start=if_,end=if__)
        return expr

    # -----------------------------------------------
    # Name bindings
    # -----------------------------------------------
    def optNameList_0(self):
        return self.expr()
    def nameList( self, nameListBody,semi ):
        return nameListBody
    def nameListBody_1( self, nameEntry ):
        return nameEntry
    def nameListBody_3( self, nameList,semi,nameEntry ):
        nameList.append(nameEntry)
        return nameList
    def nameEntry(self, idList,equals,expression):
        code = expression.code+'%s = %s'%(string.join(idList,','),expression.rhs())
        return self.expr(code,idList)

    # -----------------------------------------------
    # Let expressions use name bindings
    # -----------------------------------------------
    def letExpr( self, let_,openScope,nameList,in_,expression,closeScope,end_,let__ ):
        expr = self.expr('',expression.values,start=let,end=let__)
        expr += nameList.code
        expr += expression.code
        return expr

    # -----------------------------------------------
    # Range expressions
    # -----------------------------------------------
    def rangeClause( self, id,in_,expression ):
        assert len(expression) == 2 or len(expression) == 3
        # Make sure all sub expressions are integers
        for subexpr in expression:
            assert str(subexpr.type) == 'integer'

        id = str(id)
        u = self.unique(id)
        self.setName(id,u)
        sym = self.lookupName(id)
        sym.setType(self.__integer)
        return (u,expression)
    def rangeExpr_1( self, rangeClause ):
        return [rangeClause]
    def rangeExpr_3( self, arg1,arg2,arg3 ):
        assert str(arg2) == 'dot'
        raise RuntimeError,"rangeExpr"

    # -----------------------------------------------
    # Returns clauses for forall loops
    # -----------------------------------------------
    def arrayReturnClause( self, arrayReturn,of_,singleton ):
        return (arrayReturn,singleton)
    def arrayReturnBody_1( self, arrayReturnClause ):
        return [arrayReturnClause]
    def arrayReturnBody_3( self, arrayReturnBody,semi,arrayReturnClause ):
        arrayReturnBody.append(arrayReturnClause)
        return arrayReturnBody
    def arrayReturns( self, arrayReturnBody,optSemi ):
        return arrayReturnBody

    # -----------------------------------------------
    # Forall loops
    # -----------------------------------------------
    def forExpr( self, for_,openScope,rangeExpr,optNameList,returns_,arrayReturns,end_,for__ ):
        # We need prelude code for the range generators
        code = ''
        drivers = []
        for name, expr in rangeExpr:
            code += '\n'+expr.code
            if len(expr) == 1:
                raise RuntimeError
            elif len(expr) == 2:
                drivers.append( '%s in range(%s,%s+1)'%(name,expr[0],expr[1]) )
            elif len(expr) == 3:
                drivers.append( '%s in range(%s,%s+%s,%s)'%(name,expr[0],expr[1],expr[2],expr[2]) )
            else:
                self.error(expr,msg="Range arity must be 1,2, or 3")

        # We need prelude code for all the values gathered
        names = []
        for kind,work in arrayReturns:
            if str(kind) == 'array':
                expr = self.temporary()
                code += '%s = _array()\n'%expr
                names.append(expr)
            else:
                raise RuntimeError

        # A single range is a special case (no dotting)
        if len(rangeExpr) == 1:
            code += 'for %s:\n'%drivers[0]
        else:
            raise RuntimeError

        # Add in the body code
        optNameList.indent()
        code += optNameList.code

        # Add in the code that the return clauses need
        i = 0
        for kind,work in arrayReturns:
            work.indent()
            code += work.code+'\n'
            if str(kind) == 'array':
                name = names[i]
                code += '    %s.append(%s)\n'%(name,work.value())
            else:
                raise RuntimeError

        return self.expr(code,names)

    # -----------------------------------------------
    # Expressions just join singletons
    # -----------------------------------------------
    def expression_3(self, expression, comma, singleton):
        expression.append(singleton)
        expression.endFrom(singleton)
        return expression

    # -----------------------------------------------
    # Functions bind arguments to an expression
    # -----------------------------------------------
    def function_common(self, id,lparen,openScope,optParameters,returns_,typeList,rparen,closeScope ):
        # -----------------------------------------------
        # Get the input and output types...
        # -----------------------------------------------
        inTypes = map(lambda x: str(x.type), optParameters)
        inNames = map(lambda x: str(x.name), optParameters)
        outTypes = map(str, typeList)

        # -----------------------------------------------
        # That drives the mangled name
        # -----------------------------------------------
        name = self.mangle(id,inTypes)

        # -----------------------------------------------
        # Make sure we have a registry for this.
        # -----------------------------------------------
        if not self.__functions.has_key(name):
            self.__functions[name] = FunctionRecord(id, outTypes) # signature and body count

        # -----------------------------------------------
        # Some basic checking for correctness
        # -----------------------------------------------
        if outTypes != self.__functions[name].types:
            self.error(id, msg="New definition of %s does not match previous output signature"%id)
            self.error(self.__functions[name].id, msg="Previous definition")

        # -----------------------------------------------
        # We shouldn't see a definition if the "body" flag
        # has been set
        # -----------------------------------------------
        if self.__functions[name].defined:
            self.error(id, msg="Function %s already has a body"%id)
            self.error(self.__functions[name].id, msg="Previous definition")
        
        return inTypes,inNames,outTypes,name

    def function_expression_check(self, end_, expression, typeList):
        L = min(len(expression),len(typeList))
        if L != len(expression):
            self.error(end_,msg='not enough values for return')
        if L != len(typeList):
            self.error(end_,msg='too many values for return')
        for i in range(0,L):
            if str(expression[i].type) != str(typeList[i]):
                self.error(end_,msg='bad type result %d expected %s and got %s'%(i+1,typeList[i],expression[i].type))
        return

    def function_10( self, function_,id,lparen,openScope,optParameters,returns_,typeList,rparen,semi,closeScope ):
        inTypes,inNames,outTypes,name = self.function_common(id,lparen,openScope,optParameters,returns_,typeList,rparen,closeScope )
    
    def function_11( self, main_,lparen,openScope,optParameters,returns_,typeList,rparen,expression,closeScope,end_,main__ ):
        inTypes,inNames,outTypes,name = self.function_common(main_,lparen,openScope,optParameters,returns_,typeList,rparen,closeScope )
        self.__functions[name].defined = 1  # Mark as having a body
        
        # We build an expression to hold main
        main = self.expr('',expression.values)

        # We first need to grab all inputs
        main.append('sisalrt._start()')
        for i in range(0,len(inNames)):
            main.append('%s = sisalrt._input%s()'%(inNames[i],inTypes[i]))

        # Now the code that does all the work
        main.append('sisalrt._begin()')
        main.append(expression.code)
        main.append('sisalrt._end()')

        # Now, handle all the outputs
        for value,type in map(None, expression.values, typeList):
            main.append('sisalrt._output%s(%s)'%(type,value))
        main.append('sisalrt._finish()')

        # Here is the body of work
        print 'if __name__ == "__main__":'
        main.indent()
        print main.code
        
        self.function_expression_check(end_, expression, typeList)

    def function_13( self, function_,id,lparen,openScope,optParameters,returns_,typeList,rparen,expression,closeScope,end_,function__,id_ ):
        inTypes,inNames,outTypes,name = self.function_common(id,lparen,openScope,optParameters,returns_,typeList,rparen,closeScope )
        self.__functions[name].defined = 1  # Mark as having a body

        # Make sure tail id matches
        if str(id) != str(id_):
            self.error(id_, msg="Tail id %s doesn't match function name %s"%(id_,id))

        print '# -----------------------------------------------'
        print "def %s(%s):"%(name,string.join(inNames,','))
        expression.append('return %s # %s'%(expression.rhs(),string.join(map(str,expression.types()))))
        expression.indent()
        print expression.code
        print '# -----------------------------------------------'

        self.function_expression_check(end_, expression, typeList)
        return name,typeList

    def program(self,prologue,functionList):
        return functionList
    def functionList_1( self, function ):
        return [function]
    def functionList_2( self, functionList,function ):
        functionList.append(function)
        return functionList



F = mySALImplementation(open("test.sis"))

mysal.parse(F)
print "# done",time.ctime()

