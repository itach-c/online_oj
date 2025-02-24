#pragma once
class nocopy 
{

    public:
    nocopy(const nocopy& obj) = delete;
    void operator=(const nocopy & obj) = delete;
    nocopy() = default;
   ~nocopy() = default; 

};