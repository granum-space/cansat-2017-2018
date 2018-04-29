#include "Camera.hpp"


void Camera::open(const std::string & path)
{
	auto promise = std::promise<void>();
	auto future = promise.get_future();

	_strand.dispatch([this, &promise, &path](){
		try
		{
			if (_fd.is_open())
				throw std::runtime_error("устройство уже открыто");

			struct stat st;
			if (-1 == stat(path.c_str(), &st))
				throw std::system_error(errno, std::system_category(),
						path + " ошибка при вызове stat");

			if (!S_ISCHR(st.st_mode))
				throw std::system_error(errno, std::system_category(),
						path + " не устройство");

			int fd = ::open(path.c_str(), O_RDWR | O_NONBLOCK, 0);
			if (-1 == fd)
				throw std::system_error(errno, std::system_category(),
						"не могу открыть устройство \"" + path + "\"");

			_fd.assign(fd);

			// Запрашиваем возможности камеры
			struct v4l2_capability cap;

			if (-1 == ioctl(_fd.native(), VIDIOC_QUERYCAP, &cap))
				throw std::system_error(errno, std::system_category(), "не могу выполнить ioctl VIDIOC_QUERYCAP");

			if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
				throw std::runtime_error("это устройство не умеет делать V4L2_CAP_VIDEO_CAPTURE");

			if (!(cap.capabilities & V4L2_CAP_STREAMING))
				throw std::runtime_error("это устройство не умеет делать V4L2_CAP_STREAMING");

			promise.set_value();
		}
		catch (...)
		{
			promise.set_exception(std::current_exception());
		}
	});

	future.get();
}


void Camera::set_fmt(uint32_t xres, uint32_t yres, uint32_t v4l2_fourcc)
{
	auto promise = std::promise<void>();
	auto future = promise.get_future();

	_strand.dispatch([&, this](){
		try
		{
			memset(&_fmt, 0x00, sizeof(_fmt));
			_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == ioctl(_fd.native(), VIDIOC_G_FMT, &_fmt))
				throw std::system_error(errno, std::system_category(), "не могу получить текущий формат");

			// Устанавливаем нужный формат
			_fmt.fmt.pix.width = xres;
			_fmt.fmt.pix.height = yres;
			_fmt.fmt.pix.pixelformat = v4l2_fourcc;
			_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
			if (-1 == ioctl(_fd.native(), VIDIOC_S_FMT, &_fmt))
				throw std::system_error(errno, std::system_category(), "не могу установить нужный формат");

			promise.set_value();
		}
		catch(...)
		{
			promise.set_exception(std::current_exception());
		}
	});

	future.get();
}


void Camera::start(size_t framebuffer_queue_size /*= 4*/)
{
	auto promise = std::promise<void>();
	auto future = promise.get_future();

	_strand.dispatch([&, this](){
		try
		{
			if (_started)
				stop();

			_reset_buffers_ws();
			_init_buffers_ws(framebuffer_queue_size);

			// передаем наши буфера в очередь на запись
			for (const auto & buffer : _buffers_pool)
				_enqueue_buffer_ws(*buffer, true);

			enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == ioctl(_fd.native(), VIDIOC_STREAMON, &type))
				throw std::system_error(errno, std::system_category(), "ioctl VIDIOC_STREAMON");

			_started = true;
			_do_read_ws();
			promise.set_value();
		}
		catch (...)
		{
			promise.set_exception(std::current_exception());
		}
	});

	future.get();
}


void Camera::stop()
{
	using namespace boost;

	auto promise = std::promise<void>();
	auto future = promise.get_future();

	_strand.dispatch([this, &promise](){
		try
		{
			if (_started)
			{
				if (!_fd.is_open())
					throw std::runtime_error("устройство не открыто");

				_fd.cancel();

				enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				if (-1 == ioctl(_fd.native(), VIDIOC_STREAMOFF, &type))
					throw std::system_error(errno, std::system_category(), "ioctl VIDIOC_STREAMOFF");
			}

			_started = false;
			promise.set_value();
		}
		catch (...)
		{
			promise.set_exception(std::current_exception());
		}
	});

	future.get();
}


void Camera::close()
{
	using namespace boost;

	auto promise = std::promise<void>();
	auto future = promise.get_future();

	_strand.dispatch([this, &promise](){
		try
		{
			if (_started)
				stop();

			if (_fd.is_open())
			{
				_fd.close();
			}

			promise.set_value();
		}
		catch (...)
		{
			promise.set_exception(std::current_exception());
		}
	});

	future.get();
}


void Camera::_reset_buffers_ws()
{
	assert(_strand.running_in_this_thread());

	if (_buffers_pool.empty())
		return;

	struct v4l2_requestbuffers req;
	std::memset(&req, 0x00, sizeof(req));
	req.count = 0;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(_fd.native(), VIDIOC_REQBUFS, &req))
		throw std::system_error(errno, std::system_category(), "ошибка в ioctl VIDIOC_REQBUFS reset_buffers_ws");

	_buffers_pool.clear();
}


void Camera::_init_buffers_ws(size_t buffers_count)
{
	assert(_strand.running_in_this_thread());

	struct v4l2_requestbuffers req;
	std::memset(&req, 0x00, sizeof(req));
	req.count = buffers_count;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(_fd.native(), VIDIOC_REQBUFS, &req))
		throw std::system_error(errno, std::system_category(), "ошибка в ioctl VIDIOC_REQBUFS init_buffers_ws");

	for (size_t i = 0; i < req.count; i++) {
		struct v4l2_buffer buf;
		std::memset(&buf, 0x00, sizeof(buf));
		buf.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory		= V4L2_MEMORY_MMAP;
		buf.index		= i;

		if (-1 == ioctl(_fd.native(), VIDIOC_QUERYBUF, &buf))
			throw std::system_error(errno, std::system_category(),
					"не могу выполнить ioctl на VIDIOC_REQBUFS. buf.index=" + std::to_string(i)
			);

		auto buffer = std::make_shared<MmapBuffer>();
		buffer->map(_fd.native(), buf.index, buf.m.offset, buf.length);
		_buffers_pool.push_back(buffer);
	}
}


void Camera::_enqueue_buffer_ws(const MmapBuffer & buffer, bool force /*= false*/)
{
	assert(_strand.running_in_this_thread());

	if (!force && !_started)
		return;

	struct v4l2_buffer buf;
	std::memset(&buf, 0x00, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = buffer.index();

	if (-1 == ioctl(_fd.native(), VIDIOC_QBUF, &buf))
		throw std::system_error(errno, std::system_category(), "ioctl VIDIOC_QBUF");
}


void Camera::_do_read_ws()
{
	assert(_strand.running_in_this_thread());
	using namespace boost;

	_fd.async_read_some(asio::null_buffers(), _strand.wrap([this](const system::error_code & err, size_t readed){
		if (err == asio::error::operation_aborted)
			return;
		else if (err)
		{
			LOG_ERROR << "Ошибка при async_read_some: " << err << ":" << err.message();
			stop();
			return;
		}

		struct v4l2_buffer buf;
		std::memset(&buf, 0x00, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if (-1 == ioctl(_fd.native(), VIDIOC_DQBUF, &buf))
		{
			auto err = std::error_code(errno, std::system_category());
			LOG_ERROR << "Ошибка при ioctl VIDIOC_QBUF: " << err << ":" << err.message();
			stop();
			return;
		}

		class TheFrame: public Frame
		{
		public:
			typedef std::shared_ptr<MmapBuffer> target_t;

			TheFrame(Camera * parent, target_t target, const struct v4l2_pix_format & fmt)
				: _parent(parent), _target(target), _fmt(fmt)
			{}

			~TheFrame()
			{
				std::weak_ptr<MmapBuffer> localtarget = std::move(this->_target);
				Camera * localparent = std::move(this->_parent);

				localparent->_strand.dispatch([localtarget, localparent](){

					// Странное место. Нам нужно понимать умер ли уже класс родитель или нет
					// вешать на него shared_from_this не хочется

					// Так же, возможна ситуация, когда этот самый буфер уже деалоцирован и никому не нужен
					// а мы его сейчас отправим на повторное использование.
					// возможно следует внести в структуру пула еще одно поле с пометкой "expired"
					// но для начала попробуем работать со слабым поинтером. Если буфером не владеет никто
					// кроме нас, то он никому не нужен. Верно ведь?
					if (auto locked_target = localtarget.lock())
						localparent->_enqueue_buffer_ws(*locked_target);
				});
			}

			virtual const void * data() override	{ return _target->ptr(); }
			virtual size_t dataSize() override		{ return _target->len(); }
			virtual const struct v4l2_pix_format & pixFmt() override { return _fmt; }
		private:
			Camera * _parent;
			target_t _target;
			struct v4l2_pix_format _fmt;
		};

		assert(buf.index < _buffers_pool.size());

		auto frame = std::make_shared<TheFrame>(this, _buffers_pool[buf.index], std::cref(_fmt.fmt.pix));
		onFrame(std::move(frame));
		_strand.dispatch([this](){ _do_read_ws(); });
	}));
}
