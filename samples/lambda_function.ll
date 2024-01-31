


function s(r)
{
    c = r
    function n
    {
        super c,x,z
        c += 1
        return c
    }

    return n
}

z1 = s(2)
z2 = s(100)

io.println(z1())
io.println(z2())

io.println(z1())
io.println(z1())
io.println(z1())

z1 = s(-3)
io.println(z1())
io.println(z1())
io.println(z1())

io.println(z2())