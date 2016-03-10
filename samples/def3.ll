
x = def fib ( n )
{
    if n <= 1 { return 1 }
    return fib(n-1)+fib(n-2)
} (5)


std.println(x)

l =  def
     {
        return [9]
     }
     ()[0] + 1

std.println(l)

def c
{
    return [1,2,3]
}

l = c()[0]
std.println(l)
