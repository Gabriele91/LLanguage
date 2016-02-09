
a = [1,2,3,"hello",2.2]

std.println("-----------------------")

for v in a
{
    std.println(v)
}

std.println("-----------------------")

for v of a
{
    std.println(v)
}

std.println("-----------------------")
std.println("-----------------------")


a = { "age" : 1,
      "what?" : 2,
      "ah!": 3,
      "how?" : "hello",
      "foo" : 2.2 }

for v in a
{
    std.println(v)
}

std.println("-----------------------")

for v of a
{
    std.println(v)
}

std.println("-----------------------")
