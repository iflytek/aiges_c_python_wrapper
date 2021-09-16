from pyImpl import *
import numpy as np
def wrapperInit():
    print("hello init")
    return 0

def wrapperOnceExec():
    print("hello exec")
    print(Kernel())
    print(np.random.randint(2, size=10))
    return 0

def wrapperFini():
    print("hello fini")
    return 0