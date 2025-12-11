#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<QToolBar>
#include<QVBoxLayout>
#include<QFile>
#include<QFileInfo>
#include<QDir>
#include<QFileDialog>
#include <QStringList>
#include<QDebug>
#include "worklex.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    // // 创建主窗口的中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setFixedSize(800, 600); // 设置中心部件的固定大小


    // 创建布局管理器
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

   // ui->setupUi(this);

    toolBar=new QToolBar("My Tool Bar",this);
  // addToolBar(toolBar); // 默认位置在顶部
    //增加图片
    toolBar->setIconSize(QSize(18,18));  // 设置工具栏图标大小
    open=new QAction("open",toolBar);
    open->setIcon(QIcon(":/images/images/804.bmp"));
    open->setIconText("打开文件");
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(open);

    close=new QAction("close",toolBar);
    close->setIcon(QIcon(":/images/images/104.bmp"));
    close->setIconText("另存文件");

    lex_word=new QAction("lex",toolBar);
    lex_word->setIcon(QIcon(":/images/images/133.bmp"));
    lex_word->setIconText("词法分析");
    toolBar->addAction(lex_word);
    toolBar->addAction(close);
    layout->addWidget(toolBar);


    //添加页面换的位置
    tabwidget=new QTabWidget(this);
    source=new QPlainTextEdit();
    wordAny=new QPlainTextEdit();
    tabwidget->addTab(source,"源程序");
    tabwidget->addTab(wordAny,"词法分析");
    layout->addWidget(tabwidget);
    centralWidget->setLayout(layout);  // 这里是使用 layout

    connect(open, &QAction::triggered, this, &MainWindow::onOpen); //添加打开文件函数
    connect(lex_word,&QAction::triggered,this,&MainWindow::opetatorLex);  //进行词法分析

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::onOpen()
{
    //“QFile打开”按钮
    QString curPath=QDir::currentPath();    //获取应用程序当前目录
    QString dlgTitle="打开一个文件";
    QString filter="程序文件(*.h *.cpp);;文本文件(*.txt);;所有文件(*.*)"; //文件过滤器
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);

    if (aFileName.isEmpty())
        return;

    QFileInfo  fileInfo(aFileName);
    QDir::setCurrent(fileInfo.absolutePath());//设置应用程序当前目录
    openByIO_Lines(aFileName);      //整体读取

}

bool MainWindow::openByIO_Lines(const QString &aFileName)
{    //逐行读取
    QFile   aFile;
    aFile.setFileName(aFileName);
    if (!aFile.exists())    //文件不存在
        return false;
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    //逐行读取
    source->clear();
    int count=1;
    while (!aFile.atEnd())
    {
        QByteArray line = aFile.readLine();    //读取一行ASCII码，自动添加'\0'
        QString strNumber = QString("%1   ").arg(count,3);
        QString str=QString::fromUtf8(line);   //从字节数组转换为字符串,文件必须是UTF-8编码
        sourceCode+=str;
        str.truncate(str.length()-1);          //去除增加的空行

        str.prepend(strNumber);
        source->appendPlainText(str);
        count++;
    }
    aFile.close();
    source->setReadOnly(true); //设置只读
    tabwidget->setCurrentIndex(0);
    return  true;
}
//对tab页中第一页进行词法分析
bool MainWindow::opetatorLex()
{
    //读取到第二个tab页面
    // QStringList lines = source->toPlainText().split('\n');
    // QString code;

    // // 逐行输出内容
    // for (const QString &line : lines) {

    //   QString  line_temp=line.trimmed();
    //    // qDebug()<<line_temp;
    //   int pos = line_temp.indexOf(" ");
    //     line_temp=line_temp.mid(pos);
    //   if(pos==-1)
    //         line_temp="";
    //  line_temp=line_temp.trimmed();
    //   code+=line_temp;

    //  // wordAny->appendPlainText(line_temp);
    // }
    // qDebug()<<code;
    work.analyze(sourceCode);
    int line = 0;
    for(auto e:work.getTokens())
    {
        QString temp;

        if(e.line != line)
        {
             line++;
             temp = QString::number(e.line);
        }else
        {
           // continue;
        }

//QString::number(e.line) +
       temp += "   " + e.value.leftJustified(20,' ') + work.tokenTypeToString(e.type).leftJustified(20,' ');
        wordAny->appendPlainText(temp);
    }


    tabwidget->setCurrentIndex(1);  //切换到第二个tab页


    return true;
}

