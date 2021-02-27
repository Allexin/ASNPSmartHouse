#include "cdataloader.h"

#include <QFile>
#include <QTextStream>
#include "clogger.h"

enum class eCurrentDataType
{
    NONE,
    VARIANTS,
    START_SEQUENCE,
    SET_VALUE,
    SET_VALUE_VALUE,
    EXEC_CMD,
    EXEC_CMD_CMD,
    NOT_DATA_TYPE
};


eCurrentDataType strindToDataType(QString& value)
{
    if (value.startsWith("["))
    {
        if (value=="[VARIANTS]")
        {
            return eCurrentDataType::VARIANTS;
        }
        if (value=="[START_SEQUENCE]")
        {
            return eCurrentDataType::START_SEQUENCE;
        }
        if (value=="[SET_VALUE]")
        {
            return eCurrentDataType::SET_VALUE;
        }
        if (value=="[VALUE]")
        {
            return eCurrentDataType::SET_VALUE_VALUE;
        }
        if (value=="[EXEC_CMD]")
        {
            return eCurrentDataType::EXEC_CMD;
        }
        if (value=="[CMD]")
        {
            return eCurrentDataType::EXEC_CMD_CMD;
        }
        if (value=="[END_OF_FILE]")
        {
            return eCurrentDataType::NONE;
        }
    }
    return eCurrentDataType::NOT_DATA_TYPE;
}

bool loadDataFromFile(QString fileName, cParseTree *parseTree)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        logError("DATA_PARSE","Can't load data file "+fileName);
        return false;
    }

    parseTree->clear();
    eCurrentDataType dataType = eCurrentDataType::NONE;

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QVector<QString> mainData;
    QVector<QString> secData;
    int lineIndex = 1;

    while(!in.atEnd()) {
        QString line = in.readLine().trimmed();
        eCurrentDataType nextDataType = strindToDataType(line);
        if (nextDataType==eCurrentDataType::NOT_DATA_TYPE)
        {
            if (dataType==eCurrentDataType::SET_VALUE_VALUE || dataType==eCurrentDataType::EXEC_CMD_CMD)
            {
                secData.push_back(line);
            }
            else
            {
                mainData.push_back(line);
            }
        }
        else
        {
            if (nextDataType!=eCurrentDataType::SET_VALUE_VALUE && nextDataType!=eCurrentDataType::EXEC_CMD_CMD)
            {
                switch(dataType)
                {
                    case eCurrentDataType::VARIANTS:{
                        parseTree->addVariants(mainData);
                    }; break;
                    case eCurrentDataType::START_SEQUENCE:{
                        for (int i = 0; i<mainData.size(); ++i)
                        {
                            parseTree->addStartSequence(mainData[i]);
                        }
                    }; break;
                    case eCurrentDataType::SET_VALUE:{
                        logError("DATA_PARSE:"+QString::number(lineIndex),"Wrong lines. [VALUE] expected, but "+line+"found");
                    }; break;
                    case eCurrentDataType::SET_VALUE_VALUE:{
                        for (int i = 0; i<mainData.size(); ++i)
                        {
                            parseTree->addSetValueSequence(mainData[i],secData);
                        }
                    }; break;
                    case eCurrentDataType::EXEC_CMD:{
                        logError("DATA_PARSE:"+QString::number(lineIndex),"Wrong lines. [CMD] expected, but "+line+"found");
                    }; break;
                    case eCurrentDataType::EXEC_CMD_CMD:{
                        for (int i = 0; i<mainData.size(); ++i)
                        {
                            parseTree->addExecCmdSequence(mainData[i],secData);
                        }
                    }; break;
                    case eCurrentDataType::NONE:{
                        //DO NOTHING
                    }; break;
                    case eCurrentDataType::NOT_DATA_TYPE:{
                        logError("DATA_PARSE:"+QString::number(lineIndex),"NOT_DATA_TYPE impossible value");
                    }; break;
                }
                mainData.clear();
                secData.clear();
            }
            dataType = nextDataType;
        }
        ++lineIndex;
    }

    file.close();
    return true;
}
