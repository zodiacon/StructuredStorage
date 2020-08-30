#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\StructuredStorage\CompoundFile.h"
#include "..\StructuredStorage\CompoundFileReaderWriter.h"

#ifdef _DEBUG
	#ifdef _WIN64
	#pragma comment(lib, "..\\x64\\Debug\\StructuredStorage.lib")
	#else
	#pragma comment(lib, "..\\Debug\\StructuredStorage.lib")
	#endif
#else
	#ifdef _WIN64
	#pragma comment(lib, "..\\x64\\Release\\StructuredStorage.lib")
	#else
	#pragma comment(lib, "..\\Release\\StructuredStorage.lib")
	#endif
#endif

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace StructuredStorage;

namespace StructuredStorageTests {
	TEST_CLASS(TestCreateAndOpen) {
public:

	TEST_METHOD(CreateAndOpen) {
		auto path = L"g:\\temp\\file1.bin";
		auto file = CompoundFile::Create(path);
		Assert::IsNotNull(file.get());

		auto dir = file->CreateStructuredDirectory(L"Cactus");
		Assert::IsNotNull(dir.get());

		dir->Close();
		file->Close();

		file = CompoundFile::Open(path);
		Assert::IsNotNull(file.get());

		dir = file->OpenStructuredDirectory(L"Cactus");
		Assert::IsNotNull(dir.get());
	}

	TEST_METHOD(WriteAndRead) {
		auto path = L"g:\\temp\\file1.bin";
		auto file = CompoundFile::Create(path);
		auto ssfile = file->CreateStructuredFile(L"IntValue");
		CompoundFileReaderWriter writer(*ssfile.get());
		writer.Write(0x1234abcd);

		auto vfile = file->CreateStructuredFile(L"Vector");
		std::vector<int> vec;
		for (int i = 0; i < 10; i++)
			vec.push_back(i * i);

		CompoundFileReaderWriter writer2(*vfile.get());
		writer2.Write(vec);

		{
			auto mfile = file->CreateStructuredFile(L"Map");
			std::map<std::wstring, int> m;
			m.insert(std::make_pair(L"one", 1));
			m.insert(std::make_pair(L"five", 5));
			m.insert(std::make_pair(L"ten", 10));
			m.insert(std::make_pair(L"twelve", 12));
			m.insert(std::make_pair(L"zero", 0));

			CompoundFileReaderWriter writer3(*mfile.get());
			writer3.Write(m);
		}

		vfile->Close();
		ssfile->Close();
		file->Close();

		file = CompoundFile::Open(path);
		ssfile = file->OpenStructuredFile(L"IntValue");
		CompoundFileReaderWriter reader(*ssfile.get());
		int value;
		reader.Read(value);
		Assert::IsTrue(value == 0x1234abcd);

		vfile = file->OpenStructuredFile(L"Vector");
		vec.clear();
		CompoundFileReaderWriter reader2(*vfile.get());
		reader2.Read(vec);
		Assert::IsTrue(10 == vec.size());

		auto mfile = file->OpenStructuredFile(L"Map");
		std::map<std::wstring, int> m;
		CompoundFileReaderWriter reader3(*mfile.get());
		reader3.Read(m);

		Assert::IsTrue(m.size() == 5);
		Assert::IsTrue(m[L"zero"] == 0);
		Assert::IsTrue(m[L"ten"] == 10);
		Assert::IsTrue(m[L"five"] == 5);
		Assert::IsTrue(m[L"twelve"] == 12);

	}
	};
}