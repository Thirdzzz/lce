#include <iostream>
#include "Utils.h"
#include "CEvent.h"
#include "CCommMgr.h"
#include "CWorkerPool.h"

using namespace std;
using namespace lce;

int iSrv1;


void onError(const char*szErrMsg)
{
    cout<<"onError:"<<szErrMsg<<endl;
}

class CProWorker :  public CWorker
{
public:
    void onRequest(const SRequest &stRequest,SResponse &stResponse)
    {
        cout<<stRequest.getData()<<endl;
        stResponse.write(stRequest.getData());
    }
};



int main(int argc,char **argv)
{

    if(argc < 2)
    {
        printf("usage:%s port\n",argv[0]);
        exit(0);
    }

    //lce::initDaemon(); //后台运行

    CWorkerPool<CProWorker> *poWorkerPool = new CWorkerPool<CProWorker>();
    poWorkerPool->init();
    poWorkerPool->setErrHandler(onError);
    poWorkerPool->run();

    if(CCommMgr::getInstance().init(50000) < 0)
    {
        printf("%s\n",CCommMgr::getInstance().getErrMsg());
        return 0;
    }

    iSrv1 = CCommMgr::getInstance().createSrv(SRV_TCP,"0.0.0.0",atoi(argv[1]));

    if(iSrv1 < 0 )
    {
        cout<<CCommMgr::getInstance().getErrMsg()<<endl;
        return 0;
    }

    CCommMgr::getInstance().setProcessor(iSrv1,poWorkerPool,PKG_RAW);
    CCommMgr::getInstance().start();

    return 0;
}
