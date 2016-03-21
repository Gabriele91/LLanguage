
x = def fib ( n )
{
    if n <= 1 { return 1 }
    return fib(n-1)+fib(n-2)
} (5)


std.println(x,"\t",std.type_of(x))

std.println(null, " ",true, " ",false)

l =  def { return [9]  } ( )[0] + 1
std.println(l)

val = "cal"
std.println(std.type_of(true && val == "val"))

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


s = 0
def spp
{
    super s += 1
}
spp()
spp()
std.println(s)

def some_objects ()
{
    this =
    {
        "name" : "none",
        "print_name" : def
        {
            std.println(std.to_string(this.name))
        },
        "set_name" : def ( name )
        {
            this.name = name
        },
        "get_name" : def
        {
            return this.name
        }
    }
    return this
}
x = some_objects()
x.set_name( "gabriele" )
x.print_name()



