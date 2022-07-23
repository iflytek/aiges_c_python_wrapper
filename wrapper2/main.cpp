#include "aiges/wrapper.h"
#include "aiges/type.h"
#include <iostream>
struct ParamList* paramListCreate(){
    struct ParamList* pParamPtr= (struct ParamList*)malloc(sizeof(struct ParamList));
    pParamPtr->key=NULL;
    pParamPtr->value=NULL;
    pParamPtr->vlen=0;
    pParamPtr->next=NULL;
    return pParamPtr;
}

struct DataList* dataListCreate(){
    struct DataList* pDataPtr= (struct DataList*)malloc(sizeof(struct DataList));
    pDataPtr->key=NULL;
    pDataPtr->data=NULL;
    pDataPtr->len=0;
    pDataPtr->type=DataText;
    pDataPtr->status=DataBegin;
    pDataPtr->desc=NULL;
    pDataPtr->next=NULL;
    return pDataPtr;
}

struct DataList* dataListAppend(struct DataList* ptr,char* key,void* data,unsigned int len,int type,int status,struct ParamList* descPtr){
    struct DataList* head=ptr;
    if(ptr!=NULL && ptr->key==NULL){
        ptr->key=key;
        ptr->data=data;
        ptr->len=len;
        ptr->type=(DataType)(type);
        ptr->status=(DataStatus)(status);
        ptr->desc=descPtr;
        ptr->next=NULL;
        return ptr;
    }
    while(ptr->next!=NULL){
        ptr=ptr->next;
    }
    struct DataList* pDataPtr= (struct DataList*)malloc(sizeof(struct DataList));
    pDataPtr->key=key;
    pDataPtr->data=data;
    pDataPtr->len=len;
    pDataPtr->type=(DataType)(type);
    pDataPtr->status=(DataStatus)(status);
    pDataPtr->desc=descPtr;
    pDataPtr->next=NULL;
    ptr->next=pDataPtr;
    return head;
}

int main(int argc,char* argv[]){

    pConfig cfg=paramListCreate();
    int ret=wrapperInit(cfg);
    std::cout<<ret<<std::endl;

    pParamList pList=paramListCreate();

    DataList* dataList=dataListCreate();
    char* data_key="test_key";
    char* data_content="hello world";
    dataList= dataListAppend(dataList,data_key,(void*)data_content,11,0,2,NULL);

    pDataList resp=dataListCreate();
    unsigned int psrIds[1];
    psrIds[0]=3;

    std::string handle="handle0001";
    ret=wrapperExec(handle.c_str(),pList, dataList,&resp, psrIds,0);
    std::cout<<ret<<std::endl;
}