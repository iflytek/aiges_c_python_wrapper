from wrapper_interface import *


def add(a,b):
    print(a+b)

class WrapperImpl(WrapperInterface):
    def Initialize(self):
        print("Initialize")
        pass

    def preprocess(self, data):
        pass

    def inference(self, data, *args, **kwargs):
        pass

    def postprocess(self, data):
        pass

    def Handle(self, data, context):
        print("handle")
        return add(data[0]+data[1])

    def Finialize(self):
        print("Finialize")
        pass
