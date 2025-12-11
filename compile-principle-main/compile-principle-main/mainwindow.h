#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPlainTextEdit>
#include "worklex.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QToolBar *toolBar;

    QAction *open;
    QAction *close;
    QAction *lex_word;

    QPlainTextEdit *source;
    QPlainTextEdit *wordAny;

    QTabWidget *tabwidget;
    bool openByIO_Lines(const QString &aFileName);
    QString sourceCode;
    WorkLex work;
    bool opetatorLex();

private slots:
    void onOpen();


};
#endif // MAINWINDOW_H
