#include "widget.h"
#include "ui_widget.h"
#include "numberkeyboard.h"
#include <QDebug>
#include <QTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->lineEdit,SIGNAL(editSig()),this,SLOT(lineeditSlot()));
    connect(ui->lineEdit_2,SIGNAL(editSig()),this,SLOT(lineeditSlot()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::lineeditSlot()
{
    NumberKeyboard *numberKeyBoard = NumberKeyboard::getInstance();
    QLineEdit * tmpLineEdit = qobject_cast<QLineEdit *>(sender());

    //带缓存区的使用方法
    if(tmpLineEdit == ui->lineEdit)
    {
        connect(numberKeyBoard,&NumberKeyboard::sendTextSig,[this](QString text){
            ui->lineEdit->setText(text);
        });
        numberKeyBoard->showInputBufferArea(ui->lineEdit->text(),ui->lineEdit->validator());
        numberKeyBoard->move(numberKeyBoard->calcNumberKeyboardPos(ui->lineEdit));
        numberKeyBoard->show();
    }
    //不带缓存区的使用方法
    else if(tmpLineEdit == ui->lineEdit_2)
    {
        numberKeyBoard->hideInputBufferArea(ui->lineEdit_2);
        numberKeyBoard->show();
    }
}


