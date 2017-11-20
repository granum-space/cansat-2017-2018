/*
 * Session.hpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: snork
 */

#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <boost/asio/ip/tcp.hpp>

#include "Camera.hpp"

class Session: public std::enable_shared_from_this<Session>
{
public:
	typedef boost::asio::ip::tcp::socket socket_t;

	Session(socket_t socket_)
		: socket(std::move(socket_))
	{
		using namespace boost;
		socket.set_option(socket_t::send_buffer_size(10*1000*1024));
	}

	socket_t socket;
};



#endif /* SESSION_HPP_ */
