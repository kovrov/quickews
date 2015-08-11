#include <qqml.h>

#include "folder.h"
#include "plugin.h"

void QuickewsPlugin::registerTypes(const char *uri)
{
    // @uri EWS
    qmlRegisterType<quickews::Folder>(uri, 1, 0, "Folder");
}
