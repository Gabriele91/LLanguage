using("io")

function fun(x,y)
{
    return x*y
}

y = 1
x = eval("return fun(y,9)")
println(x)
println(":)")

x = def fib ( n )
{
    if n <= 1 { return 1 }
    return fib(n-1)+fib(n-2)
} (5)


println(x,"\t",type_of(x))

println(null, " ",true, " ",false)

l =  def { return [9]  } ( )[0] + 1
println(l)

val = "cal"
println(type_of(true && val == "val"))

def c
{
    return [1,2,3]
}

l = c()[0]
println(l)


s = (def  { return [2,3,4] }) () [0]  + 1
println(s)

r = 1
n = 2
c = def (x) { return x*(r+n) }
println(c(4))


s = 0
def spp
{
    super s += 1
}
spp()
spp()
println(s)

def some_objects ()
{
    this =
    {
        "name" : "none",
        "print_name" : def
        {
            println(to_string(this.name))
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



