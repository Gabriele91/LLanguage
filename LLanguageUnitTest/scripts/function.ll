
def pow2 (a)
{
    return a*a
}

def diff (a,b)
{
    return a-b
}

def fib (n)
{
    if n <= 1 { return 1 }
    return fib(n-1)+fib(n-2)
}


def super_test
{
    c = 1
    def n
    {
        super c
        c+=1
        return c
    }
    return n()+n()
}

def int_test
{
    return 1+1
}

def float_test
{
    return 1+1.
}

def string_test
{
    return 1+"->hello"
}

def array_test
{
    return [ 1.2, 3, "hello", "l", "language" ]
}

def for_of_test
{
    sum = 0
    for v of [1,2,3,4,5] { sum += v }
    return sum
}

def for_in_test
{
    sum = 0
    for k in [1,2,3,4,5] { sum += k }
    return sum
}

def for_range_1_rand(len)
{
    sum = 0
    for ( x of std.range(len) )
    {
        sum += x
    }
    return sum
}

def for_range_2_rand(start,len)
{
    sum = 0
    for x of std.range(start,len)
    {
        sum += x
    }
    return sum
}

def range_3(start,len, step)
{
    return std.range(start,len, step)
}

def for_c_like
{
    sum = 0
    for x = 1; x < 10; x+=1
    {
        sum += x
    }
    for ( x = 1; x < 10; x+=1 )
    {
        sum += x
    }
    return sum
}

def lambda_calc_1(n)
{
    x = def { return n }
    return x()
}

def lambda_calc_2(n,c,d)
{
    x = def (y) { return y*(c+d) }
    return x(n)
}

def lambda_calc_fib(n)
{
    return def fib (n) { if n <= 1 { return 1 }  return fib(n-1)+fib(n-2) } (n)
}