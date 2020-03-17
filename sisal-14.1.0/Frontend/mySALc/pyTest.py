import mysal,mysalparser
import string,sys,time
#sys.stdout = sys.stderr


##################################################################
#                      CLASS FUNCTIONRECORD                      #
##################################################################
class FunctionRecord:
    def __init__(self,id,types,defined=0,module=''):
	self.id		= id
	self.types	= types
	self.defined	= defined
        self.module     = module
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
        return '<sym %s : %s>'%(self.name,self.type)


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

    def __repr__(self):
        return '<ST %s>'%self.__symbols
        

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
            last = self.__stack[-1]
            del self.__stack[-1]
        except:
            last = None
        return last

    def __getattr__(self,a):
        if self.__stack:
            return getattr(self.__stack[-1],a)
        raise AttributeError,a

    def lookup(self,name):
        for i in range(1,len(self.__stack)+1):
            try: return self.__stack[-i].lookup(name)
            except: pass
        raise KeyError,name

    def __repr__(self):
        return '<STS %s>'%self.__stack

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
        from types import ListType
        token.__init__(self)
        self.code	= code
        self.values	= values
        assert type(values) == ListType
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
    __str__ = __repr__
    def __getitem__(self,i):
        return self.values[i]


##################################################################
#                   CLASS MYSALIMPLEMENTATION                    #
##################################################################
class mySALImplementation(mysalparser.mySALParser):
    # Class static
    token = token
    expr = expr
    __boolean = token('boolean','system',0,0,0,0)
    __integer = token('integer','system',0,0,0,0)
    __double = token('double','system',0,0,0,0)
    __string = token('string','system',0,0,0,0)

    __boolean_array = token('boolean_array','system',0,0,0,0)
    __integer_array = token('integer_array','system',0,0,0,0)
    __double_array = token('double_array','system',0,0,0,0)
    __string_array = token('string_array','system',0,0,0,0)

    __boolean_matrix = token('boolean_matrix','system',0,0,0,0)
    __integer_matrix = token('integer_matrix','system',0,0,0,0)
    __double_matrix = token('double_matrix','system',0,0,0,0)
    __string_matrix = token('string_matrix','system',0,0,0,0)

    __boolean_slab = token('boolean_slab','system',0,0,0,0)
    __integer_slab = token('integer_slab','system',0,0,0,0)
    __double_slab = token('double_slab','system',0,0,0,0)
    __string_slab = token('string_slab','system',0,0,0,0)

    __zero = token('0','system',0,0,0,0)

    def __init__(self,file):
        self.__file = file
        self.__symbols = SymbolTableStack()
        self.__known = {}
        self.__functions = {}
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
        while 1:
            i += 1
            name = '%s_%d'%(base,i)
            if name not in keys: break
        self.__known[name] = base
        return name

    def temporary(self,dummy=0):
        name = self.unique('_t')
        self.setName(name,name)
        expr = self.lookupName(name)
        return expr

    def integerTemporary(self,dummy=0):
        expr = self.temporary(dummy)
        expr.setType(self.__integer)
        return expr

    def uniqueTemporaries(self,n):
        return map(self.temporary, range(n))

    # -----------------------------------------------
    # This is the standard typer for arithmetic ops
    # -----------------------------------------------
    def type_arithmetic(self,op,*args):
        assert args
        types = tuple(map(lambda x: str(x.value().type), args))

        ok = { ('integer','integer') : 'integer',
               ('integer','double') : 'double',
               ('double','double') : 'double',
               ('double','integer') : 'double',
               
               ('integer',) : 'integer',
               ('double',) : 'double',
               }
        try:
            result = ok[types]
        except:
            self.error(op,msg='invalid operand types for %s, %s'%(op,str(types)))
            result = args[0].value().type
        return result

    # -----------------------------------------------
    # Unary not
    # -----------------------------------------------
    def type_not(self,op,B):
        if str(B.value.type) != 'boolean':
            self.error(op,msg='Expected boolean type, have %s'%str(B.value.type))
        return self.__boolean
    
    # -----------------------------------------------
    # Arithmetics and string catenation
    # -----------------------------------------------
    def type_string_or_arithmetic(self,op,A,B):
        typeA = str(A.value().type)
        typeB = str(B.value().type)
        if typeA == 'string' and typeB == 'string':
            return 'string'
        else:
            return self.type_arithmetic(op,A,B)
    
    def type_equals(self,op,A,B):
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

    def type_compare(self,op,A,B):
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
        try:
            sys.stderr.write( '\n%s:%d: Error, %s\n'%(x.fileName,x.startLine,msg) )
        except:
            sys.stderr.write( '\n??:??: Error, %s\n'%(msg) )
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
        return self.__symbols.close()

    def lookupName(self,name):
        return self.__symbols.lookup(name)
            
    def setName(self,name,unique):
        self.__symbols.new(name=name,unique=unique)

    # -----------------------------------------------
    # Id lists map names to ID tokens
    # -----------------------------------------------
    def idList_1(self,id_):
        # Enter value into scope
        try:
            u = self.unique(id_)
            self.setName(id_,u)
        except ValueError:
            self.error(id_,msg="%s already defined in this scope"%id_)
        return [str(id_)]

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
    # Follow parens
    # -----------------------------------------------
    def paren( self, lparen,expression,rparen ):
        expression.startFrom(lparen)
        expression.endFrom(rparen)
        return expression

    # -----------------------------------------------
    # Function call
    # -----------------------------------------------
    intrinsicFunctions = {
        '_peek_of_boolean' : [__boolean],
        '_peek_of_boolean_string' : [__boolean],

        '_peek_of_integer' : [__integer],
        '_peek_of_integer_string' : [__integer],

        '_peek_of_double' : [__double],
        '_peek_of_double_string' : [__double],

        '_peek_of_string' : [__string],
        '_peek_of_string_string' : [__string],

        '_double_of_integer' : [__double],
        '_double_of_double' : [__double],
        '_double_of_string' : [__double],

        '_integer_of_integer' : [__integer],
        '_integer_of_double' : [__integer],
        '_integer_of_string' : [__integer],

        '_string_of_integer' : [__string],
        '_string_of_double' : [__string],
        '_string_of_string' : [__string],

        }
    
    def functionCall( self, id,lparen,expression,rparen ):
        # Generate the mangled name for lookup
        name = self.mangle(id,expression.types())

        # It should be defined by now...
        if self.intrinsicFunctions.has_key(name):
            if not self.__functions.has_key(name):
                self.__functions[name] = FunctionRecord(id, self.intrinsicFunctions[name], module='sisalrt.')
        elif self.__functions.has_key(name):
            pass
        else:
            self.__functions[name] = FunctionRecord(id, [self.__integer] ) # fake signature and body count
            self.error(id,msg="Undefined function %s%s"%(id,map(str,expression.types())))

        # We need to hold these results
        results = self.uniqueTemporaries(len(self.__functions[name].types))
        lhs = string.join(map(str,results),',')

        # Set return types
        for result,type in map(None, results,self.__functions[name].types):
            result.setType(type)

        # Do the work for the current expression
        value = self.expr(expression.code,results,start=id,end=rparen)
        value.append('%s = %s%s(%s)'%(lhs,self.__functions[name].module,name,expression.rhs()))

        return value
    
    def optExpression_0( self ):
        return self.expr()


    def dollarCall(self, dollar, id):
        return self.functionCall(id,dollar,self.expr(),dollar)

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

    def stringConst(self,stringLiteral):
        return self.setConst(stringLiteral,self.__string)

    # -----------------------------------------------
    # Arithmetics just use either inline or special
    # name functions
    # -----------------------------------------------
    intrinsicOperations = {
        'mod': ('modulus', type_arithmetic),
        'div': ('divide', type_arithmetic),
        '+': ('+', type_string_or_arithmetic),
        '-': ('-', type_arithmetic),
        '*': ('*', type_arithmetic),
        '/': ('/', type_arithmetic),
        '^': ('pow', type_arithmetic),
        '==': ('==', type_equals),
        '<': ('<', type_compare),
        '<=': ('<=', type_compare),
        '>': ('>', type_compare),
        '>=': ('>=', type_compare),
        'not': ('not', type_not),
        }

    def infix_3(self, A, op, B):
        # These work only on unary expressions
        if len(A) != 1:
            self.error(A,msg="Incorrect arity of left operand, %d"%len(A))
        if len(B) != 1:
            self.error(B,msg="Incorrect arity of right operand, %d"%len(B))
        
        # The code I want is t = A op B or maybe t = op(A,B)
        function, typer = self.intrinsicOperations[str(op)]
        name = self.mangle(function,[A.value().type,B.value().type])
        if str(op) == function:
            work = '(%s %s %s)'%(A.value(),op,B.value())
        else:
            work = 'sisalrt.%s(%s,%s)'%(name,A.value(),B.value())

        # Get a temporary and set its type
        type = typer(self, op,A,B)
        t = self.temporary()
        t.setType(type)

        # Build an expression
        result = self.expr('',[t],start=A,end=B)
        result.append(A.code)
        result.append(B.code)
        result.append("%s = %s"%(t, work))
        return result

    def prefix_2(self, op, B):
        # These work only on unary expressions
        if len(B) != 1:
            self.error(B,msg="Incorrect arity of unary operand, %d"%len(B))

        # The code I want is t = op B or maybe t = op(B)
        function, typer = self.intrinsicOperations[str(op)]
        name = self.mangle(function,[B.value().type])
        if str(op) == function:
            work = '(%s %s)'%(op,B.value())
        else:
            work = 'sisalrt.%s(%s)'%(name,B.value())

        # Get a temporary and set its type
        type = typer(self, op,B)
        t = self.temporary()
        t.setType(type)

        # Build an expression
        result = self.expr('',[t],start=op,end=B)
        result.append(B.code)
        result.append("%s = %s"%(t, work))
        return result

    # -----------------------------------------------
    # Array indices are pretty simple
    # -----------------------------------------------
    def arrayIndex_4(self,array,lbracket,expression,rbracket):
        # Just simple arrays please...
        if len(array) > 1:
            self.error(array,msg="Expected unary array value, not arity %d expression"%len(array))
            del array.values[1:]
            
        # Make sure the expression is all integer
        types = map(str,expression.types())
        for i in range(0,len(types)):
            if types[i] != 'integer':
                self.error(lbracket,msg='Index #%d is %s, not integer'%(i+1,types[i]))


        # Get a temporary and set its type
        base = str(array.value().type).split('_')[0]
        t = self.temporary()
        t.setType(base)

        # Build an expression
        result = self.expr('%s = (%s).index(%s)'%(t,array.rhs(),expression.rhs()),[t],start=array,end=rbracket)
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
        # Convert names into local names
        syms = map(lambda x,self=self: self.lookupName(x), idList)

        # Sanity check for length
        if len(syms) != len(expression):
            self.error(equals,msg="LHS has arity %d, RHS has arity %d"%(len(syms),len(expression)))
            # Expand expression as needed
            while len(expression) < len(syms):
                expression.append(self.integerConst(self.__zero))
            while len(syms) < len(expression):
                syms.append(self.temporary())

        # Match types with names
        names = map(lambda x: x.unique, syms)
        for sym,type in map(None, syms,expression.types()):
            sym.setType(type)
        code = expression.code+'\n%s = %s'%(string.join(names,','),expression.rhs())
        result = self.expr(code,idList,clone=equals)
        return result

    # -----------------------------------------------
    # Let expressions use name bindings
    # -----------------------------------------------
    def letExpr( self, let_,openScope,nameList,in_,expression,closeScope,end_,let__ ):
        assert isinstance(expression,self.expr),expression
        expr = self.expr('',expression.values,start=let_,end=let__)
        expr.append(nameList.code)
        expr.append(expression.code)
        return expr

    # -----------------------------------------------
    # Range expressions
    # -----------------------------------------------
    def rangeClause( self, id,in_,expression ):
        # I only understand low/high and low/high/step
        if len(expression) != 2 and len(expression) != 3:
            self.error(expression,msg="Range driver not of form low/high or low/high/step with arity %d"%len(expression))
        if len(expression) < 2:
            expression.append(self.integerConst(self.__zero))
        if len(expression) > 3:
            del expression.values[3:]
            
        # Make sure all sub expressions are integers
        for subexpr in expression:
            if str(subexpr.type) != "integer":
                self.error(expression,msg="Expecting all integer range driver, not %s"%subexpr.type)

        # Build a range value
        t = self.integerTemporary()
        e = self.expr(expression.code+"\n%s = sisalrt._range(%s)"%(t,expression.rhs()),[t],start=id,end=expression)
        #e.append('print "%s.shape",%s.shape()'%(t,t))
        
        id = str(id)
        u = self.unique(id)
        self.setName(id,u)
        sym = self.lookupName(id)
        sym.setType(self.__integer)
        return ([u],e)

    def rangeExpr_1( self, rangeClause ):
        return rangeClause

    def rangeExpr_3( self, arg1,op,arg2 ):
        id1, expr1 = arg1; v1 = expr1.value()
        id2, expr2 = arg2; v2 = expr2.value()

        # Build a range value
        t = self.integerTemporary()
        result = self.expr('',[t],clone=op)
        result.append(expr1.code)
        result.append(expr2.code)
        result.append('%s = sisalrt._%s(%s,%s)'%(t,op,v1,v2))
        #result.append('print "%s.shape",%s.shape()'%(t,t))

        return (id1+id2,result)

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
    # Built in reduction funtions
    # -----------------------------------------------
    reductions = {
        '_array_of_boolean' : __boolean_array,
        '_array_of_integer' : __integer_array,
        '_array_of_double' : __double_array,
        '_array_of_string' : __string_array,
        }
        
    # -----------------------------------------------
    # Forall loops
    # -----------------------------------------------
    def forExpr( self, for_,openScope,rangeExpr,optNameList,returns_,arrayReturns,end_,for__,closeScope ):
        # -----------------------------------------------
        # Build the prelude
        # -----------------------------------------------
        rangeIDs, rangeCode = rangeExpr
        code = rangeCode.code

        # Lay out prelude code for returns
        # We need prelude code for all the values gathered
        syms = []
        for kind,work in arrayReturns:
            t = self.temporary()
            syms.append(t)
            t.setType(work.value().type)
            code += '\n%s = sisalrt._multiple_of_%s(%s)'%(t,work.typeString(),rangeCode.rhs())

        # Add in the driver for the loop
        ii = self.integerTemporary()
        code += '\nfor %s in xrange(%s.length):'%(ii,rangeCode.rhs())

        # -----------------------------------------------
        # Build the loop body
        # -----------------------------------------------
        body = self.expr()
        
        # calculate values for ranges
        for i in range(0,len(rangeIDs)):
            rangeID = rangeIDs[i]
            body.append('%s = %s.index(%d,%s)'%(rangeID,rangeCode.rhs(),i,ii))

        # Do the work for the body
        body.append(optNameList.code)

        # Gather up multiples
        for i in range(len(arrayReturns)):
            kind,work = arrayReturns[i]
            sym = syms[i]
            body.append(work.code)
            body.append('%s.set(%s,%s)'%(sym,ii,work.rhs()))
        
        body.indent()
        code += '\n'
        code += body.code

        # -----------------------------------------------
        # Apply the reductions
        # -----------------------------------------------
        # Gather up multiples
        results = self.uniqueTemporaries(len(arrayReturns))
        for i in range(len(arrayReturns)):
            kind,work = arrayReturns[i]
            name = self.mangle(kind,[work.value().type])
            if self.reductions.has_key(name):
                results[i].setType(self.reductions[name])
            else:
                results[i].setType(self.__integer)
                self.error(work,msg='Invalid reduction for type %s'%work.value().type)
            code += '\n%s = sisalrt.%s(%s,%s)'%(results[i],name,rangeCode.rhs(),syms[i])
        
        expr = self.expr(code,results,start=for_,end=for__)
        return expr

    # -----------------------------------------------
    # Expressions just join singletons
    # -----------------------------------------------
    def expression_3(self, expression, comma, singleton):
        assert isinstance(expression,self.expr),expression
        assert isinstance(singleton,self.expr),singleton

        expression.append(singleton)
        expression.endFrom(singleton)
        return expression

    # -----------------------------------------------
    # Functions bind arguments to an expression
    # -----------------------------------------------
    def functionPrototype( self, function_,id,lparen,optParameters,returns_,typeList,rparen ):
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
        
        return id,typeList,inTypes,inNames,outTypes,name

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

    # -----------------------------------------------
    # Function with expression body
    # -----------------------------------------------
    def function_7( self, openScope,functionPrototype,expression,end_,function_,id,closeScope ):
        rawName, typeList, inTypes,inNames,outTypes,name = functionPrototype

        # Mark as having a body
        self.__functions[name].defined = 1

        # Make sure tail id matches
        if str(rawName) != str(id):
            self.error(id, msg="Tail id %s doesn't match function name %s"%(rawName,id))

        # Convert IDs into unique name
        names = map(lambda id,closeScope=closeScope: closeScope.lookup(id).unique, inNames)

        print '# -----------------------------------------------'
        print "def %s(%s):"%(name,string.join(names,','))
        expression.append('return %s # %s'%(expression.rhs(),string.join(map(str,expression.types()))))
        expression.indent()
        print expression.code
        print '# -----------------------------------------------'

        self.function_expression_check(end_, expression, typeList)
        return name,typeList

    # -----------------------------------------------
    # Forward function
    # -----------------------------------------------
    def function_4( self, openScope,functionPrototype,semi,closeScope ):
        rawName, typeList, inTypes,inNames,outTypes,name = functionPrototype
        return name,typeList

    # -----------------------------------------------
    # Forward function in module
    # -----------------------------------------------
    def function_6( self, openScope,functionPrototype,in_,id,semi,closeScope ):
        rawName, typeList, inTypes,inNames,outTypes,name = functionPrototype
        print 'import %s'%id
        if self.__functions[name].module and self.__functions[name].module != str(id):
            self.error(in_,msg="%s was already assigned to module %s"%(rawName,self.__functions[name].module))
        self.__functions[name].module=str(id)+'.'
        return name,typeList

    # -----------------------------------------------
    # Main
    # -----------------------------------------------
    def function_11( self, openScope,main_,lparen,optParameters,returns_,typeList,rparen,expression,end_,main__,closeScope ):
        rawName, typeList, inTypes,inNames,outTypes,name = self.functionPrototype("function",main_,lparen,optParameters,returns_,typeList,rparen)

        # Mark as having a body
        self.__functions[name].defined = 1
        
        # We build an expression to hold main
        main = self.expr('',expression.values)

        # We first need to grab all inputs
        main.append('sisalrt._start()')
        for i in range(0,len(inNames)):
            name = closeScope.lookup(inNames[i])
            main.append('%s = sisalrt._input%s()'%(name,inTypes[i]))

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

    def function_13( self, openScope,function_,id,lparen,optParameters,returns_,typeList,rparen,expression,end_,function__,id_,closeScope ):
        inTypes,inNames,outTypes,name = self.function_common(id,lparen,optParameters,returns_,typeList,rparen )

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

