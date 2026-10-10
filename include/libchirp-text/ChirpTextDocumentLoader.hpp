#pragma once
#include "libchirp/ChirpDiagnostic.hpp"
#include "libchirp-text/ChirpTextDocument.hpp"
#include <filesystem>
#include <functional>
#include <memory>

namespace chirp::text {
enum class ChirpTextIncludePathKind { Rooted, Absolute };

struct ResolvedIncludePath {
  ChirpTextIncludePathKind kind;
  std::filesystem::path path;
};

using ChirpTextIncludeResolver = std::function<bool(const std::filesystem::path& includingFile, const std::string& includePath, ResolvedIncludePath& resolved)>;

class ChirpTextDocumentLoader {
public:
  explicit ChirpTextDocumentLoader(const std::filesystem::path& projectRoot = {}, ChirpTextIncludeResolver resolver = {});

  [[nodiscard]] std::shared_ptr<ChirpTextDocument> Load(const std::filesystem::path& path, ChirpDiagnosticBag diagnostics) const;

  bool ResolveIncludes(ChirpTextDocument& document, const std::filesystem::path& sourcePath, ChirpDiagnosticBag diagnostics) const;
  [[nodiscard]] const std::filesystem::path& ProjectRoot() const { return mProjectRoot; }

private:
  bool ResolveIncludePath(const std::filesystem::path& includingFile, const std::string& includePath, ResolvedIncludePath& resolved, ChirpDiagnosticBag diagnostics) const;

  std::filesystem::path mProjectRoot;
  ChirpTextIncludeResolver mResolver;
};
} // namespace chirp::text
