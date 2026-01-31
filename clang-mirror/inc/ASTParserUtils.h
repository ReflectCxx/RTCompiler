
#pragma once

#include "clang/Tooling/Tooling.h"
#include "clang-tidy/ClangTidyDiagnosticConsumer.h"

namespace
{
    static llvm::cl::OptionCategory ClangMirrorCategory("clang-mirror options");

    static llvm::cl::desc desc(llvm::StringRef Description) { return { Description.ltrim() }; }

    static llvm::cl::opt<std::string> VfsOverlay("vfsoverlay",
                                      desc(R"(Overlay the virtual filesystem described by file over the real file system.)"),
                                      llvm::cl::value_desc("filename"), llvm::cl::cat(ClangMirrorCategory));

    static std::unique_ptr<clang::tidy::ClangTidyOptionsProvider> createOptionsProvider(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS)
    {
        clang::tidy::ClangTidyOptions DefaultOptions;
        clang::tidy::ClangTidyOptions OverrideOptions;
        clang::tidy::ClangTidyGlobalOptions GlobalOptions;
        
        return std::make_unique<clang::tidy::FileOptionsProvider>(std::move(GlobalOptions), std::move(DefaultOptions),
                                                                  std::move(OverrideOptions), std::move(FS));
    }

    static llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> getVfsFromFile(const std::string& OverlayFile,
                                                                          llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> BaseFS)
    {
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> Buffer = BaseFS->getBufferForFile(OverlayFile);

        if (!Buffer) {
            llvm::errs() << "Can't load virtual filesystem overlay file '"
                         << OverlayFile << "': " << Buffer.getError().message() << ".\n";
            return nullptr;
        }
        
        llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS = llvm::vfs::getVFSFromYAML(std::move(Buffer.get()), nullptr, OverlayFile);
        if (!FS) {
            llvm::errs() << "Error: invalid virtual filesystem overlay file '" << OverlayFile << "'.\n";
            return nullptr;
        }
        return FS;
    }
    
    static llvm::IntrusiveRefCntPtr<llvm::vfs::OverlayFileSystem> createBaseFS()
    {
        llvm::IntrusiveRefCntPtr<llvm::vfs::OverlayFileSystem> BaseFS(new llvm::vfs::OverlayFileSystem(llvm::vfs::getRealFileSystem()));
        if (!VfsOverlay.empty())
        {
            llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> VfsFromFile = getVfsFromFile(VfsOverlay, BaseFS);
            if (!VfsFromFile) {
                return nullptr;
            }
            BaseFS->pushOverlay(std::move(VfsFromFile));
        }
        return BaseFS;
    }
}