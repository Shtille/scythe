#ifndef __SCYTHE_LOG_STREAM_H__
#define __SCYTHE_LOG_STREAM_H__

#include "file_stream.h"
#include "common/singleton.h"
#include "common/sc_assert.h"

namespace scythe {

	//! Simple scope class for logging
	class ScopeLogStream : private FileStream {
	public:
		ScopeLogStream(const char *filename);
		virtual ~ScopeLogStream() = default;

		// Make some functions public
		using Stream::WriteLine;
		using Stream::PrintString;
	};
	
	//! Unique logging class
	template <class T>
	class UniqueLogStream : private FileStream, public Singleton<T> {
	public:
		// Make some functions public
		using Stream::WriteLine;
		using Stream::PrintString;
		using Stream::PrintLine;
		
	protected:
		UniqueLogStream()
		{
			const char* filename = T::GetFilename();
			bool opened = Open(filename, StreamAccess::kWriteText);
			SC_ASSERT(opened);
		}
		virtual ~UniqueLogStream() = default;
	};
	
	//! Common logging class
	class CommonLogStream : public UniqueLogStream<CommonLogStream> {
		friend class Singleton<CommonLogStream>;
	public:
		static const char* GetFilename()
		{
			return "log.txt";
		}
	protected:
		CommonLogStream() = default;
		virtual ~CommonLogStream() = default;
	};

	//! Error logging class
	class ErrorLogStream : public UniqueLogStream<ErrorLogStream> {
		friend class Singleton<ErrorLogStream>;
	public:
		static const char* GetFilename()
		{
			return "errorlog.txt";
		}
	protected:
		ErrorLogStream() = default;
		virtual ~ErrorLogStream() = default;
	};

} // namespace scythe

// Useful defines
#define LOG_INFO scythe::CommonLogStream::GetInstance()->PrintLine
#define LOG_ERROR scythe::ErrorLogStream::GetInstance()->PrintLine

#endif