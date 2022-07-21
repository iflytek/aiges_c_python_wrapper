import sys

from sdk import WrapperBase, \
            StringParamField, \
                ImageBodyField, \
                    StringBodyField



'''
定义请求类:
 params:  params 开头的属性代表最终HTTP协议中的功能参数parameters部分，
          params Field支持 StringParamField，
          NumberParamField，BooleanParamField,IntegerParamField，每个字段均支持枚举
          params 属性多用于协议中的控制字段，请求body字段不属于params范畴

 input:    input字段多用与请求数据段，即body部分，当前支持 ImageBodyField, StringBodyField, 和AudioBodyField
'''


class UserRequest(object):
    params1 = StringParamField(key="p1", enums=["3", "eee"], value='3')
    params2 = StringParamField(key="p2", maxLength=44, required=True)
    params3 = StringParamField(key="p3", maxLength=44, required=False)

    input1 = ImageBodyField(key="data", path="test.png")
    input3 = ImageBodyField(key="data2", path="test.png")
    input2 = StringBodyField(key="switch", value="cc")


'''
定义响应类:
 accepts:  accepts代表响应中包含哪些字段, 以及数据类型

 input:    input字段多用与请求数据段，即body部分，当前支持 ImageBodyField, StringBodyField, 和AudioBodyField
'''


class UserResponse(object):
    accept1 = StringBodyField(key="boxes")
    accept2 = StringBodyField(key="boxes2")

class Wrapper(WrapperBase):
    serviceId = "mmocr"
    version = "backup.0"
    requestCls = UserRequest()
    responseCls = UserResponse()
    
   # @classmethod
    def wrapperError(cls, ret: int) -> str:
        print("###re",ret)
        if ret == 100:
            return "Infer error defined here"
        return ""
    @classmethod
    def wrapperInit(cls, config: {}) -> int:
        print(config)
        print("Initializing ..")
        return 0

    @classmethod
    def wrapperFini(cls) -> int:
        return 0

    '''
    非会话模式计算接口,对应oneShot请求,可能存在并发调用

    @param usrTag 句柄
    #param params 功能参数
    @param  reqData     写入数据实体
    @param  respData    返回结果实体,内存由底层服务层申请维护,通过execFree()接口释放
    @param psrIds 需要使用的个性化资源标识列表
    @param psrCnt 需要使用的个性化资源个数

    @return 接口错误码
        reqDat
        ret:错误码。无错误码时返回0
    '''

    @classmethod
    def wrapperOnceExec(cls, usrTag: str, params: {}, reqData: [], respData: [], psrIds: [], psrCnt: int) -> int:
        print("I am infer logic...")
        return 100

