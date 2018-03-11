
#include <unistd.h>

#include <stddef.h>

#include <iostream>
#include <boost/asio.hpp>

#include <mavlink/granum/mavlink.h>

int main()
{
    using namespace boost;
    using udp = boost::asio::ip::udp;

    try
    {
        asio::io_service io;
        udp::socket socket(io);
        udp::resolver resolver(io);
        udp::resolver::query query(udp::v4(), "localhost", "11000");
        udp::endpoint target_endpoint = *resolver.resolve(query);
        socket.open(target_endpoint.protocol());

        for (size_t i = 0; ; i++)
        {
            uint8_t buffer[2048];
            mavlink_test_message_t test_msg;
            test_msg.field1 = 10 + i;
            test_msg.field2 = 20 + i;
            test_msg.field3 = 30 + i;
            test_msg.field4 = 40 + i;

            mavlink_message_t msg;
            mavlink_msg_test_message_encode(255, 255, &msg, &test_msg);
            auto len = mavlink_msg_to_send_buffer(buffer, &msg);
            socket.send_to(asio::buffer(&buffer, len), target_endpoint);

            std::cout <<  "sent message " << i << std::endl;
            usleep(1000*1000);
        }
    }
    catch (std::exception & e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}



