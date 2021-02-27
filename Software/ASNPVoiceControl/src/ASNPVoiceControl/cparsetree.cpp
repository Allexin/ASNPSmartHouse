#include "cparsetree.h"
#include <QMapIterator>
#include <QStringList>

void cParseTree::resetVariables()
{
    QMap<QString, QString>::iterator it = m_AllVariables.begin();
    while (it != m_AllVariables.constEnd()) {
        it.value() = "";
        ++it;
    }
}

sVariants cParseTree::getVariants(QString word)
{
    for (int i = 0; i<m_WordsVariants.size(); ++i)
    {
        if (m_WordsVariants[i].contains(word))
        {
            return m_WordsVariants[i];
        }
    }

    sVariants variants;
    variants.words.push_back(word);
    if (!m_Words.contains(word))
    {
        m_Words.push_back(word);
    }
    return variants;
}

cParseTree::cParseTree():
    m_StartSequence(this,sVariants(),0),
    m_SetValue(this,sVariants(),0),
    m_ExecCmd(this,sVariants(),0)
{

}

void cParseTree::clear()
{
    m_WordsVariants.clear();
    m_Words.clear();

    m_StartSequence.clear();
    m_SetValue.clear();
    m_ExecCmd.clear();

    resetParsing();
}

void cParseTree::addVariants(QVector<QString> words)
{
    sVariants variants;
    variants.words = words;
    m_WordsVariants.push_back(variants);
    for (int i = 0; i<words.size(); ++i)
    {
        m_Words.push_back(words[i]);
    }
}

void cParseTree::addStartSequence(QString sequence)
{
    QStringList wordsList = sequence.split(" ",Qt::SkipEmptyParts);
    if (wordsList.size()==0)
    {
        return;
    }
    sTreeNode* currentNode = &m_StartSequence;
    for (int i = 0; i<wordsList.size(); ++i)
    {
        currentNode = currentNode->findOrAddChild(wordsList[i]);
    }
    currentNode->start = true;
}

void cParseTree::addSetValueSequence(QString sequence, QVector<QString> variables)
{
    QStringList wordsList = sequence.split(" ",Qt::SkipEmptyParts);
    if (wordsList.size()==0)
    {
        return;
    }
    sTreeNode* currentNode = &m_SetValue;
    for (int i = 0; i<wordsList.size(); ++i)
    {
        currentNode = currentNode->findOrAddChild(wordsList[i]);
    }
    for (int i = 0; i<variables.size(); ++i)
    {
        currentNode->actionsSetValue.push_back(sActionSetValue(variables[i]));
    }
}

void cParseTree::addExecCmdSequence(QString sequence, QVector<QString> commands)
{
    QStringList wordsList = sequence.split(" ",Qt::SkipEmptyParts);
    if (wordsList.size()==0)
    {
        return;
    }
    sTreeNode* currentNode = &m_ExecCmd;
    for (int i = 0; i<wordsList.size(); ++i)
    {
        currentNode = currentNode->findOrAddChild(wordsList[i]);
    }
    currentNode->actionsCommands.push_back(sActionExecCmd(commands));
}

void cParseTree::resetParsing()
{
    m_CurrentSequence.clear();
    m_Started = false;
    resetVariables();
}

void cParseTree::parseWithNextWord(QString word, bool longPause)
{
    if (m_Words.contains(word))
    {
        m_CurrentSequence.push_back(sWord(word,longPause));
    }
    if (m_CurrentSequence.size()>=MAX_SEQUENCE_LENGTH)
    {
        //HARD reset if something goes wrong
        resetParsing();
        return;
    }
    if (m_CurrentSequence.size()==0)
    {
        return;
    }
    if (!m_Started)
    {
        while (m_CurrentSequence.size()>0 && !m_Started)
        {
            int index = m_StartSequence.getFirstValidWordIndex(m_CurrentSequence);
            if (index==-1)
            {
                m_CurrentSequence.clear();
                return;
            }
            if (index>0)
            {
                m_CurrentSequence.remove(0,index);
            }
            {
                sTreeNode* node = m_StartSequence.find(m_CurrentSequence[0].word);
                if (!node)
                {
                    //HARD reset - something goes wrong
                    resetParsing();
                    return;
                }
            }
            QVector<sTreeNode*> nodes = m_StartSequence.parse(m_CurrentSequence);
            for (int i = 0; i<nodes.size() && !m_Started; ++i)
            {
                if (nodes[i]->start)
                {
                    m_Started = true;
                    m_CurrentSequence.remove(0,nodes[i]->layer);
                    break;
                }
            }
            if (m_Started)
            {
                for (int i = 0; i<m_Listeners.size(); ++i)
                {
                    m_Listeners[i]->onStartSequence();
                }
                parseWithNextWord("",false);
                return;
            }
            if (nodes.size()==0)
            {
                if (m_CurrentSequence.size()>0)
                {
                    m_CurrentSequence.remove(0);
                }
            }
            else
            {
                return;
            }
        }
    }

    if (!m_Started)
    {
        return;
    }

    //at this moment we have sequence without start subsequence
    int indexSetValue = m_SetValue.getFirstValidWordIndex(m_CurrentSequence);
    int indexExecValue = m_ExecCmd.getFirstValidWordIndex(m_CurrentSequence);
    if (indexSetValue==-1 && indexExecValue==-1)
    {
        m_CurrentSequence.clear();
        return;
    }
    if ((indexSetValue<indexExecValue || indexExecValue==-1) && indexSetValue>-1)
    {
        if (!parseSetValue())
        {
            if (indexExecValue>-1)
            {
                if (!parseExecCmd())
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
    }
    else
    {
        if (!parseExecCmd())
        {
            if (indexSetValue>-1)
            {
                if (!parseSetValue())
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
    }
    parseWithNextWord("",false);
}

bool cParseTree::parseSetValue()
{
    int index = m_SetValue.getFirstValidWordIndex(m_CurrentSequence);
    auto nodes = m_SetValue.parse(m_CurrentSequence,index);
    for (int i = 0; i<nodes.size(); ++i)
    {
        if (nodes[i]->actionsSetValue.size()>0)
        {
            for (int j = 0; j<nodes[i]->actionsSetValue.size(); ++j)
            {
                m_AllVariables[nodes[i]->actionsSetValue[j].name] = toValue(nodes[i]->actionsSetValue[j].value);
            }
            m_CurrentSequence.remove(0,index+nodes[i]->layer);
            return true;
        }
    }
    return false;
}

bool cParseTree::parseExecCmd()
{
    int index = m_ExecCmd.getFirstValidWordIndex(m_CurrentSequence);
    auto nodes = m_ExecCmd.parse(m_CurrentSequence,index);
    for (int i = 0; i<nodes.size(); ++i)
    {
        if (nodes[i]->actionsCommands.size()>0)
        {
            for (int j = 0; j<nodes[i]->actionsCommands.size(); ++j)
            {                
                for (int k = 0; k<m_Listeners.size(); ++k)
                {
                    m_Listeners[k]->onExecCmd(nodes[i]->actionsCommands[j],m_AllVariables);
                }
            }
            resetVariables();
            m_CurrentSequence.remove(0,index+nodes[i]->layer);
            return true;
        }
    }
    return false;
}

sTreeNode *sTreeNode::findOrAddChild(QString word)
{
    sTreeNode* node = find(word);
    if (node)
    {
        return node;
    }

    node = new sTreeNode(owner,owner->getVariants(word),layer+1);
    children.push_back(node);
    return node;
}

sTreeNode *sTreeNode::find(QString word)
{
    for (int i = 0; i<children.size(); ++i)
    {
        if (children[i]->word.contains(word))
        {
            return children[i];
        }
    }
    return nullptr;
}

QVector<sTreeNode*> sTreeNode::parse(QVector<sWord> &sequence, int index)
{
    QVector<sTreeNode*> parsed;
    sTreeNode* node = this;
    int i = index;
    while (i<sequence.size())
    {
        node = node->find(sequence[i].word);
        if (!node)
        {
            return parsed;
        }

        if (node->hasAction() && sequence[i].canBeLastWord)
        {
            parsed.push_back(node);
        }

        ++i;
    }
    if (node)
    {
        parsed.push_back(node);
    }
    return parsed;
}

int sTreeNode::getFirstValidWordIndex(QVector<sWord> &sequence)
{
    for (int i = 0; i<sequence.size(); ++i)
    {
        if (find(sequence[i].word))
        {
            return i;
        }
    }
    return -1;
}
