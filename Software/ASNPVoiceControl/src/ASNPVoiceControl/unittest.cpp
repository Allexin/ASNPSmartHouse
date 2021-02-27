#include "unittest.h"
#include "clogger.h"

struct sTest
{
    QString sequence;
    QVector<int> calls; //unitTest first argument, or -1 for start sequence
    sTest(QString Sequence, QVector<int> ExpectedCalls):
        sequence(Sequence),
        calls(ExpectedCalls)
    {

    }
};

QVector<sTest> tests;
int currentTest;
int currentCall;
bool success;

class cTestListener: public cParseListener
{
    virtual void onStartSequence() override
    {
        if (currentCall<tests[currentTest].calls.size() && tests[currentTest].calls[currentCall]==-1)
        {

        }
        else
        {
            success = false;
        }
        currentCall++;
    }
    virtual void onExecCmd(const sActionExecCmd& cmd, const QMap<QString,QString>& allVariables) override
    {
        QStringList commands = cmd.commands[0].split(",");
        if (commands[0]!="UnitTest")
        {
            return;
        }
        int callId = commands[1].toInt();
        if (currentCall<tests[currentTest].calls.size() && tests[currentTest].calls[currentCall]==callId)
        {

        }
        else
        {
            success = false;
        }
        currentCall++;
    }
};

cTestListener listener;


void test(cParseTree* parseTree)
{
    parseTree->addListener(&listener);

    tests.clear();
    tests.push_back(sTest("маш включи свет",QVector<int>{-1,4}));
    tests.push_back(sTest("маш закрой ставни",QVector<int>{-1,10}));
    tests.push_back(sTest("машка закрой угловые ставни",QVector<int>{-1,9}));
    tests.push_back(sTest("машка закрой угловые ставни выключи свет",QVector<int>{-1,9,4}));

    logVerbose("UNIT_TEST","STARTED");
    for (int i = 0; i<tests.size(); ++i)
    {
        logVerbose("UNIT_TEST","TEST "+QString::number(i));
        currentTest = i;
        currentCall = 0;
        success = true;
        parseTree->resetParsing();
        QStringList sequence = tests[i].sequence.split(" ");
        for (int j = 0; j<sequence.size(); ++j)
        {
            parseTree->parseWithNextWord(sequence[j],true);
        }
        success = success && currentCall==tests[i].calls.size();
        if (success)
        {
            logVerbose("UNIT_TEST","SUCCESS");
        }
        else
        {
            logVerbose("UNIT_TEST:"+QString::number(i),"FAILED");
        }
    }
    logVerbose("UNIT_TEST","COMPLETE");
}
