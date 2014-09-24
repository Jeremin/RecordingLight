#include "colorlabel.h"
#include <QWidget>

ColorLabel::ColorLabel(QWidget *parent) :
    QLabel(" ", parent)
{
    QColor col = Qt::white;
    setColor(col);
    setMinimumWidth(50);
}

ColorLabel::~ColorLabel(){

}

QColor ColorLabel::color(){
    return m_color;
}

void ColorLabel::setColor(QColor& col){
    m_color = col;
    setStyleSheet("background-color: rgba(" \
                  + QString("%1,%2,%3,%4").arg(col.red()).arg(col.green()).arg(col.blue()).arg(col.alpha()) \
                  + ");");
}

void ColorLabel::mousePressEvent(QMouseEvent* event){
    Q_UNUSED(event);
    emit clicked();
}
