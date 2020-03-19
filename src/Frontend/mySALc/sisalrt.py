import sys,time,string,math

# -----------------------------------------------
# Intrinsic ops
# -----------------------------------------------
def _pow_of_integer_integer(x,power):
    # Pingala's algorithm for integer powers
    assert power >= 0
    if power == 0:
        return 1
    elif power % 2 == 1:
        return x * _pow_of_integer_integer(x,power-1)
    else:
        t = _pow_of_integer_integer(x,power/2)
        return t*t

def _pow_of_integer_double(x,power):
    return math.pow(x,power)

def _pow_of_double_double(x,power):
    return math.pow(x,power)

def _pow_of_double_integer(x,power):
    return math.pow(x,power)

# -----------------------------------------------
# BOOLEAN
# -----------------------------------------------
class boolean:
    def __init__(self,flag=0):
	self.flag	= int(flag)
	return

    def __int__(self):
        return self.flag

    def __repr__(self):
        if self.flag:
            return 'T'
        else:
            return 'F'
    __str__ = __repr__

def _inputboolean():
    global tokens
    assert tokens
    t = tokens[0]
    del tokens[0]

    if t == 'T' or t == 't' or t == '1':
        return boolean(1)
    elif t == 'F' or t == 'f' or t == '0':
        return boolean(0)
    else:
        raise ValueError,'%s is not an boolean'%t

def _outputboolean(x):
    print boolean(x)

def _peek_of_boolean(x):
    print boolean(x)
    return x

def _peek_of_boolean_string(x,fmt):
    print fmt%boolean(x)
    return x

# -----------------------------------------------
# INTEGER
# -----------------------------------------------
def _inputinteger():
    global tokens
    assert tokens
    t = tokens[0]
    del tokens[0]
    try:
        return string.atoi(t)
    except ValueError:
        raise ValueError,'%s is not an integer'%t

def _outputinteger(x):
    print x

def _peek_of_integer(x):
    print x
    return x

def _peek_of_integer_string(x,fmt):
    print fmt%x
    return x

def _integer_of_boolean(x):
    if x:
        return 1
    else:
        return 0
def _integer_of_integer(x):
    return x
def _integer_of_double(x):
    return int(x)
def _integer_of_string(x):
    return string.atoi(x)

# -----------------------------------------------
# DOUBLE
# -----------------------------------------------
def _outputdouble(x):
    print x

def _peek_of_double(x):
    print x
    return x
def _peek_of_double_string(x,fmt):
    print fmt%x
    return x

def _double_of_integer(x):
    return float(x)
def _double_of_double(x):
    return x
def _double_of_string(x):
    return string.atof(x)

# -----------------------------------------------
# STRING
# -----------------------------------------------
def _outputstring(x):
    print repr(x)
    
def _peek_of_string(x):
    print x
    return x

def _peek_of_string(x,fmt):
    print fmt%x
    return x

def _string_of_integer(x):
    return str(x)
def _string_of_double(x):
    return str(x)
def _string_of_string(x):
    return x

# -----------------------------------------------
# RANGE
# -----------------------------------------------
class Range:
    def __init__(self,length,count,dims):
	self.length	= length
	self.count	= count
	self.dims	= dims
	return

    def shape(self):
        raise RuntimeError

class _range(Range):
    def __init__(self,low=0,high=0,step=1):
	self.low	= low
	self.high	= high
	self.step	= step
        
        self.range      = xrange(self.low,self.high+step,self.step)

        Range.__init__(self,len(self.range),1,1)
	return

    def __str__(self):
        return '<R %d:%d:%d>'%(self.low,self.high,self.step)
    
    def index(self,pos,i):
        #print '%s.index(%d,%d)'%(self,pos,i),self.range[i]
        return self.range[i]

    def shape(self):
        return (self.length,)

class _dot(Range):
    def __init__(self,A,B):
        assert A.length == B.length
        assert B.count == 1

	self.A	= A
	self.B	= B

        Range.__init__(self,A.length,A.count+B.count,1)

        return

    def __str__(self):
        return '<%s dot %s>'%(self.A,self.B)

    def index(self,pos,i):
        if pos < self.A.count:
            #print 'DOT for pos',pos,i,'select',self.A
            return self.A.index(pos,i)
        else:
            #print 'DOT for pos',pos,i,'select',self.B
            return self.B.index(0,i)

    def shape(self):
        return (self.length,)


class _cross(Range):
    def __init__(self,A,B):
	self.A	= A
	self.B	= B

        Range.__init__(self,A.length*B.length ,A.count+B.count, A.dims+B.dims)
        return
    
    def __str__(self):
        return '<%s cross %s>'%(self.A,self.B)

    def index(self,pos,i):
        if pos < self.A.count:
            #print 'CROSS for pos',pos,i,'select',self.A
            return self.A.index(pos,i/self.B.length)
        else:
            #print 'CROSS for pos',pos,i,'select',self.B
            return self.B.index(0,i%self.B.length)

    def shape(self):
        return self.A.shape() + self.B.shape()


# -----------------------------------------------
# MULTIPLE
# -----------------------------------------------
class Multiple:
    base = None
    def __init__(self,range):
	self.range	= range
        self.values	= [None]*range.length
	return

    def set(self,position,value):
        assert position < len(self.values)
        assert self.values[position] == None
        self.values[position] = value

    def __str__(self):
        return str(self.values)
        
class _multiple_of_boolean(Multiple):
    base = 'boolean'
class _multiple_of_integer(Multiple):
    base = 'integer'
class _multiple_of_double(Multiple):
    base = 'double'
class _multiple_of_string(Multiple):
    base = 'string'

# -----------------------------------------------
# ARRAY
# -----------------------------------------------
class Array:
    base = None
    def __init__(self,shape,multiple):
        self.shape	= shape
	self.values	= multiple.values
	return

    def __str__(self):
        return str(self.values)

    def index(self,i):
        assert i >=1 and i <= len(self.values)
        return self.values[i-1]

class _array_of_integer(Array):
    base = 'integer'
def _outputinteger_array(x):
    print x
def _peek_of_integer_array(x):
    print x
def _peek_of_integer_array_string(x,fmt):
    print fmt%x

class _array_of_double(Array):
    base = 'double'
def _outputdouble_array(x):
    print x
def _peek_of_double_array(x):
    print x
def _peek_of_double_array_string(x,fmt):
    print fmt%x
    
# -----------------------------------------------
# SYSTEM    
# -----------------------------------------------
def _start():
    global t0,t1,t2,t3,tokens
    t0 = time.time()
    tokens = sys.stdin.read().split()
    
def _begin():
    global t0,t1,t2,t3
    t1 = time.time()
def _end():
    global t0,t1,t2,t3
    t2 = time.time()
def _finish():
    global t0,t1,t2,t3
    t3 = time.time()
    print 'Time for input :',t1-t0
    print 'Time for work  :',t2-t1
    print 'Time for output:',t3-t2
    print 'Total          :',t3-t0
    
    
