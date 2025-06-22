// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include <cassert>
#include <format>
#include <source_location>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string>

#define ENABLE_IE_ASSERT true
#ifdef ENABLE_IE_ASSERT
    #define IEAssert(Expression) (assert(Expression), Expression)
#else
    #define IEAssert(Expression) (Expression)
#endif

namespace Private
{
    static constexpr const char* ColorSpecifierReset = "\033[0m";
    static constexpr const char* ColorSpecifierRed = "\033[31m";
    static constexpr const char* ColorSpecifierGreen = "\033[32m";
    static constexpr const char* ColorSpecifierYellow = "\033[33m";
    inline void IELog(int LogLevel, const char* FuncName, const char* Format, ...)
    {
        const char* ColorCode = nullptr;
        const char* LevelString = nullptr;
        switch (LogLevel)
        {
            case -1:
            {
                ColorCode = ColorSpecifierRed;
                LevelString = "Error";
                break;
            } 
            case 0:
            {
               ColorCode = ColorSpecifierReset;
                LevelString = "Log";
                break; 
            }
            case 1:
            {
              ColorCode = ColorSpecifierGreen;
                LevelString = "Success";
                break;  
            }
            case 2:
            {
                ColorCode = ColorSpecifierYellow;
                LevelString = "Warning";
                break;
            }
            default:
            {
                break;
            }
        };
        std::printf("%sIELog %s: ", ColorCode, LevelString);
        va_list Args;
        va_start(Args, Format);
        std::vprintf(Format, Args);
        va_end(Args);
        std::printf(" [%s]%s\n", FuncName, ColorSpecifierReset);
    }
}
#define IELOG_ERROR(Format, ...)   Private::IELog(-1,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)
#define IELOG_INFO(Format, ...)    Private::IELog( 0,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)
#define IELOG_SUCCESS(Format, ...) Private::IELog( 1,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)
#define IELOG_WARNING(Format, ...) Private::IELog( 2,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)

#define ENABLE_IE_RESULT_LOGGING true
struct IEResult
{
public:  
    enum class Type : int16_t
    {
        NotSupported = -3,
        OutOfMemory = -2,
        Fail = -1,
        Unknown = 0,
        Success = 1,
        InvalidArgument = 2,
        Unimplemented = 3,
    };

public:
    IEResult() = delete;
    IEResult(const IEResult& other) = default;
    IEResult(IEResult&& other) = default;

#if ENABLE_IE_RESULT_LOGGING
    explicit IEResult(const IEResult::Type& _Type = Type::Unknown, const std::string _Message = std::string(), const std::source_location& _CallerContext = std::source_location::current())
        : Type(_Type), Message(_Message), CallerContext(_CallerContext)
    {}
#else
    explicit IEResult(const IEResult::Type& _Type, const std::string _Message = std::string())
        : Type(_Type)
    {}
#endif

bool operator==(const IEResult& OtherResult) const
{
    return this->Type == OtherResult.Type;
}

bool operator!=(const IEResult& OtherResult) const
{
    return !(*this == OtherResult);
}

operator bool() const
{
    if (static_cast<int16_t>(Type) <= 0)
    {
#if ENABLE_IE_RESULT_LOGGING
        Private::IELog(-1, CallerContext.function_name(), Message.c_str());
#endif
        abort();
    }
    else if (static_cast<int16_t>(Type) > 1)
    {
#if ENABLE_IE_RESULT_LOGGING
        Private::IELog(2, CallerContext.function_name(), Message.c_str());
#endif
        return false;
    }
    else // this->Type == IEResult::Type::Success
    {
#if ENABLE_IE_RESULT_LOGGING
        Private::IELog(1, CallerContext.function_name(), Message.c_str());
#endif
        return true;
    }
}

public:
    Type Type = Type::Unknown;
    std::string Message = {};

private:
    const std::source_location CallerContext;
};