
def x (a,b,c)
{
    std.println(a,b,c)
}

x(1,2)
x(1,2,3)
x(1,2,3,4)

 
def a()
{
    return 1
}
def b()
{
    return 2
}
def c()
{
    return a()+b()
}

std.println(c())


function fib (n)
{
    
    if n <= 1 { return 1 }
    return fib(n-1)+fib(n-2)
}

std.println("fib:\t",fib(4))



n = 8008

def r
{
    n = 0
    def l
    {
        super n
        n += 1
        std.println(n)
        if n<5 { l() }
    }
    l()
}

r()
std.println(n)

def m { return { "array" : [ "start[",1,2,3,4,"]end" ] } }

for v of m().array
{
    std.println(v)
}
