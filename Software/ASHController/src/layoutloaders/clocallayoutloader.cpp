#include "clocallayoutloader.h"

#include <QStandardPaths>
#include <QFile>

#include "../widgets/cwidgetlabel.h"
#include "../utils.h"

cBaseWidget * cLocalLayoutLoader::loadLayouts()
{
    QString dataPath = getAppFolder()+"asnp_layouts.lt";

    cBaseWidget * layouts = nullptr;
    if (QFile::exists(dataPath))
    {
        QFile f(dataPath);
        if (f.open(QIODevice::ReadOnly))
        {
            layouts = parse(&f);
            f.close();
        }
    }


    if (!layouts)
    {
        cWidgetLabel* label = new cWidgetLabel();
        label->setGeometry(QPoint(),QSize(500,100));
        label->setText(tr("Layout config not found. Please create layout config: ")+dataPath);

        layouts = label;
    }

    return layouts;
}
