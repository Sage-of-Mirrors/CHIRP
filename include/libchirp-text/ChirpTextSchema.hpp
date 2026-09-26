#pragma once
#include "libchirp-text/ChirpTextDiagnostic.hpp"
#include "libchirp-text/ChirpTextDocument.hpp"

class ChirpTextSchema {
public:
  virtual ~ChirpTextSchema() = default;
  virtual void validate(const ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics) const = 0;
};

class ChirpTextModelSchema final : public ChirpTextSchema {
public:
  void validate(const ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics) const override;
};

class ChirpTextMaterialSchema final : public ChirpTextSchema {
public:
  void validate(const ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics) const override {}
};

class ChirpTextAnimationSchema final : public ChirpTextSchema {
public:
  void validate(const ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics) const override {}
};

class ChirpTextArmatureSchema final : public ChirpTextSchema {
public:
  void validate(const ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics) const override {}
};

class ChirpTextTextureAnimationSchema final : public ChirpTextSchema {
public:
  void validate(const ChirpTextDocument& document, ChirpTextDiagnosticBag& diagnostics) const override {}
};