#pragma once
#include "libchirp-common/ChirpDiagnostic.hpp"
#include "libchirp-text/ChirpTextDocument.hpp"

namespace chirp::text {
class ChirpTextSchema {
public:
  virtual ~ChirpTextSchema() = default;
  virtual void Validate(const ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) const = 0;
};

class ChirpTextModelSchema final : public ChirpTextSchema {
public:
  void Validate(const ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) const override;
};

class ChirpTextMaterialSchema final : public ChirpTextSchema {
public:
  void Validate(const ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) const override {}
};

class ChirpTextAnimationSchema final : public ChirpTextSchema {
public:
  void Validate(const ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) const override {}
};

class ChirpTextArmatureSchema final : public ChirpTextSchema {
public:
  void Validate(const ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) const override {}
};

class ChirpTextTextureAnimationSchema final : public ChirpTextSchema {
public:
  void Validate(const ChirpTextDocument& document, ChirpDiagnosticBag& diagnostics) const override {}
};
} // namespace chirp::text