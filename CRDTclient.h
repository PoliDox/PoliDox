#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H


class CRDTclient
{
public:
    CRDTclient();

    void localInsert(int position, char character);
};

#endif // CLIENTCRDT_H
