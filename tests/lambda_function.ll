


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

std.println(z1())
std.println(z2())

std.println(z1())
std.println(z1())
std.println(z1())

z1 = s(-3)
std.println(z1())
std.println(z1())
std.println(z1())

std.println(z2())