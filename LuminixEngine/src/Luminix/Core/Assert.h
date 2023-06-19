#pragma once

namespace Luminix
{
	enum class AssertionCaller
	{
		CoreAssertion,
		ClientAssertion
	};

	class Assertion
	{
	public:
		static void Verify(AssertionCaller caller, bool condition, const char* file, i32 line);
		static void Assert(AssertionCaller caller, bool condition, const char* error);
	};
}

#ifdef LUM_DEBUG
	#define LUM_ENABLE_ASSERTS
	#define LUM_DEBUGBREAK() __debugbreak()
#else
	#define LUM_DEBUGBREAK()
#endif

#ifdef LUM_ENABLE_ASSERTS
	#define LUM_VERIFY(x)            ::Luminix::Assertion::Verify(AssertionCaller::ClientAssertion, x, __FILE__, __LINE__)
	#define LUM_ASSERT(x, err)       ::Luminix::Assertion::Assert(AssertionCaller::ClientAssertion, x, err)
	#define LUM_CORE_VERIFY(x)       ::Luminix::Assertion::Verify(AssertionCaller::CoreAssertion, x, __FILE__, __LINE__)
	#define LUM_CORE_ASSERT(x, err)  ::Luminix::Assertion::Assert(AssertionCaller::CoreAssertion, x, err)
#else
	#define LUM_VERIFY(x) x
	#define LUM_ASSERT(x, err)
	#define LUM_CORE_VERIFY(x) x
	#define LUM_CORE_ASSERT(x, err)
#endif
