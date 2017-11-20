/*
 * Logable.hpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: snork
 */

#ifndef LOG_HPP_
#define LOG_HPP_

#include <istream>
#include <ostream>

#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/common.hpp>

#define __DO_LOG(level) BOOST_LOG_SEV(this->Logable<std::decay<decltype(*this)>::type>::_slg, level)

#define LOG_TRACE __DO_LOG(LogSev::TRACE)
#define LOG_DEBUG __DO_LOG(LogSev::DEBUG)
#define LOG_INFO  __DO_LOG(LogSev::INFO)
#define LOG_WARN  __DO_LOG(LogSev::WARN)
#define LOG_ERROR __DO_LOG(LogSev::ERROR)
#define LOG_CRIT  __DO_LOG(LogSev::CRIT)

#define LOG_TRACE_FREE(trgt) 	BOOST_LOG_SEV(trgt, LogSev::TRACE)
#define LOG_DEBUG_FREE(trgt) 	BOOST_LOG_SEV(trgt, LogSev::DEBUG)
#define LOG_INFO_FREE(trgt)  	BOOST_LOG_SEV(trgt, LogSev::INFO)
#define LOG_WARN_FREE(trgt)  	BOOST_LOG_SEV(trgt, LogSev::WARN)
#define LOG_ERROR_FREE(trgt) 	BOOST_LOG_SEV(trgt, LogSev::ERROR)
#define LOG_CRIT_FREE(trgt)  	BOOST_LOG_SEV(trgt, LogSev::CRIT)


enum class LogSev { TRACE, DEBUG, INFO, WARN, ERROR, CRIT };


inline std::ostream & operator << (std::ostream & stream, const LogSev & obj)
{
	switch (obj)
	{
	case LogSev::TRACE: stream << "TRACE"; break;
	case LogSev::DEBUG: stream << "DEBUG"; break;
	case LogSev::INFO: stream << "INFO"; break;
	case LogSev::WARN: stream << "WARN"; break;
	case LogSev::ERROR: stream << "ERROR"; break;
	case LogSev::CRIT: stream << "CRIT"; break;
	default:
		stream << "LogSev(" << (int)obj << ")"; break;
	};
	return stream;
}



template <typename __CRTP_DESCENDANT>
class Logable
{
public:
	Logable(const std::string & channelName)
		: _slg(boost::log::keywords::channel = channelName)
	{}

protected:
	mutable boost::log::sources::severity_channel_logger_mt<LogSev> _slg;
};


inline boost::log::sources::severity_channel_logger_mt<LogSev> build_free_logger(const std::string & channelName)
{
	return boost::log::sources::severity_channel_logger_mt<LogSev>(boost::log::keywords::channel = channelName);
}


#endif /* LOG_HPP_ */
