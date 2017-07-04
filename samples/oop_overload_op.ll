using(io,os)

class cc
{
    var r = [ 2, 3, 4 ]
}

n = cc()
n.r[0]=12
println(n.r[0])

n = cc()
println(n.r[0])


class vec2
{
    var x,y
    
    def vec2(x,y)
    {
        this.x = to_float(x) or 0.
        this.y = to_float(y) or 0.
    }
    
    operator + (value)
    {
        if type_of(value) == "object"
            return vec2(this.x+value.x,this.y+value.y)
        else
            return vec2(this.x+to_float(value),this.y+to_float(value))
    }
    
    operator - (value)
    {
        if type_of(value) == "object"
            return vec2(this.x-value.x,this.y-value.y)
        else
            return vec2(this.x-to_float(value),this.y-to_float(value))
    }
    
    operator -
    {
        return vec2(-this.x,-this.y)
    }
    
    operator * (value)
    {
        if type_of(value) == "object"
            return vec2(this.x*value.x,this.y*value.y)
        else
            return vec2(this.x*to_float(value),this.y*to_float(value))
    }
    
    operator / (value)
    {
        if type_of(value) == "object"
            return vec2(this.x/value.x,this.y/value.y)
        else
            return vec2(this.x/to_float(value),this.y/to_float(value))
    }
    
    operator % (value)
    {
        if type_of(value) == "object"
            return vec2(this.x%value.x,this.y%value.y)
        else
            return vec2(this.x%to_float(value),this.y%to_float(value))
    }
    
    operator right * (value)
    {
        return vec2(to_float(value) * this.x,to_float(value) * this.y)
    }
    
    
    def print()
    {
        io.println("(",this.x, ", ", this.y,")")
    }
}

x = ( ( vec2(3,4) + vec2(3,4) - vec2(3,8) ) * vec2(4,1) / vec2(2,1) ) % vec2(4,1)
y = vec2(4,1) % 4
z = 3 * vec2(3,2)
w = -vec2(3,2)
x.print()
y.print()
z.print()
w.print()

class vector
{
    var data = []
    
    def vector(...args) this.data = args
    
    def get(i) return this.data[i]
    
    def set(i,value) this.data[i] = value
    
    operator + (value)
    {
        o = vector()
        //copy data
        o.data = copy(this.data)
        //add...
        if ( type_of(value)    == "object" &&
             class_name(value) == "vector" )
        {
            for arg_v of value.data
                o.data.push(arg_v)
        }
        else
        {
            o.data.push(value)
        }
        return o
    }
    
    def forall(call)
    {
        for (k in this.data) call(k,this.data[k])
    }
}

x = vector( 2, 3, 4 )
x+= vector(-5,-6,-7 )

x.forall(def (x,y) println("key: ",x+1,"\t value: ",y))

println(class_name(x))
println(class_name(w))

class protected_test
{
protected:
    def pro()
    {
        io.println("call from protected")
    }
}

class private_test : protected_test
{
public:
    
    def pub()
    {
        io.println("call from public n")
        this.pro()
        this.pri()
    }
    
private:
    def pri()
    {
        io.println("call from private")
    }
}

p = private_test()
p.pub()

