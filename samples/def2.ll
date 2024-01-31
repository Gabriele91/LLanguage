using(io);

def a
{
    io.println("ciao");
    return 0
}
println(a());
a=1+2;

def n
{
    n = 0
    def c
    {
        super n
        n+=1
        return n
    }
    return c
}
c1=n()
c2=n()
println(c1())
println(c2())
println(c2())
println(c2())
println(c1())