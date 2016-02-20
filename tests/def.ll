
def x (a,b,c)
{
    std.println(a,b,c)
}

x(1,2)
x(1,2,3)
x(1,2,3,4)


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

