#include "myslider.h"
#include <QMouseEvent>

MySlider::MySlider(QWidget *parent)
     : QSlider(parent)
{

}

MySlider::~MySlider()
{

}

void MySlider::mousePressEvent(QMouseEvent *ev)
{
    //获取当前点击位置
    int currentX = ev->pos().x();

    //获取当前点击的位置占整个Slider的百分比
    int  value = (1.0 * currentX) / this->width() * 100;

    //利用算得的百分比得到具体数字
//    int value = per*(this->maximum() - this->minimum()) + this->minimum();

    //设定滑动条位置
    this->setValue(value);

//    printf("this->maximum() = %d, this->minimum() = %d, currentX = %d, value = %d\n",
//           this->maximum(), this->minimum() , currentX, value);

    //滑动条移动事件等事件也用到了mousePressEvent,加这句话是为了不对其产生影响，是的Slider能正常相应其他鼠标事件
    QSlider::mousePressEvent(ev);
}
