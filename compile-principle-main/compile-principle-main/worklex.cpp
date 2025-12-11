#include "worklex.h"
#include<QChar>
#include<QDebug>
#include<QMultiMap>
#include<QRegularExpression>
int WorkLex::depth=1;    //静态成员类外初始化


//词法分析
void WorkLex::analyze(const QString code) //把整个都传进来了
{
    QString filterSource=inputFilter(code);  //先去掉柱思华
   // qDebug()<<filterSource;

    int pos=0;

    int cur_line=1;
    for(;pos<filterSource.length();pos++)
    {
        while(filterSource[pos]!=' '&&pos<filterSource.length()-1)
        {
            QChar ch;
            ch=filterSource[pos];
            //qDebug()<<ch;
            if(isNumber(ch))
            {
                pos=numToken(pos,cur_line,filterSource);

            }else if(isalpha(ch))
            {
                pos=alphaToken(pos,cur_line,filterSource);
            }else if(isSeparator(ch))
            {
                pos=seperatorToken(pos,cur_line,filterSource);
            }else if(isOperator(ch))
            {
                pos=operatorToken(pos,cur_line,filterSource);
            }else if(ch=='\n')
            {
                cur_line++;
                pos++;
            }else
            {
                tokens.push_front({TokenType::ERROR,"错误字符  "+QString(ch),cur_line});
                qDebug()<<"未知字符"<<Qt::endl;
                  pos++;
            }
        }
    }
    checkUnclosedStructures(); //检测作用域是否闭合
    printTokens();
}

void WorkLex::checkUnclosedStructures()
{
    if(domainStack.size())
    {
        QString value="Scope not closed";
        tokens.push_back({TokenType::ERROR,value,-1});  //-1表示最后一行
    }
}

QString WorkLex::inputFilter(QString inputfile)
{
    QString temp;
    int pos=0;
    for(pos;pos<inputfile.length();pos++)
    {
        //单行注释过滤
        if(inputfile[pos] == '/' && inputfile[pos+1]=='/')
        {
            while(inputfile[pos]!='\n')
            {
                pos++;
            }
        }

        //多行注释过滤

        if(inputfile[pos]=='/'&&inputfile[pos+1]=='*')
        {
            while(!(inputfile[pos+1]=='/' &&inputfile[pos]=='*'))
            {
                if(inputfile[pos]=='\0')
                {
                    qDebug()<<"annatation error"<<Qt::endl;
                }
                ++pos;
            }
            pos+=2;
        }

        if(inputfile[pos]!='\v'&&inputfile[pos]!='\t')
        {
            temp+=inputfile[pos];
        }
    }

    return temp;
}

void WorkLex::printTokens()
{
    for(auto e:tokens)
    {
        qDebug()<<e.type<<" "<<e.value<<Qt::endl;
    }
}

int WorkLex::numToken(int pos, int cur_line,QString filterSource)
{
    QString value;

    value+=filterSource[pos++]; //指针向后移动

    //数字类型
    while(isNumber(filterSource[pos])||filterSource[pos]=='.')
    {
        value+=filterSource[pos++];
    }

    //生成数字类型并生成token
    tokens.push_back({TokenType::NUMBER,value,cur_line});

    //返回分析的最新位置
    return pos;


}

int WorkLex::alphaToken(int pos, int cur_line, QString filterSource)
{
    QString value;
    value+=filterSource[pos++];

    //后面字符是数字或者字母
    while(isalpha(filterSource[pos])||isNumber(filterSource[pos]))
    {
        value+=filterSource[pos++];
    }

    //查表，不是关键字就是标识符
    if(isKeyword(value)){
        tokens.push_back({TokenType::KEYWORD,"name = "+value,cur_line});
    }else
    {
        // if(isValidIdentifier(value))
        // {
        //     tokens.push_back({TokenType::ERROR,value,cur_line});
        // }
       tokens.push_back({TokenType::IDENTIFIER,value,cur_line}); //关键字是否符合要求需要检查一下
        multiMap.insert(WorkLex::depth,value);
    }
    return pos;
}

int WorkLex::seperatorToken(int pos, int cur_line, QString filterSource)
{
    QString value;

    value+=filterSource[pos++];
    if(value=="{")   //每进一个作用域都要域宽加1，假设最开始域的深度为1
    {
        enterscope('{');
    }else if(value=='}')
    {
        existscope(cur_line);
    }

    tokens.push_back({TokenType::SEPARATOR,value,cur_line});
        return pos;
}

int WorkLex::operatorToken(int pos, int cur_line, QString filterSource)
{
    QString value;


    if (filterSource[pos] == '=' && filterSource[pos + 1] == '=' ){
        value = "==";
        pos+=2;
    }else if(filterSource[pos] == '>' && filterSource[pos + 1] == '='){
        value = ">=";
        pos+=2;
    }else if(filterSource[pos] == '>' && filterSource[pos + 1] == '>'){
       value = ">>";
        pos+=2;
    }else if(filterSource[pos] == '<' && filterSource[pos + 1] == '='){
        value = "<=";
        pos+=2;
    }else if(filterSource[pos] == '<' && filterSource[pos + 1] == '<'){
        value = "<<";
        pos+=2;
    }else if(filterSource[pos] == '!' && filterSource[pos + 1] == '='){
        value = "!=";
        pos+=2;
    }else if(filterSource[pos] == '&' && filterSource[pos + 1] == '&'){
        value = "&&";
        pos+=2;
    }else if(filterSource[pos] == '|' && filterSource[pos + 1] == '|'){
        value = "||";
        pos+=2;
    }else{
        value = filterSource[pos++];
    }

    tokens.push_back({TokenType::OPERATOR,value,cur_line});
    return pos;

}

bool WorkLex::isKeyword(const QString word)
{
    return keyword.contains(word);
}

bool WorkLex::isSeparator(const QChar s)
{
    return separators.contains(QString(s));
}

bool WorkLex::isOperator(const QChar s)
{
    for(auto e:operators)
    {
        if(e[0]==s)              //每个运算符的第一个是否鱼这个字符匹配
        {
            return true;
        }
    }
    return false;
}


bool WorkLex::isIdentifier(const QString word)
{
 return true;
}

bool WorkLex::isNumber(const QChar s)
{
    return s.isNumber();
}

bool WorkLex::isalpha(const QChar s)
{
    return  (s.isLetter()||s=='_');
}

//进入作用域
void WorkLex::enterscope(QChar s)
{
    domainStack.push(s);
    WorkLex::depth++;
}
//出作用域
void WorkLex::existscope(int cur_line)
{
    WorkLex::depth--;
    if(WorkLex::depth<1)
    {
        QString value="Scope not closed";
        tokens.push_back({TokenType::ERROR,value,cur_line});
    }else
    {
        domainStack.pop();  //都可以弹出来
    }


}

QVector<WorkLex::Token> WorkLex::getTokens()
{
    return tokens;
}

QString WorkLex::tokenTypeToString(TokenType type)
{
    static const QMap<TokenType, QString> typeToStringMap = {
        {KEYWORD, "KEYWORD"},
        {SEPARATOR, "SEPARATOR"},
        {OPERATOR, "OPERATOR"},
        {IDENTIFIER, "IDENTIFIER"},
        {NUMBER, "NUMBER"},
        {ERROR, "ERROR"}
    };
    return typeToStringMap.value(type, "UNKNOWN_TYPE"); // 如果找不到匹配项，
}

bool WorkLex::isValidIdentifier(const QString &str)
{
    // 正则表达式模式：以字母或下划线开头，后面可以跟任意数量的字母、数字或下划线
    QRegularExpression regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    QRegularExpressionMatch match = regex.match(str);
    return match.hasMatch();
}
