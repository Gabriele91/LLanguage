
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