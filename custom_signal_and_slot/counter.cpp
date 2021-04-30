#include "counter.h"
#include <QDebug>

Counter::Counter(QObject *parent) : QObject(parent)
{


}

void Counter::setValue(int value)
{
    if (value != m_value) {
        m_value = value;
         qDebug() << "Counter::setValue" << m_value;
        emit valueChanged(value);
    }
}
