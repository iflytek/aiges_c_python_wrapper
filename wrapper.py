from wrapper_impl import WrapperImpl

inst = WrapperImpl()


def WrapperInit(config: {}) -> int:
    inst.Initialize()
    return 0


def WrapperFini() -> int:
    inst.Finialize()
    return 0


def WrapperOnceExec(usrTag: str, params: [], datas: [], psrIds: [], psrCnt: int) -> []:
    inst.Handle()
    return 0


def WrapperCreate(usrTag: str, params: [], psrIds: [], psrCnt: int) -> str:
    return ""


def WrapperWrite(handle: str, datas: []) -> int:
    return 0


def WrapperRead(handle: str) -> []:
    return []


def WrapperDestroy(handle: str) -> int:
    return 0
