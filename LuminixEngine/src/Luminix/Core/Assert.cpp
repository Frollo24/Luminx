#include "lumpch.h"
#include "Assert.h"

#include "Log.h"

namespace Luminix
{
	void Assertion::Verify(AssertionCaller caller, bool condition, const char* file, i32 line)
	{
		if (!condition)
		{
			switch (caller)
			{
				case AssertionCaller::CoreAssertion:
					LUM_CORE_ERROR("Verification Failed at File: {0}; Line: {1}", file, line);
					break;
				case AssertionCaller::ClientAssertion:
					LUM_ERROR("Verification Failed at File: {0}; Line: {1}", file, line);
					break;
				default:
					break;
			}

			LUM_DEBUGBREAK();
		}
	}

	void Assertion::Assert(AssertionCaller caller, bool condition, const char* error)
	{
		if (!condition)
		{
			switch (caller)
			{
				case AssertionCaller::CoreAssertion:
					LUM_CORE_ERROR("Assertion Failed: {0}", error);
					break;
				case AssertionCaller::ClientAssertion:
					LUM_ERROR("Assertion Failed: {0}", error);
					break;
				default:
					break;
			}

			LUM_DEBUGBREAK();
		}
	}
}