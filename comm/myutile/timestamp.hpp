#pragma once
#include <iostream>
#include <string>
#include <time.h>
class Timestamp
{

    public:
    Timestamp(int64_t microSecondsSinceEpoch= 0)
    :microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {

    }
    static Timestamp now()
    {
        time_t t = time(NULL);
        return Timestamp(t);

    }
    std::string TimestamptoString()const
    {
        char buf[128] = {0};
        tm* tm_t = localtime(&microSecondsSinceEpoch_);
        sprintf(buf,"%4d/%02d/%02d %02d:%02d:%02d",tm_t->tm_year+1900
                        ,tm_t->tm_mon + 1,tm_t->tm_mday,tm_t->tm_hour,tm_t->tm_min,tm_t->tm_sec);
                        return buf;

    }

    private:
    int64_t microSecondsSinceEpoch_;

};