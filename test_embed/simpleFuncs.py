from embeded import Vehiclee

def simplePrint(argument):
    print(argument.id)
    argument.id=44444
    print(Vehiclee)
    a = Vehiclee()
    a.id=431111
    print(dir(a))
    return a

