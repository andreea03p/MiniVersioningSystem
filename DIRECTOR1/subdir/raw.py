import operator
print()

# ex 1
def valori(x, y):
    if x==y:
        return True
    else:
        return False

def verify(a, b, c):
    if valori(a,b)==True and valori(b, c)==True:
        print("toate elementele sunt egale")
    elif valori(a,b)==False and valori(b,c)==False and valori(a,c)==False:
        print("toate elementele sunt distincte")
    elif valori(a,b)==True:
        print("elem de pe poz 1 si 2 sunt egale")
    elif valori(b,c)==True:
        print("elem de pe poz 2 si 3 sunt egale")
    elif valori(a,c)==True:
        print("elem de pe poz 1 si 3 sunt egale")

verify(4, 6, 4)


# ex 3
# a)
def f(x):
    return x+2
def g(x):
    return x*2

def comp(f, g):
    return lambda x: f(x) + g(x)

print(comp(f,g)(3))

# b)
def generalf(op, f, g):
    return lambda x: op(f(x), g(x))

print(generalf(operator.mul, f, g)(3))


# ex 4
print((lambda x: x+15)(4))
