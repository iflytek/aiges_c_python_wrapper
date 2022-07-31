#!/usr/bin/env python
# coding:utf-8
"""
@author: nivic ybyang7
@license: Apache Licence
@file: wrapper.py
@time: 2022/06/16
@contact: ybyang7@iflytek.com
@site:
@software: PyCharm

"""

#  Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
#  Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
#  Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
#  Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
#  Vestibulum commodo. Ut rhoncus gravida arcu.
import sys

try:
    from aiges_embed import ResponseData, Response, DataListNode, DataListCls  # c++
except:
    from aiges.dto import Response, ResponseData, DataListNode, DataListCls

import hashlib
from aiges.sdk import WrapperBase, \
    StringParamField, \
    ImageBodyField, \
    StringBodyField
from aiges.utils.log import log

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

    input1 = ImageBodyField(key="data", path="test_data/test.png")
    input3 = ImageBodyField(key="data2", path="test_data/test.png")
    input2 = StringBodyField(key="switch", value="cc")


'''
定义响应类:
 accepts:  accepts代表响应中包含哪些字段, 以及数据类型

 input:    input字段多用与请求数据段，即body部分，当前支持 ImageBodyField, StringBodyField, 和AudioBodyField
'''


class UserResponse(object):
    accept1 = StringBodyField(key="boxes")
    accept2 = StringBodyField(key="boxes2")


'''
用户实现， 名称必须为Wrapper, 必须继承SDK中的 WrapperBase类
'''


class Wrapper(WrapperBase):
    serviceId = "mmocr"
    version = "backup.0"
    requestCls = UserRequest()
    responseCls = UserResponse()

    '''
    服务初始化
    @param config:
        插件初始化需要的一些配置，字典类型
        key: 配置名
        value: 配置的值
    @return
        ret: 错误码。无错误时返回0
    '''

    def wrapperInit(cls, config: {}) -> int:
        print(config)
        print("Initializing ..")
        return 0

    '''
    非会话模式计算接口,对应oneShot请求,可能存在并发调用

    @param usrTag 句柄
    @param params 功能参数
    @param  reqData     请求数据实体字段
    @param  respData    返回结果实体,内存由底层服务层申请维护,通过execFree()接口释放
    @param psrIds 需要使用的个性化资源标识列表
    @param psrCnt 需要使用的个性化资源个数

    @return 接口错误码
        reqDat
        ret:错误码。无错误码时返回0
    '''

    def wrapperOnceExec(cls, params: {}, reqData: DataListCls) -> Response:
        log.info("got reqdata , %s" % reqData.list)
        #        print(type(reqData.list[0].data))
        #        print(type(reqData.list[0].data))
        #        print(reqData.list[0].len)
        for req in reqData.list:
            log.info("reqData key: %s , size is %d" % (req.key,len(req.data)))
        log.warning("reqData bytes md5sum is %s" % hashlib.md5(reqData.list[0].data).hexdigest())
        log.info("I am infer logic...please inplement")

        r = Response()
        l = ResponseData()
        l.key = "ccc"
        l.status = 1
        d = open("test_data/test.png", "rb").read()
        l.len = len(d)
        l.data = d
        l.type = 0
        r.list = [l, l, l]
        return r

    '''
    服务逆初始化

    @return
        ret:错误码。无错误码时返回0
    '''

    def wrapperFini(cls) -> int:
        return 0

    def wrapperError(cls, ret: int) -> str:
        if ret == 100:
            return "user error defined here"
        return ""


if __name__ == '__main__':
    m = Wrapper()
    # m.schema()
    m.run()
