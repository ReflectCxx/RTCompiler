#include "ASTParser.h"

#include <mutex>
#include <iostream>
#include <filesystem>

#include "Logger.h"
#include "Constants.h"
#include "clang-tidy/ClangTidyDiagnosticConsumer.h"
#include "ClangReflectDiagnosticConsumer.h"
#include "ClangReflectActionFactory.h"

using namespace llvm;
using namespace clang;
using namespace clang::tidy;
using namespace clang::tooling;

namespace
{
	std::mutex g_mutex;
	static cl::OptionCategory toolCategory(clmirror::CLANG_MIRROR);
	static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
	static cl::extrahelp MoreHelp("\nMore help text...\n");
}


namespace clmirror
{
	ASTParser::ASTParser(const std::vector<std::string>& pFiles, CompilationDatabase& pCdb)
		: m_files(pFiles)
		, m_cdb(pCdb)
	{ }

	Expected<CommonOptionsParser> ASTParser::getCommonOptionsParser(int pArgc, const std::vector<char*>& pArgv)
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		return CommonOptionsParser::create(pArgc, (const char**)(&pArgv[0]), toolCategory);
	}

	const int ASTParser::parseFiles(const int pStartIndex, const int pEndIndex)
	{
		for (size_t index = pStartIndex; index <= pEndIndex; index++)
		{
			int argc = 2;
			std::vector<char*> argv;
			const auto& srcFilePath = m_files.at(index).c_str();

			Logger::outProgress("compiling: " + std::string(srcFilePath));

			if (!std::filesystem::exists(srcFilePath)) {
				Logger::outProgress(srcFilePath + std::string(". File not found..!"), false);
				continue;
			}

			argv.push_back(const_cast<char*>(CLANG_MIRROR));
			argv.push_back(const_cast<char*>(srcFilePath));
			auto commonOptionParser = getCommonOptionsParser(argc, argv);

			if (!commonOptionParser) {
				llvm::errs() << commonOptionParser.takeError();
				Logger::out("\tUnexpected error occured. Aborting..!");
				return 1;
			}

			ClangTidyOptions overrideOpts;
			overrideOpts.HeaderFilterRegex = ".*";
			overrideOpts.ExtraArgs = ClangTidyOptions::ArgList();
			
			auto customOptsProvider = std::make_unique<DefaultOptionsProvider>(ClangTidyGlobalOptions(), overrideOpts);
			if (!customOptsProvider) {
				return -1;
			}

			ClangTool clangTool(m_cdb, { srcFilePath });
			//ClangTool Tool(Compilations, InputFiles,
			//	std::make_shared<PCHContainerOperations>(), BaseFS);

			ClangTidyContext context(std::move(customOptsProvider));
			ClangReflectDiagnosticConsumer diagConsumer(context);
			DiagnosticOptions diagOpts;
			DiagnosticsEngine diagEngine(new DiagnosticIDs(), diagOpts, &diagConsumer, false);
			
			context.setDiagnosticsEngine(std::make_unique<DiagnosticOptions>(diagOpts), &diagEngine);
			clangTool.setDiagnosticConsumer(&diagConsumer);
			//clangTool.appendArgumentsAdjuster(getArgumentsAdjuster());

			auto actionFactory = std::unique_ptr<ActionFactory>(new ActionFactory(context));
			clangTool.run(actionFactory.get());

			auto unreflectedFuncs = actionFactory->getUnreflectedFunctions();
			auto missingHeaderErrors = diagConsumer.getMissingHeaderMsgs();
			Logger::outReflectError(srcFilePath, unreflectedFuncs, missingHeaderErrors);
		}
		return 0;
	}
}