using(io,os)

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


