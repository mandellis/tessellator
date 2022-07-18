#include "networkcheck.h"

#include <QTcpSocket>

//! ------------------------------------
//! function: check
//! details:  check internet connection
//! ------------------------------------
bool network::check()
{
    //! try connecting google
    sock->connectToHost("www.google.com", 80);
    bool connected = sock->waitForConnected(30000);//ms
    if (!connected)
    {
        sock->abort();
        return false;
    }
    sock->close();

    //! try connecting solvosrl
    sock->connectToHost("www.solvosrl.com", 80);
    connected = sock->waitForConnected(30000);//ms
    if (!connected)
    {
        sock->abort();
        return false;
    }

    //! send IP, date, time
    return true;
}
