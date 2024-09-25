#pragma once

#include "App/Libs/WinInclude.h"
#include "MDS/Tools/RLog.h"

#include <string_view>
#include <format>
#define PREPROCESSOR_TO_STRING(x) PREPROCESSOR_TO_STRING_INNER(x)
#define PREPROCESSOR_TO_STRING_INNER(x) #x
#define L_PATH __FILE__ "(" PREPROCESSOR_TO_STRING(__LINE__) "):\n"

class RSRLog
{
public:
	enum class ESeverity : UINT8
	{
		Display = 0,
		Warning = 1,
		Error = 2,
		Exception = 3
	};
public:
	RSRLog(const RSRLog&) = delete;
	RSRLog& operator=(const RSRLog&) = delete;

private:
	RSRLog() = default;

public:
	inline static void LogDisplay	(std::string_view InDisplayText)	{ Log(RSRLog::ESeverity::Display,	InDisplayText); };
	inline static void LogWarning	(std::string_view InWarningText)	{ Log(RSRLog::ESeverity::Warning,	InWarningText); };
	inline static void LogError		(std::string_view InErrorText)		{ Log(RSRLog::ESeverity::Error,		InErrorText); };
	inline static void LogException	(std::string_view InExceptionText)	{ Log(RSRLog::ESeverity::Exception,	InExceptionText); };

	inline static void LogDisplay(std::string_view InDisplayText, HRESULT InHresult)		{ Log(RSRLog::ESeverity::Display, InDisplayText, InHresult); };
	inline static void LogWarning(std::string_view InWarningText, HRESULT InHresult)		{ Log(RSRLog::ESeverity::Warning, InWarningText, InHresult); };
	inline static void LogError(std::string_view InErrorText, HRESULT InHresult)			{ Log(RSRLog::ESeverity::Error, InErrorText, InHresult); };
	inline static void LogException(std::string_view InExceptionText, HRESULT InHresult)	{ Log(RSRLog::ESeverity::Exception, InExceptionText, InHresult); };
public:
	static void Log(ESeverity EInSeverity, const std::string_view InDisplayError);
	static void Log(ESeverity EInSeverity, HRESULT InHresult);
	static void Log(ESeverity EInSeverity, const std::string_view InDisplayError, HRESULT InHresult);

public:
	template<typename... Args >
	void static Log(ESeverity EInSeverity, std::format_string<Args...> InConstexpFormatText, Args&&... InArgs)
	{
		RSRLog::Log(EInSeverity, std::format(InConstexpFormatText, std::forward<Args>(InArgs)...));
	};
	template<typename... Args>
	void static Log(ESeverity EInSeverity, HRESULT result, std::format_string<Args...> InConstexpFormatText, Args&&... InArgs)
	{
		RSRLog::Log(EInSeverity, std::format(InConstexpFormatText, std::forward<Args>(InArgs)...), result);
	};

};
constexpr RSRLog::ESeverity LOG_DISPLAY = RSRLog::ESeverity::Display;
constexpr RSRLog::ESeverity LOG_WARNING = RSRLog::ESeverity::Warning;
constexpr RSRLog::ESeverity LOG_ERROR = RSRLog::ESeverity::Error;
constexpr RSRLog::ESeverity LOG_EXCEPTION = RSRLog::ESeverity::Exception;
const std::string& ToString(RSRLog::ESeverity EInSeverity);