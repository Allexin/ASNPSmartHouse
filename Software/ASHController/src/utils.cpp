#include "utils.h"

#include <QStandardPaths>
#include <QDir>

QString getAppFolder()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +"/"+"ASNPRemote";
    QDir dir(path);
    dir.mkpath(path);
    return path+"/";
}
