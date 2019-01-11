#include "pqengine.h"
#include "pqenginecore.h"

int main(int argc, char *argv[])
{
    // NOTE: change the pmd5-key if need
    //QString pmd5 = "be71c3787982c632f238b1192b5b6259;0x0";
    PQEngineCore *pqenginecore = new PQEngineCore;
    PQExtensionList pqExtensions;
    pqExtensions << pqenginecore;

    PQEngine engine(pqExtensions);
    PQEngineInitConf cfg;
    cfg.appName = "HelloWorld";
    cfg.appVersion = "1.0";
    cfg.orgName = "phpqt";
    cfg.orgDomain = "phpqt.ru";
    cfg.hashKey = "0x0";
    cfg.pmd5 = "0x0;0x0";
    cfg.checkName = false;
    cfg.debugSocketName = "PQEngine Debug Server";

#ifdef PQDEBUG
    engine.init(argc, argv, "pqengine-debug", cfg);
#else
    engine.init(argc, argv, "pqengine", cfg);
#endif

    // NOTE: change path
    return engine.exec("main.php");
}
