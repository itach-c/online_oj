#include "../comm/include/socket.h"
#include "../comm/include/httpResponse.hpp"
#include "../comm/myutile/log.hpp"
class HttpClinet
{

public:
    HttpClinet(const std::string &ip, uint16_t port)
    {
        bool ret = cli_sock_.CreateClient(ip, port);
        if (ret)
        {
            LOG_INFO("connect ip: %s port: %u compile server success", ip.c_str(), port)
        }
    }
    HttpResPonse Post(const std::string &path, const std::string &message, const std::string &content_type)
    {
        //http 请求行构建
        std::string req = "Post " + path + " " + "HTTP/1.1\r\n";
        req += "Content-type: " + content_type +"\r\n";
        req += "Content-Length: " + std::to_string(message.size()) + "\r\n\r\n";
        cli_sock.Send(req.c_str(), req.size());
        cli_sock.Send(body.c_str(),body.size());

    }

private:
    Socket cli_sock_;
};
*