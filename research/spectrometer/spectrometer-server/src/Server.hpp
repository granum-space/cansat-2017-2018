/*
 * Server.h
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: snork
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <queue>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/v6_only.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>

#include "log.hpp"
#include "Messages.hpp"


class Server : private Logable<Server>
{
private:
	typedef boost::asio::ip::tcp::acceptor acceptor_t;
	typedef boost::asio::ip::tcp::socket socket_t;
	typedef boost::asio::io_service::strand strand_t;

	struct Session { socket_t socket; };

	typedef std::list<std::shared_ptr<Session>> session_list_t;

public:
	Server(boost::asio::io_service & io)
		: ::Logable<Server>("Server"), _acceptor(io), _socket(io), _strand(io)
	{}


	~Server()
	{
		Server::stop();

		// Импровизированный псевдо спинлок
		// Не умираем пока все сессии не умрут
		enum TheState { OKAY, AGAIN };

	again:
		auto promise = std::promise<TheState>();
		auto future = promise.get_future();

		_strand.dispatch([&, this](){
			if (_sessions.size() == 0)
				promise.set_value(TheState::OKAY);
			else
				promise.set_value(TheState::AGAIN);
		});

		TheState res = future.get();
		if (TheState::AGAIN == res)
			goto again;
	}


	void start(int32_t port, const std::string iface="0.0.0.0")
	{
		using namespace boost;

		auto promise = std::promise<void>();
		auto future = promise.get_future();

		_strand.dispatch([&, this](){
			try
			{
				auto ep = asio::ip::tcp::endpoint(asio::ip::address::from_string(iface),port);
				_acceptor.open(ep.protocol());
				if (ep.protocol() == asio::ip::tcp::v6())
					_acceptor.set_option(asio::ip::v6_only(false));

				_acceptor.bind(ep);

				_acceptor.listen();
				_do_accept_ws();
				promise.set_value();
			}
			catch (...)
			{
				promise.set_exception(std::current_exception());
			}
		});

		future.get();
	}


	void send_message(std::shared_ptr<Message> msg)
	{
		_strand.dispatch([this, msg](){

			if (_current_message)
			{
				LOG_INFO << "Клиент не готов, сбрасываю сообщение";
				return;
			}
			else
			{
				_current_message = std::move(msg);
				_current_message_buffers.clear();
				_current_message->fill_buffer_sequnce(_current_message_buffers);
				_do_chain_ws(_sessions.begin());
			}
		});
	}


	void stop()
	{
		using namespace boost;

		auto promise = std::promise<void>();
		auto future = promise.get_future();

		_strand.dispatch([&, this](){

			boost::system::error_code err;
			_acceptor.close(err);
			if (err)
				LOG_WARN << "Что-то нехорошее на отмене приёма новых соединений: " << err << ":" << err.message();

			for (auto sessionPtr : _sessions)
				sessionPtr->socket.close();

			if (_current_message) // Если никакое сообщение не стоит сейчас на отправку - просто удаляем все сокеты сами
				_sessions.clear();
			promise.set_value();
		});

		future.get();
	}


private:
	void _do_accept_ws()
	{
		using namespace boost;

		assert(_strand.running_in_this_thread());

		_acceptor.async_accept(_socket, _strand.wrap([this](const system::error_code & err){
			if (err == asio::error::operation_aborted)
				return;
			else if (err)
			{
				LOG_ERROR << "Ошибка на приеме соединения: " << err << ":" << err.message();
				_do_accept_ws();
				return;
			}

			auto session = std::make_shared<Session>( Session{std::move(_socket)} );

//			// Запускаем чтение, чтобы ловить EOF и вежливо закрывать соединение с клиентами, которые отключились сами
//			session->socket.async_read_some(asio::null_buffers(), _strand.wrap([this, session](const system::error_code & err, size_t){
//				// впринципе не важно что мы тут поймали, закрываем сокет - вежливо или уже как получится
//				// всеравно нам никто ничего не должно отправлять
//
//				LOG_INFO << "Что-то пришло от клиента. err = " << err << ":" << err.message() << ". Закрываю и удаляю сессию";
//				session->socket.close();
//				_sessions.remove(session);
//				// удаляем сокет из списка
//			}));

			LOG_INFO << "Новый клиент: " << session->socket.remote_endpoint();
			_sessions.emplace_front(std::move(session));
			_do_accept_ws();
		}));
	}


	void _do_chain_ws(session_list_t::iterator current)
	{
		using namespace boost;
		assert(_strand.running_in_this_thread());

		if (current == _sessions.end())
		{
			_current_message.reset();
			return;
		}

		asio::async_write((*current)->socket, _current_message_buffers,
			_strand.wrap([this, current](const system::error_code & err, size_t transferred) {
				assert(_strand.running_in_this_thread());

				decltype(current) next;

				if (err == asio::error::operation_aborted)
				{
					// очень некрасивое решение, но в целом пойдет
					_sessions.clear();
					return;
				}
				else if (err)
				{
					// Что-то пошло не так, выкидываем текущего клиента
					LOG_ERROR << "Ошибка на передаче данных к клиенту: " << err << ":" << err.message();

					system::error_code socket_close_err;
					(*current)->socket.close(socket_close_err);
					if (socket_close_err)
						LOG_WARN << "Какая-то ошибка с закрытием сокета : "  << socket_close_err
							<< " : " << socket_close_err.message();

					next = _sessions.erase(current);
				}
				else
					next = std::next(current, 1);

				// Если ошибки нет, просто переходим к следующему
				_do_chain_ws(next);
			})
		);
	}


	acceptor_t _acceptor;
	socket_t _socket;
	strand_t _strand;
	session_list_t _sessions;
	std::shared_ptr<Message> _current_message;
	Message::buff_sequence_t _current_message_buffers;
};




#endif /* SERVER_HPP_ */
