#pragma once

#include "stdlibs.h"

#include <boost/any.hpp>

namespace Common
{

class BadAnyCastException : public std::runtime_error
{
public:
	BadAnyCastException(const std::string& err)
		: std::runtime_error(err)
	{
	}
};

class Any
{
public:
	Any()
		: m_placeHolder(nullptr)
	{
	}

	template <typename T,
		typename = std::enable_if_t<!std::is_same<T, Any>::value>
	>
	Any(T&& value)
		: m_placeHolder(new Holder<std::remove_cv_t<std::decay_t<T>>>(static_cast<T&&>(value)))
	{
	}

	Any(const Any& other)
		: m_placeHolder(other.m_placeHolder ? other.m_placeHolder->clone() : nullptr)
	{
	}

	Any(Any&& other)
		: m_placeHolder(other.m_placeHolder)
	{
		other.m_placeHolder = nullptr;
	}

	~Any()
	{
		delete m_placeHolder;
	}

	Any& operator=(const Any& other)
	{
		Any(other).swap(*this);
		return *this;
	}

	Any& operator=(Any&& other)
	{
		other.swap(*this);
		Any().swap(other);
		return *this;
	}

	template <typename T>
	Any& operator=(T&& object)
	{
		Any(static_cast<T&&>(object)).swap(*this);
		return *this;
	}

	const type_info& type() const noexcept
	{
		return m_placeHolder ? m_placeHolder->type() : typeid(void);
	}

	Any& swap(Any& other) noexcept
	{
		std::swap(m_placeHolder, other.m_placeHolder);
		return *this;
	}

	template <typename T>
	friend T* anyCast(const Any* anyObject)
	{
		if (m_placeHolder->type() != typeid(T))
		{
			throw BadAnyCastException("Bad AnyObject cast");
		}

		return static_cast<Any::Holder<T>*>(anyObject->m_placeHolder)->value;
	}	
	
	template <typename T>
	friend T* anyCast(const Any* anyObject, std::nothrow_t)
	{
		return m_placeHolder->type() == typeid(T) ? 
			static_cast<Any::Holder<T>*>(anyObject->m_placeHolder)->value : nullptr;
	}

private:

	struct PlaceHolder
	{
		virtual ~PlaceHolder() noexcept = default;

		virtual const type_info& type() const noexcept = 0;

		virtual PlaceHolder* clone() const = 0;
	};

	template <typename T>
	struct Holder : public PlaceHolder
	{
		//
		// support move and ctor semantic for type T
		//
		Holder(T&& val) noexcept
			: value(static_cast<T&&>(val))
		{
		}

		virtual const type_info& type() const noexcept override
		{
			return typeid(T);
		}

		virtual PlaceHolder* clone() const override
		{
			return new Holder<T>(value);
		}
	
		T value;
	};

private:
	PlaceHolder* m_placeHolder;
};

}
