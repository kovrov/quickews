#include <qqml.h>

#include "folder.h"
#include "useravailability.h"
#include "plugin.h"

void QuickewsPlugin::registerTypes(const char *uri)
{
    // @uri EWS
    qmlRegisterType<quickews::Folder>(uri, 1, 0, "Folder");
    qmlRegisterType<quickews::UserAvailability>(uri, 1, 0, "UserAvailabilityModel");
}
