/*
 *@author:  缪庆瑞
 *@date:    2022.11.26
 *@brief:   数字小键盘
 *1.采用单例模式，利用智能指针实现资源的自动释放
 *2.使用Popup窗口标志实现点击窗口外关闭的功能,不可设置父对象
 */
#ifndef NUMBERKEYBOARD_H
#define NUMBERKEYBOARD_H

#include <QWidget>
#include <QToolButton>
#include <QLineEdit>
#include <QScopedPointer>
#include <QCloseEvent>

class NumberKeyboard : public QWidget
{
    Q_OBJECT
public:
    static NumberKeyboard * getInstance();//单例模式
    //显示输入缓存区
    void showInputBufferArea(QString inputContent=QString(),const QValidator *validator=NULL);
    //隐藏输入缓存区
    void hideInputBufferArea(QLineEdit *currLineEdit,bool autoMovePos = true);
    //计算数字小键盘显示的位置
    QPoint calcNumberKeyboardPos(QWidget *inputWidget);

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    NumberKeyboard(QWidget *parent = nullptr);//构造函数私有化
    NumberKeyboard(const NumberKeyboard &);//拷贝构造私有化,不定义
    NumberKeyboard & operator=(const NumberKeyboard &);//拷贝赋值运算符私有化，不定义

    void initCharacterBtn();//初始化符号按键
    void initSpecialBtn();//初始化特殊功能按键
    void initKeyboardWidget();//初始化数字键盘部件

    //实例对象
    static QScopedPointer<NumberKeyboard> numberKeyboardInstance;
    //数字键盘部件
    QWidget *keyboardWidget;
    //输入缓存区
    QLineEdit *inputBufferEdit;//内置编辑框
    QLineEdit *currentLineEdit;//键盘当前的(单行)输入编辑框，可以接受外面传递的指针，默认为内置的inputBufferEdit
    //12个符号按键(十个数字(0-9)、负号、小数点)
    QToolButton *characterBtn[12];
    //4个特殊功能按键
    QToolButton *deleteBtn;//删除
    QToolButton *backwardBtn;//光标后(左)移
    QToolButton *forwardBtn;//光标前(右)移
    QToolButton *okBtn;//确定

signals:
    void sendTextSig(QString text);//以信号的形式将输入缓存区文本发出去

public slots:
    void characterBtnSlot();
    void deleteBtnSlot();
    void backwardBtnSlot();
    void forwardBtnSlot();
    void okBtnSlot();

};

#endif // NUMBERKEYBOARD_H
