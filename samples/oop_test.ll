
class basic_vector
{
	var n = 23

	def say_hello()
	{
		io.println("hello")
	}
}

class basic_to_string
{

	def string()
	{
		return "basic_to_string:"
	}
	
}

class basic_vector2 : basic_to_string 
{
	var n = 10
	var f = 8

	def basic_vector2(f)
	{
		this.f = f or this.f
	}
	
	def say_hello()
	{
		io.println("hello2")
	}
}

class vector : basic_vector2, basic_vector
{
	var x,y,z
	
	def vector( x, y, z)
	{
		this.x=x or 0
		this.y=y or 0
		this.z=z or 0
		this.basic_vector2(3)
	}
	
	def to_string()
	{
		return this. string()+"("+this.x+", "+this.y+", "+this.z+")"
	}
	
	def print()
	{
		io.println(this.to_string())
	}
}

n = vector(1,2,3)
n.print()
n.say_hello()
io.println(n.n)
io.println(n.f)



class human
{
    var name = ""
}


class men : human
{
    def men (name) this.name = name
    def get_name() return this.name
    def ref_name()
    {
        l = this
        return def () return l.name
    }
}

io.println(men("Julia").get_name())
nref = men("Julia").ref_name()
io.println(nref())


class callback
{
    var this_callback
    
    def set_callback(call)
        this.this_callback=call
}

class event : callback
{
    var last_event
    
    def event(x)
        this.last_event = x
    
    def send_event()
        this.this_callback(this.last_event)
}

c = event("hello")
c.set_callback(def (x) io.println(x) )
c.send_event()


class agent
{
    var name = "none"
    
    def update()
    {
        io.println("update("+this.name+")")
        this.action()
    }
}

class mario : agent
{
    var name = "mario"
    
    def action()
    {
        for v of ["hello","men"] io.println(v)
    }
}

class luigi : agent
{
    var name = "luigi"
    
    def action()
    {
        for v of ["lol","boy"] io.println(v)
    }
}

class manager
{
    var n_events = []
    
    def manager(...args)
    {
        for k in args
        {
            this.n_events[k] = args[k]
        }
    }
    
    def update()
        for e of this.n_events
            e.update()
}

m = manager(mario(),luigi())
m.update()

