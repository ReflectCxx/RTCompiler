#pragma once

#include <vector>
#include <string>

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

namespace clmirror
{
	class ASTParser
	{
		const std::vector<std::string>& m_files;
		clang::tooling::CompilationDatabase& m_cdb;

		ASTParser() = delete;

		llvm::Expected<clang::tooling::CommonOptionsParser> getCommonOptionsParser(int pArgc, const std::vector<char*>& pArgv);

	public:

		ASTParser(const std::vector<std::string>& pFiles, clang::tooling::CompilationDatabase& pCdb);

		const int parseFiles(const int pStartIndex, const int pEndIndex);
	};
}