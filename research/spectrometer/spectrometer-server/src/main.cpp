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

	static int count = 10;
	if (--count == 0)
	{
		on_signal(boost::system::error_code(), SIGINT);
		return;
	}

	/*
	std::ofstream f;
	{
		std::stringstream fname_stream;
		fname_stream << "/tmp/frame_" << (10-count) << ".data";
		std::string fname = fname_stream.str();
		f.open(fname.c_str(), std::ios::out | std::ios::binary);
		if (!f)
		{
			LOG_ERROR_FREE(logger) << "не могу открыть файл для снимка: " << fname;
			return;
		}
	}

	f.write((char*)buffer->data(), buffer->dataSize());
	f.close();
	*/
}


int main()
{
	static auto logger = build_free_logger("main");

	int retcode = EXIT_SUCCESS;
	LOG_INFO_FREE(logger) << "started!";

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
			server.start(6112);

			boost::signals2::scoped_connection conn_guard_0(
				cam.onFrame.connect([](std::shared_ptr<Camera::Frame> buffer){
					on_frame(std::move(buffer));
				})
			);

			boost::signals2::scoped_connection conn_guard_1(
				cam.onFrame.connect([&server](std::shared_ptr<Camera::Frame> buffer){
					server.send_message(std::make_shared<Message>(std::move(buffer)));
				})
			);

			cam.open("/dev/video0");
			//cam.set_fmt(2592, 1944, V4L2_PIX_FMT_YUYV);
			cam.set_fmt(640, 480, V4L2_PIX_FMT_YUYV);
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

