#ifndef NETWORKCHECK_H
#define NETWORKCHECK_H

#include <QObject>
#include <QTcpSocket>

class network: public QObject
{
    Q_OBJECT

private:

    QTcpSocket* sock;

public:

    network(QObject*parent=0):QObject(parent)
    {
        sock = new QTcpSocket(this);
    }

    bool check();
};

#endif // NETWORKCHECK_H
