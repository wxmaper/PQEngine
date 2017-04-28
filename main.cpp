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

#ifdef PQDEBUG
    engine.init(argc, argv, "pqengine-debug", cfg);
#else
    engine.init(argc, argv, "pqengine", cfg);
#endif

    // NOTE: change path
    return engine.exec("qrc://scripts/main.php");
}
