/*
 * Message.hpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: snork
 */

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <boost/asio/buffer.hpp>

#include "Camera.hpp"

class Message
{
public:
	constexpr static int buffers_per_msg = 2;
	typedef std::array<boost::asio::const_buffer, buffers_per_msg>  buffers_sequence_t;


	Message(std::shared_ptr<Camera::Frame> framePtr)
		: _framePtr(framePtr)
	{
		_header.width = _framePtr->pixFmt().width;
		_header.height = _framePtr->pixFmt().height;
		_header.v4l2_fourcc = _framePtr->pixFmt().pixelformat;
		_header.bytes = _framePtr->dataSize();

		_to_buffers(_buffers_seq.begin());
	}

	const buffers_sequence_t & buffers_seq() { return _buffers_seq; }

private:
	struct header_t
	{
		uint32_t width;
		uint32_t height;
		uint32_t v4l2_fourcc;
		uint32_t bytes;
	};

	//typedef std::vector<boost::asio::const_buffer>::iterator OUTPUT_ITT_T;
	template <typename OUTPUT_ITT_T>
	void _to_buffers(OUTPUT_ITT_T oitt)
	{
		using namespace boost;
		*oitt++ = asio::buffer(&_header, sizeof(_header));
		*oitt++ = asio::buffer(_framePtr->data(), _framePtr->dataSize());
	}

	header_t _header;
	std::shared_ptr<Camera::Frame> _framePtr;
	buffers_sequence_t _buffers_seq;
};




#endif /* MESSAGE_HPP_ */
