#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;
using f32 = float;

namespace chirp {
/* Type-sensitive byte swappers */
template < typename T >
constexpr T bswap16(T val) noexcept {
#if __GNUC__
  return __builtin_bswap16(val);
#elif _WIN32
  return _byteswap_ushort(val);
#else
  return (val = (val << 8) | ((val >> 8) & 0xFF));
#endif
}

template < typename T >
constexpr T bswap32(T val) noexcept {
#if __GNUC__
  return __builtin_bswap32(val);
#elif _WIN32
  return _byteswap_ulong(val);
#else
  val = (val & 0x0000FFFF) << 16 | (val & 0xFFFF0000) >> 16;
  val = (val & 0x00FF00FF) << 8 | (val & 0xFF00FF00) >> 8;
  return val;
#endif
}

template < typename T >
constexpr T bswap64(T val) noexcept {
#if __GNUC__
  return __builtin_bswap64(val);
#elif _WIN32
  return _byteswap_uint64(val);
#else
  return ((val & 0xFF00000000000000ULL) >> 56) | ((val & 0x00FF000000000000ULL) >> 40) |
         ((val & 0x0000FF0000000000ULL) >> 24) | ((val & 0x000000FF00000000ULL) >> 8) |
         ((val & 0x00000000FF000000ULL) << 8) | ((val & 0x0000000000FF0000ULL) << 24) |
         ((val & 0x000000000000FF00ULL) << 40) | ((val & 0x00000000000000FFULL) << 56);
#endif
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
constexpr int16_t SBig(int16_t val) noexcept { return bswap16(val); }
constexpr uint16_t SBig(uint16_t val) noexcept { return bswap16(val); }
constexpr int32_t SBig(int32_t val) noexcept { return bswap32(val); }
constexpr uint32_t SBig(uint32_t val) noexcept { return bswap32(val); }
constexpr int64_t SBig(int64_t val) noexcept { return bswap64(val); }
constexpr uint64_t SBig(uint64_t val) noexcept { return bswap64(val); }
constexpr float SBig(float val) noexcept {
  union {
    float f;
    u32 i;
  } uval1 = {val};
  union {
    u32 i;
    float f;
  } uval2 = {bswap32(uval1.i)};
  return uval2.f;
}
constexpr double SBig(double val) noexcept {
  union {
    double f;
    u32 i;
  } uval1 = {val};
  union {
    u32 i;
    double f;
  } uval2 = {bswap64(uval1.i)};
  return uval2.f;
}
#ifndef SBIG
#define SBIG(q)                                                                                    \
  (((q) & 0x000000FF) << 24 | ((q) & 0x0000FF00) << 8 | ((q) & 0x00FF0000) >> 8 |                  \
   ((q) & 0xFF000000) >> 24)
#endif

constexpr int16_t SLittle(int16_t val) noexcept { return val; }
constexpr uint16_t SLittle(uint16_t val) noexcept { return val; }
constexpr int32_t SLittle(int32_t val) noexcept { return val; }
constexpr uint32_t SLittle(uint32_t val) noexcept { return val; }
constexpr int64_t SLittle(int64_t val) noexcept { return val; }
constexpr uint64_t SLittle(uint64_t val) noexcept { return val; }
constexpr float SLittle(float val) noexcept { return val; }
constexpr double SLittle(double val) noexcept { return val; }
#ifndef SLITTLE
#define SLITTLE(q) (q)
#endif
#else
constexpr int16_t SLittle(int16_t val) noexcept { return bswap16(val); }
constexpr uint16_t SLittle(uint16_t val) noexcept { return bswap16(val); }
constexpr int32_t SLittle(int32_t val) noexcept { return bswap32(val); }
constexpr uint32_t SLittle(uint32_t val) noexcept { return bswap32(val); }
constexpr int64_t SLittle(int64_t val) noexcept { return bswap64(val); }
constexpr uint64_t SLittle(uint64_t val) noexcept { return bswap64(val); }
constexpr float SLittle(float val) noexcept {
  int32_t ival = bswap32(*((int32_t*)(&val)));
  return *((float*)(&ival));
}
constexpr double SLittle(double val) noexcept {
  int64_t ival = bswap64(*((int64_t*)(&val)));
  return *((double*)(&ival));
}
#ifndef SLITTLE
#define SLITTLE(q)                                                                                 \
  (((q) & 0x000000FF) << 24 | ((q) & 0x0000FF00) << 8 | ((q) & 0x00FF0000) >> 8 |                  \
   ((q) & 0xFF000000) >> 24)
#endif

constexpr int16_t SBig(int16_t val) noexcept { return val; }
constexpr uint16_t SBig(uint16_t val) noexcept { return val; }
constexpr int32_t SBig(int32_t val) noexcept { return val; }
constexpr uint32_t SBig(uint32_t val) noexcept { return val; }
constexpr int64_t SBig(int64_t val) noexcept { return val; }
constexpr uint64_t SBig(uint64_t val) noexcept { return val; }
constexpr float SBig(float val) noexcept { return val; }
constexpr double SBig(double val) noexcept { return val; }
#ifndef SBIG
#define SBIG(q) (q)
#endif
#endif

class FourCC {
protected:
  union {
    char fcc[4];
    uint32_t num = 0;
  };

public:
  // Sentinel FourCC
  constexpr FourCC() noexcept = default;
  constexpr FourCC(const FourCC& other) noexcept = default;
  constexpr FourCC(FourCC&& other) noexcept = default;
  constexpr FourCC(const char* name) noexcept : fcc{name[0], name[1], name[2], name[3]} {}
  constexpr FourCC(uint32_t n) noexcept : num(n) {}

  constexpr FourCC& operator=(const FourCC&) noexcept = default;
  constexpr FourCC& operator=(FourCC&&) noexcept = default;

  constexpr bool operator==(const FourCC& other) const noexcept { return num == other.num; }
  constexpr bool operator!=(const FourCC& other) const noexcept { return !operator==(other); }
  constexpr bool operator<(const FourCC& other) const noexcept { return num < other.num; }
  constexpr bool operator==(const char* other) const noexcept {
    return other[0] == fcc[0] && other[1] == fcc[1] && other[2] == fcc[2] && other[3] == fcc[3];
  }
  constexpr bool operator!=(const char* other) const noexcept { return !operator==(other); }
  constexpr bool operator==(int32_t other) const noexcept {
    return num == static_cast< uint32_t >(other);
  }
  constexpr bool operator!=(int32_t other) const noexcept { return !operator==(other); }
  constexpr bool operator<(int32_t other) const noexcept { return num < other; }
  constexpr bool operator==(uint32_t other) const noexcept { return num == other; }
  constexpr bool operator!=(uint32_t other) const noexcept { return !operator==(other); }
  constexpr bool operator<(uint32_t other) const noexcept { return num < other; }

  std::string toString() const { return std::string(std::begin(fcc), std::end(fcc)); }
  constexpr std::string_view toStringView() const { return std::string_view(fcc, std::size(fcc)); }
  constexpr uint32_t toUint32() const noexcept { return num; }
  constexpr const char* getChars() const noexcept { return fcc; }
  constexpr char* getChars() noexcept { return fcc; }
  constexpr bool IsValid() const noexcept { return num != 0; }
};
#define FOURCC(chars) FourCC(SBIG(chars))
} // namespace chirp