/*
 *@author: 缪庆瑞
 *@date:   2022.11.28
 *@brief:  基类行编辑框
 * 该类重新实现了mouseReleaseEvent()方法，添加了编辑信号(鼠标点击并释放)，外部不需要再对
 * 编辑框单独安装事件过滤器，即可对点击编辑操作进行处理
 */
#ifndef BASELINEEDIT_H
#define BASELINEEDIT_H

#include <QLineEdit>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QResizeEvent>


class BaseLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit BaseLineEdit(QWidget *parent = 0);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void editSig();//编辑信号

public slots:
};

#endif // BASELINEEDIT_H
