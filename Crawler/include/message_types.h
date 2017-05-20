#pragma once

#include "stdlibs.h"
#include "url.h"

namespace CrawlerImpl
{

class IMessage
{
public:
	enum class MessageType
	{
		Url,
		DNSError,
		CurrentQueueSize,
		QueueItersAndRefsInvalidated,
		HttpResponseCode,
		WarningType,
		DuplicatedTitle,
		DuplicatedDescription,
		EmptyTitle,
		EmptyDescription
	};

	virtual MessageType type() const noexcept = 0;
};

class WarningMessage : public IMessage
{
public:
	WarningMessage(const std::string& warning)
		: m_warning(warning)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::WarningType;
	}

	const std::string& warning() const noexcept
	{
		return m_warning;
	}

private:
	std::string m_warning;
};

class HttpResponseCodeMessage : public IMessage
{
public:
	HttpResponseCodeMessage(int code)
		: m_codeResponse(code)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::HttpResponseCode;
	}

	int code() const noexcept
	{
		return m_codeResponse;
	}

private:
	int m_codeResponse;
};

class QueueItersAndRefsInvalidatedMessage : public IMessage
{
public:
	QueueItersAndRefsInvalidatedMessage(int queueType)
		: m_queueType(queueType)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::QueueItersAndRefsInvalidated;
	}

	int queueType() const noexcept
	{
		return m_queueType;
	}

private:
	int m_queueType;
};

class QueueSizeMessage : public IMessage
{
public:
	QueueSizeMessage(int queueType, std::size_t size)
		: m_queueType(queueType)
		, m_size(size)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::CurrentQueueSize;
	}

	int queueType() const noexcept
	{
		return m_queueType;
	}

	std::size_t size() const noexcept
	{
		return m_size;
	}

private:
	int m_queueType;
	std::size_t m_size;
};

class UrlMessage : public IMessage
{
public:
	UrlMessage(const std::string& url, 
		const std::string& title, 
		const std::string& description,
		const std::string& charset,
		int responseCode, 
		int queueType)
		: m_url(url)
		, m_title(title)
		, m_description(description)
		, m_charset(charset)
		, m_responseCode(responseCode)
		, m_queueType(queueType)
	{
	}

	const std::string& url() const noexcept
	{
		return m_url;
	}

	const std::string& title() const noexcept
	{
		return m_title;
	}

	const std::string& description() const noexcept
	{
		return m_description;
	}

	const std::string& charset() const noexcept
	{
		return m_charset;
	}

	int responseCode() const noexcept
	{
		return m_responseCode;
	}

	int queueType() const noexcept
	{
		return m_queueType;
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::Url;
	}

private:
	std::string m_url;
	std::string m_title;
	std::string m_description;
	std::string m_charset;
	int m_responseCode;
	int m_queueType;
};

class DNSErrorMessage : public IMessage
{
public:
	virtual MessageType type() const noexcept override
	{
		return MessageType::DNSError;
	}
};

class DuplicatedTitleMessage : public IMessage
{
public:
	DuplicatedTitleMessage(const std::string& url, const std::string& title)
		: m_url(url)
		, m_title(title)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::DuplicatedTitle;
	}

	const std::string& url() const noexcept
	{
		return m_url;
	}

	const std::string& title() const noexcept
	{
		return m_title;
	}

private:
	std::string m_title;
	std::string m_url;
};

class DuplicatedDescriptionMessage : public IMessage
{
public:
	DuplicatedDescriptionMessage(const std::string& url, const std::string& description)
		: m_url(url)
		, m_description(description)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::DuplicatedDescription;
	}

	const std::string& url() const noexcept
	{
		return m_url;
	}

	const std::string& description() const noexcept
	{
		return m_description;
	}

private:
	std::string m_description;
	std::string m_url;
};

class EmptyTitleMessage : public IMessage
{
public:
	EmptyTitleMessage(const std::string& url)
		: m_url(url)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::EmptyTitle;
	}

	const std::string& url() const noexcept
	{
		return m_url;
	}

private:
	std::string m_url;
};

class EmptyDescriptionMessage : public IMessage
{
public:
	EmptyDescriptionMessage(const std::string& url)
		: m_url(url)
	{
	}

	virtual MessageType type() const noexcept override
	{
		return MessageType::EmptyDescription;
	}

	const std::string& url() const noexcept
	{
		return m_url;
	}

private:
	std::string m_url;
};

}
