/*
 * main.cpp
 *
 *  Created on: 18 нояб. 2017 г.
 *      Author: snork
 */

#include <linux/videodev2.h>

#include <sys/ioctl.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <thread>
#include <array>
#include <mutex>
#include <condition_variable>
#include <sstream>

#include <boost/asio/signal_set.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/program_options.hpp>

#include "Camera.hpp"
#include "Server.hpp"
#include "log.hpp"

std::condition_variable __end_cv;
std::mutex __end_mtx;
bool __is_finished = false;


void on_signal(const boost::system::error_code & err, int signal)
{
	static auto logger = build_free_logger("of_handler");
	std::unique_lock<std::mutex> lock(__end_mtx);

	LOG_INFO_FREE(logger) << "got signal: " << signal;
	__is_finished = true;
	__end_cv.notify_all();
}


void on_frame(std::shared_ptr<Camera::Frame> buffer)
{
	static auto logger = build_free_logger("of_handler");

	static std::chrono::steady_clock::time_point prev = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
	size_t delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current-prev).count() ;
	LOG_DEBUG_FREE(logger) << "GOT FRAME!"
			<< " dur = " << delta_ms << " ms. "
			<< " framerate = " << 1.0/(delta_ms/1000.0) << " fps. "
	;

	prev = current;
	return;
}


int main(int argc, char ** argv)
{
	static auto logger = build_free_logger("main");

	int retcode = EXIT_SUCCESS;
	LOG_INFO_FREE(logger) << "started!";

	namespace po = boost::program_options;

	std::string iface = "0.0.0.0";
	int32_t port;
	int32_t width = 1280, height = 1720;

	po::options_description descr;
	descr.add_options()
			("iface,i",
					po::value(&iface)->default_value(iface),
					"сетевой интерфейс на котором будет запущен сервер")

			("port,p",
					po::value(&port)->required(),
					"порт на котором будет запущен сервер")

			("width,w",
					po::value(&width)->default_value(width),
					"ширина снимка в пикселях")

			("height,h",
					po::value(&height)->default_value(height),
					"высота снимка в пикселях")
			;

	try
	{


		po::variables_map vm;
		auto parsed = po::parse_command_line(argc, argv, descr);
		po::store(parsed, vm, true);
		po::notify(vm);
	}
	catch (std::exception & e)
	{
		LOG_INFO_FREE(logger) << "Не могу определиться с опциями: " << e.what();
		LOG_INFO_FREE(logger) << "меня следует запускать так: " << std::endl
				<< descr;
		return 1;
	}

	using namespace boost;
	std::array<std::thread, 1> io_threads;
	asio::io_service io;
	auto io_work = std::make_shared<asio::io_service::work>(std::ref(io));
	for (auto & thread : io_threads)
		thread = std::thread([&io](){ io.run(); });

	{
		Server server(io);
		Camera cam(io);

		try
		{
			server.start(port, iface);

			boost::signals2::scoped_connection conn_guard_0(
				cam.onFrame.connect([](std::shared_ptr<Camera::Frame> buffer){
					on_frame(std::move(buffer));
				})
			);

			boost::signals2::scoped_connection conn_guard_1(
				cam.onFrame.connect([&server](std::shared_ptr<Camera::Frame> buffer){
					server.send_message(std::make_shared<FrameMessage>(std::move(buffer)));
				})
			);

			cam.open("/dev/video0");
			cam.set_fmt(width, height, V4L2_PIX_FMT_YUYV);
			cam.start(4);

			boost::asio::signal_set set(io);
			set.add(SIGINT);
			set.async_wait([](const boost::system::error_code & err, int signal){ on_signal(err, signal);});

			{
				std::unique_lock<std::mutex> lock(__end_mtx);
				__end_cv.wait(lock, [](){return __is_finished; });
			}

			//cam.stop();
			cam.close();
			server.stop();

			LOG_DEBUG_FREE(logger) << "no error";
		}
		catch (std::exception & e)
		{
			LOG_ERROR_FREE(logger) << "ERROR: " << e.what();
			retcode = EXIT_FAILURE;
		}
	}

	io_work.reset();
	for (auto & thread: io_threads)
		thread.join();

	return retcode;
}

