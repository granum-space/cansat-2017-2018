/*
 * Message.hpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: snork
 */

#ifndef MESSAGES_HPP_
#define MESSAGES_HPP_

#include <algorithm>

#include <boost/asio/buffer.hpp>

#include "Camera.hpp"


class Message
{
public:
	typedef std::vector<boost::asio::const_buffer> buff_sequence_t;
	typedef std::back_insert_iterator<buff_sequence_t> buff_sequence_oitt;

	enum MessageType
	{
		MESSAGE_TYPE_RAW_FRAME = 0x00,
	};

	Message(MessageType type): _header({type, }) {}
	virtual ~Message() = default;

	MessageType type() const { return (MessageType)_header.message_type; }

	void fill_buffer_sequnce(std::vector<boost::asio::const_buffer> & sequence)
	{
		buff_sequence_oitt oitt = std::back_inserter(sequence);
		*(oitt++) = boost::asio::const_buffer(&_header, sizeof(_header));
		_header.message_size = sizeof(_header) + _do_fill_buffer_sequence(oitt);
	}

protected:
	virtual int32_t _do_fill_buffer_sequence(buff_sequence_oitt oitt) = 0;

private:
	struct header_t
	{
		int32_t message_size;
		int32_t message_type;
	};

	header_t _header;
};



class FrameMessage: public Message
{
public:
	FrameMessage(std::shared_ptr<Camera::Frame> framePtr)
		: Message(Message::MESSAGE_TYPE_RAW_FRAME), _framePtr(framePtr)
	{
		_header.width = _framePtr->pixFmt().width;
		_header.height = _framePtr->pixFmt().height;
		_header.v4l2_fourcc = _framePtr->pixFmt().pixelformat;
		_header.size = _framePtr->dataSize();
	}

private:
	struct header_t
	{
		int32_t width;
		int32_t height;
		int32_t v4l2_fourcc;
		int32_t size;
	};

	virtual int32_t _do_fill_buffer_sequence(buff_sequence_oitt oitt) override
	{
		*(oitt++) = boost::asio::buffer(&_header, sizeof(_header));
		*(oitt++) = boost::asio::buffer(_framePtr->data(), _framePtr->dataSize());
		return sizeof(_header) + _framePtr->dataSize();
	}

	header_t _header;
	std::shared_ptr<Camera::Frame> _framePtr;
};




#endif /* MESSAGES_HPP_ */
