using("io")

def some_objects ()
{
    return
    {
        "name" : null,
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

/*
//to do
class some_objects
{
    constructor()
    {
        this.name = null
    }
 
    destructor()
    {
        //none
    }
 
    def set_name( name )
    {
        this.name = name
    }
 
    def get_name
    {
        return this.name
    }
    
    def print_name
    {
        println(to_string(this.name))
    }
 }
 */