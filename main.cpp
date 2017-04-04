#include "pqengine.h"
#include "pqenginecore.h"

int main(int argc, char *argv[])
{
    // NOTE: change the pmd5-key if need
    QString pmd5 = "be71c3787982c632f238b1192b5b6259;0x0";
    PQExtensionList pqExtensions;

    PQEngineCore *pqenginecore = new PQEngineCore;

    pqExtensions << pqenginecore;

    pqenginecore->use_instance();
    pqenginecore->use_pre();
    pqenginecore->use_ub_write();

    PQEngine engine(pqExtensions);
#ifdef PQDEBUG
    engine.init(argc, argv, pmd5, "pqengine-debug", false, "0x0", "", "1.0", "", "");
#else
    engine.init(argc, argv, pmd5, "pqengine", false, "0x0", "", "1.0", "", "");
#endif

    // NOTE: change path
    return engine.exec("qrc://scripts/main.php");
}
