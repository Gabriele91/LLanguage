using("io")

def some_objects ()
{
    return
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
}
x = some_objects()
x.set_name( "gabriele" )
x.print_name()
