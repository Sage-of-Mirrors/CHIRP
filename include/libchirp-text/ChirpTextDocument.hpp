#pragma once
#include "libchirp-binary/ChirpErrorSource.hpp"
#include "libchirp-text/ChirpTextValue.hpp"
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace chirp::text {
struct ChirpTextDocument {
  struct Comment {
    std::string mText;
    ChirpSourceSpan mSpan;
  };

  struct Include {
    std::string mPath;
    ChirpSourceSpan mSpan;
    std::shared_ptr<ChirpTextDocument> mResolved;
  };

  struct CountStatement {
    std::int64_t mValue = 0;
    ChirpSourceSpan mSpan;
  };

  struct PropertyStatement {
    std::string mName;
    ChirpTextValue mValue;
    ChirpSourceSpan mSpan;
  };

  struct Row {
    std::vector<ChirpTextValue> mFields;
    ChirpSourceSpan mSpan;
  };

  struct Section {
    std::string mName;
    std::vector<Comment> mComments;
    std::vector<CountStatement> mCounts;
    std::vector<PropertyStatement> mProperties;
    std::vector<Row> mRows;
    ChirpSourceSpan mSpan;
  };

  struct UserData {
    std::string mNamespaceName;
    std::vector<PropertyStatement> mFields;
    std::vector<UserData> mChildren;
    ChirpSourceSpan mSpan;
  };

  std::string mTypeName;
  ChirpSourceSpan mSpan;

  std::vector<Comment> mComments;
  std::vector<Include> mIncludes;
  std::vector<Section> mSections;
  std::vector<UserData> mUserData;

  const Section* FindSection(const std::string& name) const;
  Section* FindSection(const std::string& name);
};
} // namespace chirp::text
