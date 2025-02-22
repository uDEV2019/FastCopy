#define _SILENCE_CXX20_CISO646_REMOVED_WARNING //needed for C++20 with Absl
#include "TestFactory.h"
#include "Process.h"
#include "TestCases.h"
#include "Env.h"
#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

#ifdef _DEBUG
#include "COMApiTest.h"
#include "XCopyTest.h"
#include "FilesystemApiTest.h"
#include "RobocopyTest.h"
#endif

/*
	The following flags are for configuring what test cases to be run via command lines.
	For example, there might be cases when you want to generate random 4K files with this tool,
	and then test the performance of Windows' built-in File Explorer.

	If no flags are specified, this program should run everything, including generating test files,
	and run all the implemented copy methods.
*/
ABSL_FLAG(bool, generate_random_files, false, "Generate 1GB of random 4K files");
ABSL_FLAG(bool, generate_big_files, false, "Generate 4GB of big files");


/*
	This is a test program for testing various ways of copying/moving files and folders in windows.
	Some of the ways includes:
		- Built-in xcopy.exe 
		- Built-in robocopy.exe
		- Windows Explorer
		- C++'s filesystem library
		- Win32 API
	The results of this program will ultimately affect what methods to be included in the FastCopy's main program.

	To write your implementation of such tests, create a new class and inherits from <ICopyBase> and AutoRegister<Self>
	For an example, look into ---> XCopyTest.h

	Test files and folders and generated in TestFactory::MakeTestPaths()
*/

/**
 * This class kills explorer.exe in constructor and then restart it in the destructor
 */
struct ExplorerGuard
{
	ExplorerGuard()
	{
		puts("Killing explorer.exe for accurate result. It will be restarted after test.\n");
		std::system("taskkill /f /im explorer.exe");
	}

	~ExplorerGuard()
	{
		Process<wchar_t> explorer{ Env::GetFolderPath(Env::SpecialFolder::Windows) + L"\\explorer.exe" };
	}
};

/**
 * If there is command line args, run what command line specified.
 */
static void RunCommandLineConfig(...)
{
	if (absl::GetFlag(FLAGS_generate_random_files))
		TestFactory{} << Random4KFiles{};
	if (absl::GetFlag(FLAGS_generate_big_files))
		TestFactory{} << BigFile{};
}


#include <wil/com.h>
#include <ShlObj_core.h>

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		//ExplorerGuard guard;
#ifdef _DEBUG
		//manually add debugging test implementation here...
		//In release build, all tests registered with AutoRegister<Self> runs

		//TestFactory::Register(std::make_unique<COMApiTest>());
		TestFactory::Register(std::make_unique<COMApiTest>());
		TestFactory::Register(std::make_unique<FilesystemApiTest>());
		TestFactory::Register(std::make_unique<RobocopyTest>());
#endif
		TestFactory{}
			//<< Random4KFiles{}
			//<< BigFile{};
		<< MoveFileSamePartition{};
		TestFactory::RunAllTest();
		TestFactory::PrintResult();
		//TestFactory::Clear();
	}
	else
	{
		RunCommandLineConfig(absl::ParseCommandLine(argc, argv));
	}
}