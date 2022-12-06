/*
 *@author: 缪庆瑞
 *@date:   2022.11.28
 *@brief:  基类行编辑框
 */
#include "baselineedit.h"
#include <QDebug>


BaseLineEdit::BaseLineEdit(QWidget *parent)
    : QLineEdit(parent)
{

}
/*
 *@brief:   鼠标释放事件处理
 *@author:  缪庆瑞
 *@date:    2022.11.28
 *@param:   event:鼠标事件
 */
void BaseLineEdit::mouseReleaseEvent(QMouseEvent *event)
{
    //左键释放&&非只读模式
    if(event->button() == Qt::LeftButton && !this->isReadOnly())
    {
        emit editSig();//发射编辑信号
    }
    QLineEdit::mouseReleaseEvent(event);
}
