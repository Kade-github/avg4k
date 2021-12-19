#pragma once
#include "includes.h"
class buffer_restore
{
public:
    buffer_restore(std::ios& _ios)
        : ios(_ios)
        , sbuf(_ios.rdbuf())
    { }

    ~buffer_restore()
    {
        ios.rdbuf(sbuf);
    }
private:
    std::ios& ios;
    std::streambuf* sbuf;
};