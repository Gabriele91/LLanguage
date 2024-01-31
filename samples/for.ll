
a = [1,2,3,"hello",2.2]

io.println("-----------------------")

for v in a
{
    io.println(v)
}

io.println("-----------------------")

for v of a
{
    io.println(v)
}

io.println("-----------------------")
io.println("-----------------------")


a = { "age" : 1,
      "what?" : 2,
      "ah!": 3,
      "how?" : "hello",
      "foo" : 2.2 }

for v in a
{
    io.println(v)
}

io.println("-----------------------")

for v of a
{
    io.println(v)
}

io.println("-----------------------")
io.println("-----------------------")


for v of [2,3,"wooohhhh!?@#+*[]"]
{
    io.println(v)
}

io.println("-----------------------")