#include "libchirp-text/ChirpTextDocumentLoader.hpp"
#include "libchirp-text/ChirpTextLexer.hpp"
#include "libchirp-text/ChirpTextParser.hpp"
#include <fstream>
#include <sstream>
// TODO: Rewrite to use athena instead

ChirpTextDocumentLoader::ChirpTextDocumentLoader(std::filesystem::path projectRoot,
                                                 IncludeResolver resolver)
: mProjectRoot(std::filesystem::absolute(std::move(projectRoot))), mResolver(std::move(resolver)) {
  if (mProjectRoot.empty())
    mProjectRoot = std::filesystem::current_path();
}

std::shared_ptr< ChirpTextDocument >
ChirpTextDocumentLoader::load(const std::filesystem::path& path,
                              ChirpTextDiagnosticBag diagnostics) const {
  std::error_code ec;
  const auto absolutePath = std::filesystem::absolute(path, ec);
  const auto actualPath = ec ? path : absolutePath;

  std::ifstream file(actualPath);
  if (!file) {
    diagnostics.error({{actualPath.string(), 1, 1}, {actualPath.string(), 1, 1}},
                      "unable to open source file");
    return {};
  }

  std::ostringstream contents;
  contents << file.rdbuf();

  ChirpTextLexer lexer(contents.str(), actualPath.string());
  auto tokens = lexer.tokenize(diagnostics);
  if (diagnostics.hasErrors())
    return {};

  ChirpTextParser parser(std::move(tokens));
  auto document = std::make_shared< ChirpTextDocument >(parser.parse(diagnostics));
  if (!diagnostics.hasErrors())
    resolveIncludes(*document, actualPath, diagnostics);

  return document;
}

bool ChirpTextDocumentLoader::resolveIncludePath(const std::filesystem::path& includingFile,
                                                 const std::string& includePath,
                                                 ResolvedIncludePath& resolved,
                                                 ChirpTextDiagnosticBag diagnostics) const {
  // 1. An explicit absolute filesystem path.
  const std::filesystem::path requested(includePath);
  if (requested.is_absolute()) {
    if (std::filesystem::exists(requested)) {
      resolved = {IncludePathKind::Absolute, requested};
      return true;
    }

    diagnostics.error({{includingFile.string(), 1, 1}, {includingFile.string(), 1, 1}},
                      "absolute include path does not exist: " + includePath);
    return false;
  }

  // 2. '$/...' means project-root relative.
  if (!includePath.empty() && includePath[0] == '$') {
    if (includePath.size() == 1 || (includePath.size() >= 2 && includePath[1] != '/')) {
      diagnostics.error({{includingFile.string(), 1, 1}, {includingFile.string(), 1, 1}},
                        "invalid '$' include path; expected '$/path': " + includePath);
      return false;
    }

    const auto relative = std::filesystem::path(includePath.substr(2));
    const auto candidate = mProjectRoot / relative;

    if (std::filesystem::exists(candidate)) {
      resolved = {IncludePathKind::Rooted, candidate};
      return true;
    }

    diagnostics.error({{includingFile.string(), 1, 1}, {includingFile.string(), 1, 1}},
                      "project-root include path does not exist: " + includePath);
    return false;
  }

  // 3. Ordinary relative paths are not part of the include syntax.
  diagnostics.error(
      {{includingFile.string(), 1, 1}, {includingFile.string(), 1, 1}},
      "invalid include path; expected an absolute path or a '$/...' project-root path: " +
          includePath);
  return false;
}

bool ChirpTextDocumentLoader::resolveIncludes(ChirpTextDocument& document,
                                              const std::filesystem::path& sourcePath,
                                              ChirpTextDiagnosticBag diagnostics) const {
  for (auto& include : document.mIncludes) {
    ResolvedIncludePath resolved;

    const std::filesystem::path requested(include.mPath);
    const bool absolute = requested.is_absolute();
    const bool rooted =
        include.mPath.size() >= 2 && include.mPath[0] == '$' && include.mPath[1] == '/';

    // Even a custom resolver receives only syntactically valid include
    // forms. It may change where those forms are backed, but it cannot
    // introduce a third include syntax.
    if (!absolute && !rooted) {
      diagnostics.error(
          include.mSpan,
          "invalid include path; expected an absolute path or a '$/...' project-root path: " +
              include.mPath);
      return false;
    }

    if (mResolver) {
      if (!mResolver(sourcePath, include.mPath, resolved)) {
        diagnostics.error(include.mSpan, "custom include resolver failed for: " + include.mPath);
        return false;
      }
    } else if (!resolveIncludePath(sourcePath, include.mPath, resolved, diagnostics)) {
      diagnostics.error(include.mSpan, "failed to resolve include: " + include.mPath);
      return false;
    }

    include.mResolved = load(resolved.path, diagnostics);
    if (!include.mResolved)
      return false;
  }

  return true;
}
