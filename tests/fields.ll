std.using("std")

c = {}
c.global = global
c["global"]().a = 1
print(c.global()["a"],"\n")

c["c"] = c
print(c["global"]().c,"\n")
print(c["global"]().c.c,"\n")
print(c["global"]()["c"]["c"].global(),"\n")

