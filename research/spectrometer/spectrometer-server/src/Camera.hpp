/*
 * Webcam.hpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: snork
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <errno.h>
#include <sys/stat.h>

#include <cstring>
#include <string>
#include <vector>
#include <system_error>
#include <future>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <boost/signals2.hpp>

#include "log.hpp"
#include "MmapBuffer.hpp"

class Camera: private Logable<Camera>
{
public:
	class Frame
	{
	public:
		virtual ~Frame() = default;
		virtual const void * data() = 0;
		virtual size_t dataSize() = 0;
		virtual const struct v4l2_pix_format & pixFmt() = 0;
	};

public:
	Camera(boost::asio::io_service & io): ::Logable<Camera>("Camera"), _strand(io), _fd(io) {}
	~Camera() { close(); }

	void open(const std::string & path);
	void set_fmt(uint32_t xres, uint32_t yres, uint32_t v4l2_fourcc);
	void start(size_t framebuffer_queue_size = 4);
	void stop();
	void close();

	boost::signals2::signal<void(std::shared_ptr<Frame>)> onFrame;

private:
	typedef std::vector<std::shared_ptr<MmapBuffer>> BufferPool;

	void _init_device_ws();
	void _init_buffers_ws(size_t buffers_count);
	void _reset_buffers_ws();
	void _enqueue_buffer_ws(const MmapBuffer & buffer);
	void _do_read_ws();

	boost::asio::io_service::strand _strand;
	boost::asio::posix::stream_descriptor _fd;

	struct v4l2_format _fmt;
	bool _started = false;
	BufferPool _buffers_pool;
};


#endif /* CAMERA_HPP_ */
