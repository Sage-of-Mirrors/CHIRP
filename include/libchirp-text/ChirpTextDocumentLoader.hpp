#pragma once
#include "libchirp-text/ChirpTextDiagnostic.hpp"
#include "libchirp-text/ChirpTextDocument.hpp"
#include <filesystem>
#include <functional>
#include <memory>

enum class IncludePathKind { Rooted, Absolute };

struct ResolvedIncludePath {
  IncludePathKind kind;
  std::filesystem::path path;
};

using IncludeResolver =
    std::function< bool(const std::filesystem::path& includingFile, const std::string& includePath,
                        ResolvedIncludePath& resolved) >;

class ChirpTextDocumentLoader {
public:
  explicit ChirpTextDocumentLoader(std::filesystem::path projectRoot = {},
                                   IncludeResolver resolver = {});

  std::shared_ptr< ChirpTextDocument > load(const std::filesystem::path& path,
                                            ChirpTextDiagnosticBag diagnostics) const;

  bool resolveIncludes(ChirpTextDocument& document, const std::filesystem::path& sourcePath,
                       ChirpTextDiagnosticBag diagnostics) const;

  const std::filesystem::path& projectRoot() const { return mProjectRoot; }

private:
  bool resolveIncludePath(const std::filesystem::path& includingFile,
                          const std::string& includePath, ResolvedIncludePath& resolved,
                          ChirpTextDiagnosticBag diagnostics) const;

  std::filesystem::path mProjectRoot;
  IncludeResolver mResolver;
};
