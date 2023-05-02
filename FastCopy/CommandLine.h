#pragma once
#include <vector>
#include <string_view>
#include "../FastCopyShellExtension/CopyOperation.h"
#include <winrt/base.h>

class Command
{
public:
	static Command& Get();
	winrt::hstring GetDestination();
	std::wstring RecordFile();
};
