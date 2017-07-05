using(io)

class User
{
	def print_name_and_sum()
	{
		println(this.name, " - ", this.sum)
	}
	def sum_inc()
	{
		this.sum += 1
	}
	
	operator + (right) //always public
	{
		u = User()
		u.name = this.name + right.name
		return u
    }

protected:
	
	var name = ""

private:

	var sum = 0
			
}

class Mario : User
{	
	def Mario()
	{
		this.User()
		this.name = "Mario"
	}
	def sum_to_0()
	{
		this.sum = 0 //<- error is private
	}
}

m = Mario()
m.print_name_and_sum()
m.sum_inc()
m.print_name_and_sum()
m = Mario() + m
m.print_name_and_sum()
//m.sum_to_0();
//println(m.name) //<- error is protected
