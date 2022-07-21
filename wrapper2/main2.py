import sys
from sdk import WrapperBase
class PythonClass:
    def __init__(self):
        self._x = 0

    def get_x(self):
        return self._x

    def set_x(self, x):
        self._x = x

