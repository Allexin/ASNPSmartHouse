#ifndef CLAYOUTLOADER_H
#define CLAYOUTLOADER_H

#include "cbasewidget.h"

class cLayoutLoader: public QObject
{
private:
    struct sKeyValue
    {
        QString key;
        QString value;
        sKeyValue(QString s);
    };

    QVector<cBaseWidget*> loadWidgets(QIODevice* device);
    cBaseWidget* loadWidget(QIODevice* device);
    cBaseWidget* createWidget(QString type);
protected:
    cBaseWidget* parse(QIODevice* device);
public:
    virtual cBaseWidget* loadLayouts() = 0;
};

#endif // CLAYOUTLOADER_H
