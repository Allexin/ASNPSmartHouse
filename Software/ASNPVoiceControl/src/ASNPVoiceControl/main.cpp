#include <QCoreApplication>
#include <QFile>
#include "cparsetree.h"
#include "cdataloader.h"
#include "cvoiceprocessor.h"
#include "casnpclient.h"

//#define UNIT_TEST
#include "unittest.h"



class cCommandListener: public cParseListener
{
protected:
    cASNPClient* m_asnpClient;
    cParseTree* m_parseTree;
public:
    cCommandListener(cParseTree* parseTree, cASNPClient* asnpClient):
        m_asnpClient(asnpClient),
        m_parseTree(parseTree)
    {}

    virtual void onStartSequence() override
    {
    }
    virtual void onExecCmd(const sActionExecCmd& cmd, const QMap<QString,QString>& allVariables) override
    {
        for (int i = 0; i<cmd.commands.size(); ++i)
        {
            QStringList commands = cmd.commands[i].split(",");

            if (commands[0]=="startGate" && commands.size()==3)
            {
                m_asnpClient->startGate(m_parseTree->variableToValue(commands[1]).toInt(),m_parseTree->variableToValue(commands[2]).toInt());
                continue;
            }
            if (commands[0]=="setLight" && commands.size()==5)
            {
                m_asnpClient->setLight(m_parseTree->variableToValue(commands[1]).toInt(),
                                    m_parseTree->variableToValue(commands[2]).toInt(),
                                    m_parseTree->variableToValue(commands[3]).toInt(),
                                    m_parseTree->variableToValue(commands[4]).toInt());
                continue;
            }
        }
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cASNPClient asnpClient;
    cParseTree parseTree;
    cCommandListener commandsListener(&parseTree,&asnpClient);

    loadDataFromFile("data/commands.lst",&parseTree);    

#ifdef UNIT_TEST
    test(&parseTree);
#endif
/*
    const QVector<QString>& words = parseTree.getWords();
    QFile file("dictionary.txt");
    if (file.open(QFile::WriteOnly))
    {
        for (int i = 0; i<words.size(); ++i)
        {
            QString word = (i>0?" ":"")+words[i]+"";
            file.write(word.toUtf8());
        }
        file.close();
    }
*/

    parseTree.addListener(&commandsListener);

    cVoiceProcessor voiceProcessor(&parseTree);
    voiceProcessor.start();

    return a.exec();
}
