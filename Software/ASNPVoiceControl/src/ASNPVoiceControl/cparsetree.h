#ifndef CPARSETREE_H
#define CPARSETREE_H

#include <QVector>
#include <QStringList>
#include <QMap>

struct sVariants
{
    QVector<QString> words;
    bool contains(QString word)
    {
        return words.contains(word);
    }
};

struct sActionSetValue
{
    QString name;
    QString value;

    sActionSetValue(QString rawString)
    {
        QStringList data = rawString.split("=",Qt::SkipEmptyParts);
        if (data.size()>=2)
        {
            name = data[0];
            value = data[1];
        }
    }
};

struct sActionExecCmd
{
    QVector<QString> commands;
    sActionExecCmd(QVector<QString> Commands):
        commands(Commands)
    {
    }
};

class cParseTree;

struct sWord
{
    QString word;
    bool canBeLastWord;

    sWord(QString Word, bool CanBeLastWord):
        word(Word),
        canBeLastWord(CanBeLastWord)
    {
    }
};

struct sTreeNode
{
    int layer;
    cParseTree* owner;
    sVariants word;
    QVector<sActionSetValue> actionsSetValue;
    QVector<sActionExecCmd> actionsCommands;
    bool start;
    QVector<sTreeNode*> children;

    sTreeNode(cParseTree* Owner, sVariants Word, int Layer):
        owner(Owner),
        word(Word),
        layer(Layer)
    {
        start = false;
    }

    ~sTreeNode()
    {
        clear();
    }

    void clear()
    {
        for (int i = 0; i<children.size(); ++i)
        {
            delete children[i];
        }
        children.clear();
    }

    sTreeNode* findOrAddChild(QString word);
    sTreeNode* find(QString word);

    QVector<sTreeNode*> parse(QVector<sWord>& sequence, int index = 0);

    int getFirstValidWordIndex(QVector<sWord>& sequence);

    bool hasAction()
    {
        return start || actionsCommands.size()>0 || actionsSetValue.size()>0;
    }

};

class cParseListener
{
public:
    virtual void onStartSequence() = 0;
    virtual void onExecCmd(const sActionExecCmd& cmd, const QMap<QString,QString>& allVariables) = 0;
};

class cParseTree
{
protected:
    static const int MAX_SEQUENCE_LENGTH = 100;

    QVector<sVariants>  m_WordsVariants;
    QVector<QString>    m_Words;

    sTreeNode  m_StartSequence;
    sTreeNode  m_SetValue;
    sTreeNode  m_ExecCmd;

    QVector<sWord> m_CurrentSequence;
    bool m_Started;

    QMap<QString,QString> m_AllVariables;

    QVector<cParseListener*> m_Listeners;
    void resetVariables();
    bool parseSetValue();
    bool parseExecCmd();
    QString toValue(QString rawValue)
    {
        auto value = m_AllVariables.find(rawValue);
        if (value==m_AllVariables.end())
            return rawValue;
        if (value.value()==rawValue)
            return rawValue;
        return variableToValue(value.value());
    }
public:
    sVariants getVariants(QString word);

    cParseTree();

    void addListener(cParseListener* listener)
    {
        m_Listeners.push_back(listener);
    }

    void clear();
    void addVariants(QVector<QString> words);
    void addStartSequence(QString sequence);
    void addSetValueSequence(QString sequence, QVector<QString> variables);
    void addExecCmdSequence(QString sequence, QVector<QString> commands);

    void resetParsing();
    void parseWithNextWord(QString word, bool longPause);
    QString variableToValue(QString rawValue)
    {
        auto value = m_AllVariables.find(rawValue);
        if (value==m_AllVariables.end())
            return rawValue;
        return value.value();
    }

    QVector<QString>& getWords()
    {
        return m_Words;
    }

    bool isValidWord(QString& word)
    {
        return m_Words.contains(word);
    }
};

#endif // CPARSETREE_H
