/*
 *@author:  缪庆瑞
 *@date:    2022.11.26
 *@brief:   数字小键盘
 */
#include "numberkeyboard.h"
#include <QBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

//数字小键盘样式
#define KEYBOARD_WIDGET_STYLE ".QWidget{border-radius:4px;background-color:#ccced3;}\
QLineEdit{border-radius:4px;}\
QToolButton{font-size:16pt;border-radius:4px;background-color:white;}\
QToolButton:pressed{background-color:#f2f2f2;padding:3px 0px 0px 3px;}"
//静态实例
QScopedPointer<NumberKeyboard> NumberKeyboard::numberKeyboardInstance;
/*
 *@brief:  单例模式(获取唯一实例)
 *因为ui界面只在单线程使用，实例初始化未加锁
 *@date:   2022.11.29
 *@return: 实例指针
 */
NumberKeyboard *NumberKeyboard::getInstance()
{
    if(numberKeyboardInstance.isNull())
    {
        numberKeyboardInstance.reset(new NumberKeyboard());
    }
    return numberKeyboardInstance.data();
}
/*
 *@brief:  显示输入缓存区
 *输入内容临时存放到内置编辑框内，点击确认后通过信号的形式将整体内容发送出去
 *@date:   2022.11.30
 *@param:  inputContent:输入缓存区的初始内容
 *@param:  validator:内置输入缓存部件验证器，默认为0无验证
 */
void NumberKeyboard::showInputBufferArea(QString inputContent, const QValidator *validator)
{
    //设置推荐大小
    this->resize(312,280);
    //显示输入缓存区
    inputBufferEdit->setVisible(true);
    inputBufferEdit->setText(inputContent);
    inputBufferEdit->setValidator(validator);
    inputBufferEdit->selectAll();//默认全选，方便替换/删除
    //将当前(单行)输入编辑框设置为内置编辑框
    currentLineEdit = inputBufferEdit;
    currentLineEdit->setFocus();
}
/*
 *@brief:  隐藏输入缓存区
 *@date:   2022.11.30
 *@param:  currLineEdit:外部的(单行)编辑框指针
 *@param:  autoMovePos:自动计算并调整窗口位置，默认为true
 */
void NumberKeyboard::hideInputBufferArea(QLineEdit *currLineEdit, bool autoMovePos)
{
    //设置推荐大小
    this->resize(312,260);
    //设置输入缓存区隐藏
    inputBufferEdit->setVisible(false);
    //将当前(单行)输入编辑框设置为外部指针
    currentLineEdit = currLineEdit;
    currentLineEdit->setFocus();
    if(autoMovePos)
    {
        this->move(calcNumberKeyboardPos(currentLineEdit));
    }
}
/*
 *@brief:  计算数字小键盘的显示位置
 *根据输入控件和小键盘的位置大小，自动计算出小键盘最合理的显示位置(尽可能的不遮挡输入控件)
 *@date:   2022.11.30
 *@param:  inputWidget:对应的输入控件
 *@return: QPoint:小键盘窗口左顶点的全局位置(相对于屏幕)
 */
QPoint NumberKeyboard::calcNumberKeyboardPos(QWidget *inputWidget)
{
    QPoint global_pos = inputWidget->mapToGlobal(QPoint(0,0));
    /*窗口默认以输入控件为基准水平居中*/
    int move_x = global_pos.x() + inputWidget->width()/2 - this->width()/2;
    //如果左侧空间不足则输入控件左对齐
    if(move_x < 0)
    {
        move_x = global_pos.x();
    }
    //如果右侧空间不足则输入控件右对齐
    else if(move_x+this->width() > QApplication::desktop()->width())
    {
        move_x = global_pos.x()+inputWidget->width()-this->width();
    }
    /*窗口默认在输入控件的下方*/
    int move_y = global_pos.y() + inputWidget->height() + 2;//带2个像素的间隔
    //如果输入控件下方的空间不足以容纳键盘，在放在输入控件上面(前提是键盘高度 < (屏幕高度+输入控件高度)/2)
    if(move_y + this->height() > QApplication::desktop()->height())
    {
        move_y = global_pos.y() - this->height() - 2;//带2个像素的间隔
    }
    return QPoint(move_x,move_y);
}
/*
 *@brief:  关闭事件处理函数
 *主要在窗口关闭后做一些重置与清理操作，避免下一次使用时发生异常
 *@date:   2022.11.30
 *@param:  e:关闭事件
 */
void NumberKeyboard::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);
    //重置回默认状态
    setParent(NULL,Qt::FramelessWindowHint|Qt::Popup);//恢复为孤立窗口(避免父对象管理内存与智能指针冲突)
    showInputBufferArea();//显示输入缓存区
    disconnect(this,0,0,0);//断开键盘所有的信号与槽连接
}
/*
 *@brief:  私有构造
 *@date:   2022.11.26
 */
NumberKeyboard::NumberKeyboard(QWidget *parent) : QWidget(parent)
{
    //设置相关属性标志
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    //this->setAttribute(Qt::WA_ShowWithoutActivating);
    this->setFocusPolicy(Qt::NoFocus);
    this->setAttribute(Qt::WA_TranslucentBackground);

    //初始化布局
    initKeyboardWidget();
    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setMargin(0);
    vBoxLayout->addWidget(keyboardWidget);
    //默认显示输入缓存区
    showInputBufferArea();
}
/*
 *@brief:  初始化符号按键
 *@date:   2022.11.26
 */
void NumberKeyboard::initCharacterBtn()
{
    //12个符号文本显示
    QStringList charTextList;
    charTextList<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"-"<<".";
    //以下12个按键，仅作为普通输入，无其他功能，所以连接一个槽函数
    for(int i=0;i<12;i++)
    {
        characterBtn[i] = new QToolButton();
        characterBtn[i]->setToolButtonStyle(Qt::ToolButtonTextOnly);
        characterBtn[i]->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        characterBtn[i]->setText(charTextList.at(i));
        characterBtn[i]->setAutoRepeatDelay(300);
        characterBtn[i]->setAutoRepeatInterval(60);
        characterBtn[i]->setAutoRepeat(true);//启用自动重复功能，实现长按连续动作
        connect(characterBtn[i],SIGNAL(clicked(bool)),this,SLOT(characterBtnSlot()));
    }
}
/*
 *@brief:  初始化特殊功能按键
 *@date:   2022.11.26
 */
void NumberKeyboard::initSpecialBtn()
{
    //删除
    deleteBtn = new QToolButton();
    deleteBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    deleteBtn->setIcon(QIcon(QPixmap(":/images/delete.png")));
    deleteBtn->setIconSize(QSize(24,24));
    deleteBtn->setAutoRepeatDelay(300);
    deleteBtn->setAutoRepeatInterval(60);
    deleteBtn->setAutoRepeat(true);//启用自动重复功能，实现长按连续动作
    connect(deleteBtn,SIGNAL(clicked(bool)),this,SLOT(deleteBtnSlot()));
    //光标后(左)移
    backwardBtn = new QToolButton();
    backwardBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    backwardBtn->setIcon(QIcon(QPixmap(":/images/backward.png")));
    backwardBtn->setIconSize(QSize(24,24));
    backwardBtn->setAutoRepeatDelay(300);
    backwardBtn->setAutoRepeatInterval(60);
    backwardBtn->setAutoRepeat(true);//启用自动重复功能，实现长按连续动作
    connect(backwardBtn,SIGNAL(clicked(bool)),this,SLOT(backwardBtnSlot()));
    //光标前(右)移
    forwardBtn = new QToolButton();
    forwardBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    forwardBtn->setIcon(QIcon(QPixmap(":/images/forward.png")));
    forwardBtn->setIconSize(QSize(24,24));
    forwardBtn->setAutoRepeatDelay(300);
    forwardBtn->setAutoRepeatInterval(60);
    forwardBtn->setAutoRepeat(true);//启用自动重复功能，实现长按连续动作
    connect(forwardBtn,SIGNAL(clicked(bool)),this,SLOT(forwardBtnSlot()));
    //确认
    okBtn = new QToolButton();
    okBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    okBtn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    okBtn->setText("OK");
    connect(okBtn,SIGNAL(clicked(bool)),this,SLOT(okBtnSlot()));
}
/*
 *@brief:  初始化数字键盘部件
 *@date:   2022.11.26
 */
void NumberKeyboard::initKeyboardWidget()
{
    //初始化按键
    initCharacterBtn();
    initSpecialBtn();
    //初始化输入缓存区
    inputBufferEdit = new QLineEdit();
    inputBufferEdit->setMaximumHeight(60);
    inputBufferEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    currentLineEdit = inputBufferEdit;//当前(单行)输入编辑框，默认为内置编辑框

    //部件布局
    keyboardWidget = new QWidget();
    keyboardWidget->setStyleSheet(KEYBOARD_WIDGET_STYLE);
    QGridLayout *gridLayout = new QGridLayout(keyboardWidget);
    gridLayout->setMargin(8);
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setVerticalSpacing(8);

    gridLayout->addWidget(inputBufferEdit,0,0,1,4);

    gridLayout->addWidget(characterBtn[1],1,0,1,1);
    gridLayout->addWidget(characterBtn[2],1,1,1,1);
    gridLayout->addWidget(characterBtn[3],1,2,1,1);
    gridLayout->addWidget(deleteBtn,1,3,1,1);

    gridLayout->addWidget(characterBtn[4],2,0,1,1);
    gridLayout->addWidget(characterBtn[5],2,1,1,1);
    gridLayout->addWidget(characterBtn[6],2,2,1,1);
    gridLayout->addWidget(characterBtn[10],2,3,1,1);

    gridLayout->addWidget(characterBtn[7],3,0,1,1);
    gridLayout->addWidget(characterBtn[8],3,1,1,1);
    gridLayout->addWidget(characterBtn[9],3,2,1,1);
    gridLayout->addWidget(characterBtn[11],3,3,1,1);

    gridLayout->addWidget(backwardBtn,4,0,1,1);
    gridLayout->addWidget(characterBtn[0],4,1,1,1);
    gridLayout->addWidget(forwardBtn,4,2,1,1);
    gridLayout->addWidget(okBtn,4,3,1,1);
}
/*
 *@brief:  符号按键响应槽
 *@date:   2022.11.26
 */
void NumberKeyboard::characterBtnSlot()
{
    //获取信号发送者的对象
    QToolButton *clickedBtn = qobject_cast<QToolButton *>(sender());
    currentLineEdit->insert(clickedBtn->text());
}
/*
 *@brief:  删除按键响应槽
 *@date:   2022.11.26
 */
void NumberKeyboard::deleteBtnSlot()
{
    currentLineEdit->backspace();
}
/*
 *@brief:  光标后(左)移按键响应槽
 *@date:   2022.11.26
 */
void NumberKeyboard::backwardBtnSlot()
{
    currentLineEdit->cursorBackward(false);
}
/*
 *@brief:  光标前(右)移按键响应槽
 *@date:   2022.11.26
 */
void NumberKeyboard::forwardBtnSlot()
{
    currentLineEdit->cursorForward(false);
}
/*
 *@brief:  确认按键响应槽
 *@date:   2022.11.26
 */
void NumberKeyboard::okBtnSlot()
{
    //输入缓存区显示时，将缓存的内容通过信号发送出去
    if(inputBufferEdit->isVisible())
    {
        emit sendTextSig(inputBufferEdit->text());
    }
    this->close();
}
