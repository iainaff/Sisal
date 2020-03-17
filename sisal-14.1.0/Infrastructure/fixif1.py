test = """M "foobar"
  T 1	1 0
  T 2	1 1
  T 3	1 2
  T 4	1 3
  T 5	1 4
  T 6	1 5
  T 7	1 6
  { Type 8	8
    T 4	1 3
  }
  { Type 9	8
  }
  T 10	3 0	8
  C$ "C Faked IF1CHECK"
  C$ "D Nodes are DFOrdered"
  C$ "F OpenSource Frontend Version 0.1"
  X 10 "three"
    L 0 1 4 "3"
"""

def atoi(s):
    try:
        return string.atoi(s)
    except:
        return 0

print
print test

import string
Lines = string.split(test,'\n')
Lines = filter(None,Lines)
Lines = map(string.strip,Lines)

# -----------------------------------------------
# Convert module line into a comment
# -----------------------------------------------
print 'C',Lines[0]
Lines = Lines[1:]

# -----------------------------------------------
# Grab all the type Lines until the first stamp
# -----------------------------------------------
TLines = []
while Lines:
    if Lines[0][0:1] == 'C': break
    TLines.append(Lines[0])
    del Lines[0]

# -----------------------------------------------
# Process the type lines (dealing with chains)
# -----------------------------------------------
biggestIndex = 0
for T in TLines:
    biggestIndex = max(biggestIndex,max(map(atoi,string.split(T))))
while TLines:
    T = TLines[0]
    del TLines[0]
    if T[0] == 'T':
        print T
    else:
        parse = map(atoi,string.split(T))
        label = parse[2]
        kind = parse[3]
        links = []
        while TLines:
            TT = TLines[0]
            del TLines[0]
            if TT[0] == '}': break
            parse = map(atoi,string.split(TT))
            links.append(parse[1])
        if not links:
            print 'C',T
        else:
            pairs = []
            for i in range(0,len(links)):
                if i < len(links)-1:
                    biggestIndex = biggestIndex+1
                    link = biggestIndex
                else:
                    link = 0
                pairs.append([label,links[i],link])
                label = link
            for label,item,next in pairs:
                print 'T',label,kind,item,next

for L in Lines:
    if L[0:2] == 'C$':
        x = string.split(L)
        x[1] = x[1][1]+' "'
        L = string.join(x)
    print L
