def WrapperInit(config: {}) -> int:
    return 0


def WrapperFini() -> int:
    return 0


def WrapperOnceExec(usrTag: str, params: [], datas: [], psrIds: [], psrCnt: int) -> []:
    return 0


def WrapperCreate(usrTag: str, params: [], psrIds: [], psrCnt: int) -> str:
    return ""


def WrapperWrite(handle: str, datas: []) -> int:
    return 0


def WrapperRead(handle: str) -> []:
    return []


def WrapperDestroy(handle: str) -> int:
    return 0
