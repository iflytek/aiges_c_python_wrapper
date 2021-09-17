class WrapperInterface:
    def __init__(self):
        pass

    def Initialize(self):
        pass

    def preprocess(self, data):
        pass

    def inference(self, data, *args, **kwargs):
        pass

    def postprocess(self, data):
        pass

    def Handle(self, data, context):
        pass

    def Finialize(self):
        pass