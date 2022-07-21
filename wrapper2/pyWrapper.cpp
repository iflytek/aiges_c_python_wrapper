#include "pyWrapper.h"

//namespace py = pybind11;
//using namespace std::chrono_literals;


const char* WrapperFile = "main";
const char* WrapperClass = "Wrapper";

PyWrapper::PyWrapper()
  {
    py::gil_scoped_acquire acquire;
    _obj = py::module::import(WrapperFile).attr(WrapperClass)();

    _wrapperInit = _obj.attr("wrapperInit");
    _wrapperFini = _obj.attr("wrapperFini");
    _wrapperOnceExec = _obj.attr("wrapperOnceExec");
    _wrapperError = _obj.attr("wrapperError");
    _wrapperTest = _obj.attr("wrapperTestFunc");

  }

PyWrapper::~PyWrapper()
  {
    _wrapperError.release();
    _wrapperInit.release();
    _wrapperFini.release();
    _wrapperOnceExec.release();
    _wrapperTest.release();
  }


int PyWrapper::wrapperInit(std::map<std::string, std::string> config)
{
    py::gil_scoped_acquire acquire;
    return _wrapperInit(config).cast<int>();
}


int PyWrapper::wrapperFini()
{
    py::gil_scoped_acquire acquire;
    return _wrapperFini().cast<int>();
}

int PyWrapper::wrapperOnceExec(std::map<std::string,std::string> params, std::vector<py::object> reqData, std::vector<py::object> respData, std::string sid) {

    py::gil_scoped_acquire acquire;
    return _wrapperOnceExec(params, reqData, respData).cast<int>();
}


std::string PyWrapper::wrapperError(int err){
     py::gil_scoped_acquire acquire;
    return _wrapperError(err).cast<std::string>();
}


int PyWrapper::wrapperTest() {
   py::gil_scoped_acquire acquire;
        std::vector<py::dict> req;
	py::dict item;
	item["key"] = "33"; 
	item["data"] = "ccccc";
	item["len"] = 11;
	item["type"] =22;
	req.push_back(item);
   std::vector<py::object>  resp;
   int ret =  _wrapperTest(req, &resp).cast<int>();
   printf("resp len%d\n",resp.size());
   printf("%d", ret);


}

