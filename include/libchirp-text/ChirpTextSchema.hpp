#pragma once
#include "libchirp-text/ChirpTextDiagnostic.hpp"
#include "libchirp-text/ChirpTextDocument.hpp"

class ChirpTextSchema {
public:
  virtual ~ChirpTextSchema() = default;
  virtual void validate(const ChirpTextDocument& document,
                        ChirpTextDiagnosticBag& diagnostics) const = 0;
};

class ChirpModelSchema final : public ChirpTextSchema {
public:
  void validate(const ChirpTextDocument& document,
                ChirpTextDiagnosticBag& diagnostics) const override;
};
