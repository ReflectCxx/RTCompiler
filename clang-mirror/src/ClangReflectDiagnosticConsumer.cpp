
#include "ClangReflectDiagnosticConsumer.h"
#include "Logger.h"
#include "clang/Basic/SourceManager.h"

namespace clmirror {

	using namespace clang;
	using namespace clang::tidy;

	ClangReflectDiagnosticConsumer::ClangReflectDiagnosticConsumer(ClangTidyContext& pContext)
		: clang::tidy::ClangTidyDiagnosticConsumer(pContext)
	{ }

	const std::vector<ErrorTuple>& ClangReflectDiagnosticConsumer::getMissingHeaderMsgs() {
		return m_errors;
	}

	void ClangReflectDiagnosticConsumer::HandleDiagnostic(DiagnosticsEngine::Level pDiagLevel, const Diagnostic& pInfo)
	{
		if (pInfo.hasSourceManager() && pInfo.getLocation().isValid())
		{
			const SourceManager& srcManager = pInfo.getSourceManager();
			if (!srcManager.isInSystemHeader(pInfo.getLocation()) && !srcManager.isInSystemMacro(pInfo.getLocation()))
			{
				if (pDiagLevel == DiagnosticsEngine::Level::Fatal)
				{
					const SourceLocation& errLoc = pInfo.getLocation();
					const SourceManager& srcManager = pInfo.getSourceManager();
					StringRef FileName = srcManager.getFilename(errLoc);
					unsigned LineNumber = srcManager.getSpellingLineNumber(errLoc);
					unsigned ColumnNumber = srcManager.getSpellingColumnNumber(errLoc);
					SmallString<128> MessageStr;
					pInfo.FormatDiagnostic(MessageStr);
					const std::string& lineInfo = "(" + std::to_string(LineNumber) + "," + std::to_string(ColumnNumber) + ")";
					m_errors.push_back(std::make_tuple(FileName.str(), lineInfo, MessageStr.str().str()));
				}
			}
		}
	}
}