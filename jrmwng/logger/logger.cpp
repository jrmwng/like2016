#include "logger.h"

int main()
{
	jrmwng::logger Logger;

	Logger.record(L"%04hX-%04hX\n", static_cast<short>(1), static_cast<short>(2), static_cast<short>(3), static_cast<short>(4));
	Logger.record(L"%g\n", 1.0F);
	Logger.record(L"%g\n", 1.0);
	Logger.record(L"%S\n", "Hello World!");
	Logger.play(
		[&](va_list pArgs)
		{
			wchar_t const *pcFormat = va_arg(pArgs, wchar_t const *);
			vwprintf_s(pcFormat, pArgs);
		});
	return 0;
}
