using(io)

v = 0
if 0 
{
    for( n = 0; n < 100000000 ; n += 1 )
    {
        v += 1
    }
}
else 
{
    for n of xrange(100000000)
    {
        v+=1
    }
}

println(v)

