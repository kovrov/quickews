#ifndef QUICKEWS_PLUGIN_H
#define QUICKEWS_PLUGIN_H

#include <QQmlExtensionPlugin>

class QuickewsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // QUICKEWS_PLUGIN_H
