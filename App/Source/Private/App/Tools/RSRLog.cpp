#include "App/Tools/RSRLog.h"
#include <iostream>

static const std::string ESEVERITY_DISPLAY = TEXT("Display");
static const std::string ESEVERITY_WARNING = TEXT("Warning");
static const std::string ESEVERITY_ERROR = TEXT("Error");
static const std::string ESEVERITY_EXCEPTION = TEXT("Exception");
static const std::string ESEVERITY_UNKNOWN = TEXT("Unknown");

using std::cin, std::cout, std::endl;

void RSRLog::Log(ESeverity EInSeverity, const std::string_view InDisplayError)
{
    cout << "[" << ToString(EInSeverity) << "] : " << InDisplayError << endl;
}

void RSRLog::Log(ESeverity EInSeverity, HRESULT InHresult)
{
    std::string hError = std::system_category().message(InHresult);
    cout << "[" << ToString(EInSeverity) << "] : '" << hError << "'" << endl;
}

void RSRLog::Log(ESeverity EInSeverity, const std::string_view InDisplayError, HRESULT InHresult)
{
    std::string hError = std::system_category().message(InHresult);
    cout << "[" << ToString(EInSeverity) << "] : '" << InDisplayError << "' with error code '" << hError << "'" << endl;
}

const std::string& ToString(RSRLog::ESeverity EInSeverity)
{
    switch (EInSeverity)
    {
    case RSRLog::ESeverity::Display:
        return ESEVERITY_DISPLAY;
    case RSRLog::ESeverity::Warning:
        return ESEVERITY_WARNING;
    case RSRLog::ESeverity::Error:
        return ESEVERITY_ERROR;
    case RSRLog::ESeverity::Exception:
        return ESEVERITY_EXCEPTION;
    default:
        cout << "[UnimplExcep] Case not handled in switch of type 'RSRLog::ESeverity' in RSRLog.cpp" << endl;
        return ESEVERITY_UNKNOWN;
    }
}
