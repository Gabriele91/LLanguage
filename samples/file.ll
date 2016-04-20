
file = io.file()
file.open("ciao.txt","w")
file.write("hello\n")
file.write("world\n")
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