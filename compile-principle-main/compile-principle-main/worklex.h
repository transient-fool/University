#ifndef WORKLEX_H
#define WORKLEX_H
#include<QStringList>
#include<QSet>
#include<QVector>
#include<QStack>
#include <QMultiMap>
#include<QMap>

class WorkLex
{
public:

    //创建几个表来存储符号
    //关键字表
    QStringList keyword = {"begin","call","const","do","end","if","odd","procedure","read","then","var","while","write","return","include",
      "int","double","main","stdio","iosteam","cstring","algorithm","for","while","do","queue","vector","stack","using","namespace","std","printf"};
    QStringList separators = {"," ,"." ,"{" ,"}" ,"(" ,")" ,";","#",}; //界符
    QStringList operators = {"+", "-", "*", "++", "--", "-", "/", "**","<=",">",">=","<","=","==","!="}; //运算符号表


    enum TokenType{
        KEYWORD = 1,   //关键字
        SEPARATOR,     //界符
        OPERATOR,   //运算符号表
        IDENTIFIER,  //标识符
        NUMBER,  //数字
        ERROR   //错误
    };

    struct Token{
        TokenType type;
        QString value;
        int line;
    };

    static int depth;

    void analyze(const QString code); //词法分析程序
    QVector<Token> getTokens();
    QString tokenTypeToString(TokenType type);//枚举转字符串

private:
    QVector<Token>tokens; //保存分析出的符号
    QVector<Token>errors; //保存错误信息
    QSet<QString> declarations; // 用于保存唯一的标识符或声明
    QStack<QChar>domainStack; //用于闭合和作用域的检查
    QStack<QChar>braketStack;
    QMultiMap<int,QString> multiMap;  //作用域对标识符的映射
    bool isValidIdentifier(const QString &str);


    // void checkIllegalCharacters(const QString code);
    // void checkKeywordSpelling();
     void checkUnclosedStructures();
    // void checkDuplicateDeclarations();
    // void errorRecoveryAndContinue();

    QString inputFilter(QString inpufile);
    void printTokens();  ///打印字段
    int numToken(int pos,int cur_line,QString filterSource);
    int alphaToken(int pos,int cur_line,QString filterSource);
    int seperatorToken(int pos,int cur_line,QString filterSource);
    int operatorToken(int pos,int cur_line,QString filterSource);
    bool isKeyword(const QString word); //是否是关键字
    bool isSeparator(const QChar s);  //是否是界符
    bool isOperator(const QChar s);   //是否是算数运算符
    bool isIdentifier(const QString word);  //是否是标识符
    bool isNumber(const QChar s);  //是否是数字
    bool isalpha(const QChar s);  //是否是字母
    void enterscope(QChar s); //进入作用域
    void existscope(int cur_line); //退出作用域


};



#endif // WORKLEX_H
