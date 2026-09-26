#include "libchirp-text/ChirpTextDocumentLoader.hpp"
#include "libchirp-text/ChirpTextLexer.hpp"
#include "libchirp-text/ChirpTextParser.hpp"
#include <fstream>
#include <sstream>
// TODO: Rewrite to use athena instead

ChirpTextDocumentLoader::ChirpTextDocumentLoader(const std::filesystem::path& projectRoot, IncludeResolver resolver)
: mProjectRoot(std::filesystem::absolute(projectRoot))
, mResolver(std::move(resolver)) {
  if (mProjectRoot.empty()) {
    mProjectRoot = std::filesystem::current_path();
  }
}

std::shared_ptr<ChirpTextDocument> ChirpTextDocumentLoader::load(const std::filesystem::path& path, ChirpTextDiagnosticBag diagnostics) const {
  std::error_code ec;
  const auto absolutePath = std::filesystem::absolute(path, ec);
  const auto actualPath = ec ? path : absolutePath;

  std::ifstream file(actualPath);
  if (!file) {
    diagnostics.error(
        {
            .mBegin =
                {
                    .mFile = actualPath.string(),
                    .mLine = 1,
                    .mColumn = 1,
                },
            .mEnd =
                {
                    .mFile = actualPath.string(),
                    .mLine = 1,
                    .mColumn = 1,
                },
        },
        "unable to open source file");
    return {};
  }

  std::ostringstream contents;
  contents << file.rdbuf();

  ChirpTextLexer lexer(contents.str(), actualPath.string());
  auto tokens = lexer.tokenize(diagnostics);
  if (diagnostics.hasErrors()) {
    return {};
  }

  ChirpTextParser parser(std::move(tokens));
  auto document = std::make_shared<ChirpTextDocument>(parser.parse(diagnostics));
  if (!diagnostics.hasErrors()) {
    resolveIncludes(*document, actualPath, diagnostics);
  }

  return document;
}

bool ChirpTextDocumentLoader::resolveIncludePath(const std::filesystem::path& includingFile, const std::string& includePath, ResolvedIncludePath& resolved, ChirpTextDiagnosticBag diagnostics) const {
  // An explicit absolute filesystem path.
  if (const std::filesystem::path requested(includePath); requested.is_absolute()) {
    if (std::filesystem::exists(requested)) {
      resolved = {
          .kind = IncludePathKind::Absolute,
          .path = requested,
      };
      return true;
    }

    diagnostics.error(
        {
            .mBegin =
                {
                    .mFile = includingFile.string(),
                    .mLine = 1,
                    .mColumn = 1,
                },
            .mEnd =
                {
                    .mFile = includingFile.string(),
                    .mLine = 1,
                    .mColumn = 1,
                },
        },
        std::format("absolute include path does not exist: {}", includePath));
    return false;
  }

  // '$/...' means project-root relative.
  if (!includePath.empty() && includePath[0] == '$') {
    if (includePath.size() == 1 || (includePath.size() >= 2 && includePath[1] != '/')) {
      diagnostics.error(
          {
              .mBegin =
                  {
                      .mFile = includingFile.string(),
                      .mLine = 1,
                      .mColumn = 1,
                  },
              .mEnd =
                  {
                      .mFile = includingFile.string(),
                      .mLine = 1,
                      .mColumn = 1,
                  },
          },
          std::format("invalid '$' include path; expected '$/path': {}", includePath));
      return false;
    }

    const auto relative = std::filesystem::path(includePath.substr(2));

    if (const auto candidate = mProjectRoot / relative; std::filesystem::exists(candidate)) {
      resolved = {
          .kind = IncludePathKind::Rooted,
          .path = candidate,
      };
      return true;
    }

    diagnostics.error(
        {
            .mBegin =
                {
                    .mFile = includingFile.string(),
                    .mLine = 1,
                    .mColumn = 1,
                },
            .mEnd =
                {
                    .mFile = includingFile.string(),
                    .mLine = 1,
                    .mColumn = 1,
                },
        },
        std::format("project-root include path does not exist: {}", includePath));
    return false;
  }

  diagnostics.error(
      {
          .mBegin =
              {
                  .mFile = includingFile.string(),
                  .mLine = 1,
                  .mColumn = 1,
              },
          .mEnd =
              {
                  .mFile = includingFile.string(),
                  .mLine = 1,
                  .mColumn = 1,
              },
      },
      std::format("invalid include path; expected an absolute path or a '$/...' project-root path: ", includePath));
  return false;
}

bool ChirpTextDocumentLoader::resolveIncludes(ChirpTextDocument& document, const std::filesystem::path& sourcePath, ChirpTextDiagnosticBag diagnostics) const {
  for (auto& [path, span, resolvedDocument] : document.mIncludes) {
    ResolvedIncludePath resolved;

    // Even a custom resolver receives only syntactically valid include
    // forms. It may change where those forms are backed, but it cannot
    // introduce a third include syntax.
    if (const std::filesystem::path requested(path); !(path.size() >= 2 && path[0] == '$' && path[1] == '/') && !requested.is_absolute()) {
      diagnostics.error(span, std::format("invalid include path; expected an absolute path or a '$/...' project-root path: {}", path));
      return false;
    }

    if (mResolver) {
      if (!mResolver(sourcePath, path, resolved)) {
        diagnostics.error(span, std::format("custom include resolver failed for: {}", path));
        return false;
      }
    } else if (!resolveIncludePath(sourcePath, path, resolved, diagnostics)) {
      diagnostics.error(span, "failed to resolve include: " + path);
      return false;
    }

    resolvedDocument = load(resolved.path, diagnostics);
    if (!resolvedDocument)
      return false;
  }

  return true;
}
