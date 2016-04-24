
file = io.file()
file.open("ciao.txt","w")
file.write("hello\n")
file.write("world")
file.close()


file = io.file()
file.open("ciao.txt","rb")
file.print()

file = io.file()
file.open("ciao.txt","rb")
str = file.read_all()
io.println(str)


file = io.file()
file.open("ciao.txt","rb")
for ( line = file.read_line() i = 1 ;
      file.is_open() ;
      line = file.read_line() i += 1 )
{
    io.println( i, " -> ", line )
}
file.close()

file = io.file()
file.open("ciao.txt","rb")
for ( char of file ) io.print("-"+char)
file.rewind()
io.println("\n-----------")
for ( key in file )
{
    if file.get_char()!="\n"
        io.print("("+key+")",file.get_char(),"\t")
    else
        io.print("("+key+")",file.get_char())
}

io.println("")

file.close()


io.rename("ciao.txt","ciao2.txt")
io.copy("ciao2.txt","ciao3.txt")
io.remove("ciao2.txt")

z = io.file()
z.open("ciao.txt","wb")
z.write("time: "+os.time()+"\n")
z.write("clock: "+os.clock()+"\n")
