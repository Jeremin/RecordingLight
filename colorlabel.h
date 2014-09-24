#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ColorLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ColorLabel(QWidget *parent = 0);
    virtual ~ColorLabel();

    QColor color();

signals:
    void clicked();

public slots:
    void setColor(QColor& col);

protected:
    virtual void mousePressEvent(QMouseEvent* event);

private:
    QColor m_color;
};

#endif // CLICKABLELABEL_H
