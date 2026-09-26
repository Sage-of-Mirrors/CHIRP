#pragma once
#include "libchirp-text/ChirpTextSource.hpp"
#include "libchirp-text/ChirpTextValue.hpp"
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

struct ChirpTextDocument {
  struct Comment {
    std::string mText;
    ChirpTextSourceSpan mSpan;
  };

  struct Include {
    std::string mPath;
    ChirpTextSourceSpan mSpan;
    std::shared_ptr< ChirpTextDocument > mResolved;
  };

  struct CountStatement {
    std::int64_t mValue = 0;
    ChirpTextSourceSpan mSpan;
  };

  struct PropertyStatement {
    std::string mName;
    ChirpTextValue mValue;
    ChirpTextSourceSpan mSpan;
  };

  struct Row {
    std::vector< ChirpTextValue > mFields;
    ChirpTextSourceSpan mSpan;
  };

  struct Section {
    std::string mName;
    std::vector< Comment > mComments;
    std::vector< CountStatement > mCounts;
    std::vector< PropertyStatement > mProperties;
    std::vector< Row > mRows;
    ChirpTextSourceSpan mSpan;
  };

  struct UserData {
    std::string mNamespaceName;
    std::vector< PropertyStatement > mFields;
    std::vector< UserData > mChildren;
    ChirpTextSourceSpan mSpan;
  };

  std::string mTypeName;
  ChirpTextSourceSpan mSpan;

  std::vector< Comment > mComments;
  std::vector< Include > mIncludes;
  std::vector< Section > mSections;
  std::vector< UserData > mUserData;

  const Section* findSection(const std::string& name) const;
  Section* findSection(const std::string& name);
};
