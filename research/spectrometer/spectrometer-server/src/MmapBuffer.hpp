/*
 * MmapBuffer.hpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: snork
 */

#ifndef MMAPBUFFER_HPP_
#define MMAPBUFFER_HPP_

#include <sys/mman.h>
#include <system_error>

#include <linux/videodev2.h>

#include "log.hpp"

class MmapBuffer: private Logable<MmapBuffer>
{
public:
	typedef decltype(v4l2_buffer().m.offset) offset_t;
	typedef decltype(v4l2_buffer().length) length_t;

	MmapBuffer(): Logable<MmapBuffer>("MapBuffer") {}

	~MmapBuffer () {
		try {
			unmap();
		} catch (std::exception & e) {
			LOG_ERROR << "Ошибка в деструкторе MmapBuffer: \"" << e.what() << "\"" << std::endl;
		}
	}


	MmapBuffer(const MmapBuffer & other) = delete;
	MmapBuffer & operator=(const MmapBuffer & other) = delete;


	MmapBuffer(MmapBuffer && other)
		: Logable<MmapBuffer>("MapBuffer"), _ptr(other._ptr), _len(other._len), _index(other._index)
	{
		other._ptr = nullptr;
		other._len = 0;
		other._index = -1;
	}


	MmapBuffer & operator=(MmapBuffer && other)
	{
		_ptr = std::move(other._ptr);
		_len = std::move(other._len);
		_index = std::move(other._index);

		other._ptr = nullptr;
		other._len = 0;
		other._index = -1;

		return *this;
	}


	void map(int fd, int index, offset_t offset, length_t len)
	{
		unmap();

		_ptr = mmap(NULL /* start anywhere */,
			len,
			PROT_READ | PROT_WRITE /* required */,
			MAP_SHARED /* recommended */,
			fd, offset
		);

		if (MAP_FAILED == _ptr)
			throw std::system_error(errno, std::system_category(), " MMAP_FAILED");

		_len = len;
		_index = index;
	}


	void unmap()
	{
		if (!_ptr)
			return;

		if (0 > munmap(_ptr, _len))
			throw std::system_error(errno, std::system_category(), "не могу выполнить munamap!");

		_ptr = nullptr;
		_len = 0;
		_index = -1;
	}


	int index() const { return _index; }
	void * ptr() const { return _ptr; }
	length_t len() const { return _len; }


	friend void swap(MmapBuffer & left, MmapBuffer & right)
	{
		std::swap(left._ptr, right._ptr);
		std::swap(left._len, right._len);
	}

private:
	void * _ptr = nullptr;
	length_t _len = 0;
	int _index = -1;
};




#endif /* MMAPBUFFER_HPP_ */
