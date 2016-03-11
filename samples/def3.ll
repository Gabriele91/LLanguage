
x = def fib ( n )
{
    if n <= 1 { return 1 }
    return fib(n-1)+fib(n-2)
} (5)


std.println(x)

l =  def { return [9]  } ( )[0] + 1

std.println(l)

def c
{
    return [1,2,3]
}

l = c()[0]
std.println(l)


s = (def  { return [2,3,4] }) () [0]  + 1
std.println(s)

r = 1
n = 2
c = def (x) { return x*(r+n) }
std.println(c(4))
