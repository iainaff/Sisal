def processClass(name,klass):
    import string
    isPublic = 0
    interfaces = []
    for L in klass:
        S = string.split(L)

        # Skip some stuff
        if not L: continue
        if L == 'public:':
            isPublic = 1
            continue
        if L == 'private:':
            isPublic = 0
            continue
        if L == 'protected:':
            isPublic = 0
            continue
        if S[0] == '//': continue

        if S[0] == 'typedef': continue

        # Strip off virtual and static stuff
        if S[0] == 'virtual':
            L = L[8:]
        if S[0] == 'static':
            L = L[7:]

        # Don't implement pure virtuals
        if S and S[-1] == '0;': continue

        # Convert to :: form
        key = ''
        for token in string.split(L):
            if key: key = key+' '
            if '(' not in token:
                key = key + token
            else:
                token = name+'::'+token
                key = key + token

        if isPublic:
            interfaces.append(key)
    return interfaces

class hh:
    def __init__(self,file):
        import os,string
        name,ext = os.path.splitext(file)
        self.name = name

        lines = open(file).readlines()
        classDef = []
        target = 'class ' + name
        for L in lines:
            L = string.strip(L)
            if  classDef or L[:len(target)] == target:
                classDef.append(L)
            if L[0:1] == '}': break

        self.interfaces = processClass(name,classDef)

    def skeleton(self,interfaces=None):
        s = ''        
        if interfaces == None: interfaces = self.interfaces
        for i in interfaces:
            s = s + '\n'
            s = s + '// ' + i + '\n'
            s = s + i[:-1] + '\n'
            s = s + '{\n}\n'
        return s

    def checkCC(self):
        import string
        try:
            lines = open(self.name+'.cc').readlines()
        except:
            lines = []
        # Search for the interface comments
        interfaces = self.interfaces[:]
        for L in lines:
            if string.strip(L) == '// '+interfaces[0]:
                del interfaces[0]
                if not interfaces: break
        if interfaces:
            for i in interfaces:
                print '// '+i
            raise RuntimeError

if __name__ == '__main__':
    import sys
    try:
        file = sys.argv[1]
    except:
        file = 'Stamp.hh'
    H = hh(file)
    H.checkCC()
