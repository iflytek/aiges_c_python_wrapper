#include "pyWrapper.h"
#include <dlfcn.h>

//namespace py = pybind11;
//using namespace std::chrono_literals;


const char *WrapperFile = "wrapper";
const char *WrapperClass = "Wrapper";


PYBIND11_EMBEDDED_MODULE(aiges_embed, module
){

py::class_ <ResponseData> responseData(module, "ResponseData");
responseData.

def (py::init<>())

.def_readwrite("key",&ResponseData::key)
.def_readwrite("data", &ResponseData::data)
.def_readwrite("status", &ResponseData::status)
.def_readwrite("len", &ResponseData::len)
.def_readwrite("type", &ResponseData::type);

py::class_<Response> response(module, "Response");
response.

def (py::init<>())

.def_readwrite("list",&Response::list);

py::class_<DataListNode> dataListNode(module, "DataListNode");
dataListNode.

def (py::init<>())

.def_readwrite("key",&DataListNode::key)
.def_readwrite("data", &DataListNode::data)
.def_readwrite("len", &DataListNode::len)
.def_readwrite("type", &DataListNode::type);

py::class_<DataListCls> dataListCls(module, "DataListCls");
dataListCls.

def (py::init<>())

.def_readwrite("list",&DataListCls::list);
}


PyWrapper::PyWrapper() {
    py::gil_scoped_acquire acquire;
    _obj = py::module::import(WrapperFile).attr(WrapperClass)();

    _wrapperInit = _obj.attr("wrapperInit");
    _wrapperFini = _obj.attr("wrapperFini");
    _wrapperOnceExec = _obj.attr("wrapperOnceExec");
    _wrapperError = _obj.attr("wrapperError");
    _wrapperTest = _obj.attr("wrapperTestFunc");

}

Manager::Manager() {

    dlopen("libpython3.so", RTLD_GLOBAL | RTLD_NOW);
}

PyWrapper::~PyWrapper() {
    _wrapperError.release();
    _wrapperInit.release();
    _wrapperFini.release();
    _wrapperOnceExec.release();
    _wrapperTest.release();
    pybind11::gil_scoped_release release;
}


int PyWrapper::wrapperInit(std::map <std::string, std::string> config) {
	    py::gil_scoped_acquire acquire;

    return _wrapperInit(config).cast<int>();
}


int PyWrapper::wrapperFini() {
    py::gil_scoped_acquire acquire;

    return _wrapperFini().cast<int>();
}

int PyWrapper::wrapperOnceExec(std::map <std::string, std::string> params, DataListCls reqData,
                               pDataList *respData, std::string sid) {
    try {
        py::gil_scoped_acquire acquire;
        py::object r = _wrapperOnceExec(params, reqData);
	py::gil_scoped_release release;
        Response *resp;
	spdlog::debug("start cast python resp to c++ object, thread_id: {}, sid: {}", gettid(), sid);
        resp = r.cast<Response *>();
        pDataList headPtr;
        pDataList prePtr;
        pDataList curPtr;
        for (int idx = 0; idx < resp->list.size(); idx++) {
            pDataList tmpData = new(DataList);
            ResponseData itemData = resp->list[idx];
            char *key = strdup(itemData.key.c_str());
            tmpData->key = key;
            tmpData->len = itemData.len;
            tmpData->type = DataType(itemData.type);
            void *r;
            r = malloc(itemData.data.length());
            if (r == NULL) {
                int ret = -1;
                spdlog::error("Can't malloc memory,  sid:{}", sid);
                return ret;
            }
            memcpy(r, itemData.data.data(), itemData.data.length());
            //char *data_ = new char[itemData.data.length()+1];
            // strdup(.c_str());
            tmpData->data = r;
            tmpData->status = DataStatus(itemData.status);
            if (idx == 0) {
                headPtr = tmpData;
                prePtr = tmpData;
                curPtr = tmpData;
            } else {
                curPtr = tmpData;
                prePtr->next = curPtr;
                prePtr = curPtr;
            }
            spdlog::debug("get result,key:{},data:{},len:{},type:{},status:{},sid:{}",
                          tmpData->key, (char *) tmpData->data, tmpData->len, tmpData->type,
                          tmpData->status, sid);
        }
        *respData = headPtr;
    } catch (py::cast_error &e) {
	    spdlog::error("cast error: {}", e.what());
        return -1;
    }

    return 0;

}


std::string PyWrapper::wrapperError(int err) {
     return _wrapperError(err).cast<std::string>();
}


int PyWrapper::wrapperTest() {
    py::gil_scoped_acquire acquire;
    std::vector <py::dict> req;
    py::dict item;
    item["key"] = "33";
    item["data"] = "ccccc";
    item["len"] = 11;
    item["type"] = 22;
    req.push_back(item);
    std::vector <py::object> resp;
    resp.push_back(py::str("cfc"));
    py::object ret = _wrapperTest(req, resp);
    Response *l;
    try {
        l = ret.cast<Response *>();
        // py::object r =  _wrapperOnceExec(params, reqData);
    } catch (py::cast_error &e) {
        std::cout << "errC" << std::endl;
        return -1;
    }
    for (int i = 0; i < l->list.size(); ++i) {
        ResponseData d = l->list[i];
        std::cout << "Response key: " << d.key << std::endl;
        std::cout << "Response len" << d.len << std::endl;
        std::cout << "response actual data Size " << d.data.length() << std::endl;


    }
    return 0;
//  auto message = ret.cast<std::vector<std::string>>();
    // printf("%s,:", message[0].c_str());
    // printf("%s,:", message[1].c_str());
    //  printf("resp len%d\n",resp.size());
//   printf("%d", ret);


}

