#pragma once
#include "secure_vector.h"
#include <array>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/info_tuple.hpp>
#include <boost/throw_exception.hpp>
#include <boost/tuple/tuple.hpp>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
namespace ec {

using byte = uint8_t;
using bytes = std::vector<byte>;
using bytesRef = vector_ref<byte>;
using bytesConstRef = vector_ref<byte const>;
using string32 = std::array<char, 32>;
using bytesSec = secure_vector<byte>;

enum class WhenError {
    DontThrow = 0,
    Throw = 1,
};

/**
 * @brief : Base class for all exceptions
 */
struct Exception : virtual std::exception, virtual boost::exception {
    Exception(std::string _message = std::string())
        : m_message(std::move(_message)) {}
    const char *what() const noexcept override {
        return m_message.empty() ? std::exception::what() : m_message.c_str();
    }

  private:
    std::string m_message;
};

/// construct a new exception class overidding Exception
#define DEV_SIMPLE_EXCEPTION(X)                                                \
    struct X : virtual Exception {}

DEV_SIMPLE_EXCEPTION(BadHexCharacter);
DEV_SIMPLE_EXCEPTION(NoNetworking);
DEV_SIMPLE_EXCEPTION(RootNotFound);
DEV_SIMPLE_EXCEPTION(BadRoot);
DEV_SIMPLE_EXCEPTION(FileError);
DEV_SIMPLE_EXCEPTION(Overflow);
DEV_SIMPLE_EXCEPTION(FailedInvariant);

DEV_SIMPLE_EXCEPTION(MissingField);

DEV_SIMPLE_EXCEPTION(InterfaceNotSupported);
DEV_SIMPLE_EXCEPTION(ExternalFunctionFailure);
DEV_SIMPLE_EXCEPTION(InitLedgerConfigFailed);

DEV_SIMPLE_EXCEPTION(StorageError);
DEV_SIMPLE_EXCEPTION(OpenDBFailed);

DEV_SIMPLE_EXCEPTION(EncryptedDB);

DEV_SIMPLE_EXCEPTION(DatabaseError);
DEV_SIMPLE_EXCEPTION(DatabaseNeedRetry);

DEV_SIMPLE_EXCEPTION(WriteDBFailed);
DEV_SIMPLE_EXCEPTION(DecryptFailed);
DEV_SIMPLE_EXCEPTION(EncryptFailed);
DEV_SIMPLE_EXCEPTION(InvalidDiskEncryptionSetting);

DEV_SIMPLE_EXCEPTION(DBNotOpened);
DEV_SIMPLE_EXCEPTION(UnsupportedFeature);

DEV_SIMPLE_EXCEPTION(ForbidNegativeValue);
DEV_SIMPLE_EXCEPTION(InvalidConfiguration);
DEV_SIMPLE_EXCEPTION(InvalidPort);

inline int fromHexChar(char _i) noexcept {
    if (_i >= '0' && _i <= '9')
        return _i - '0';
    if (_i >= 'a' && _i <= 'f')
        return _i - 'a' + 10;
    if (_i >= 'A' && _i <= 'F')
        return _i - 'A' + 10;
    return -1;
}

/**
 * @brief: Trans given hex numbers to string
 * @tparam Iterator: Iterator type of given hex number
 * @param _it : Point to the first hex number to be transformed into hex string
 * @param _end: Point to the last hex number to be transformed into hex string
 * @param _prefix : Prefix of the outputed hex string
 * @return std::string : Transformed hex string of given hex numbers
 */
template <class Iterator>
std::string toHex(Iterator _it, Iterator _end, std::string const &_prefix) {
    typedef std::iterator_traits<Iterator> traits;
    static_assert(sizeof(typename traits::value_type) == 1,
                  "toHex needs byte-sized element type");

    static char const *hexdigits = "0123456789abcdef";
    size_t off = _prefix.size();
    std::string hex(std::distance(_it, _end) * 2 + off, '0');
    hex.replace(0, off, _prefix);
    for (; _it != _end; _it++) {
        hex[off++] = hexdigits[(*_it >> 4) & 0x0f];
        hex[off++] = hexdigits[*_it & 0x0f];
    }
    return hex;
}

/// Convert a series of bytes to the corresponding hex string.
/// @example toHex("A\x69") == "4169"
template <class T> std::string toHex(T const &_data) {
    return toHex(_data.begin(), _data.end(), "");
}

/// Convert a series of bytes to the corresponding hex string with 0x prefix.
/// @example toHexPrefixed("A\x69") == "0x4169"
template <class T> std::string toHexPrefixed(T const &_data) {
    return toHex(_data.begin(), _data.end(), "0x");
}

/// Converts a (printable) ASCII hex string into the corresponding byte stream.
/// @example fromHex("41626261") == asBytes("Abba")
/// If _throw = ThrowType::DontThrow, it replaces bad hex characters with 0's,
/// otherwise it will throw an exception.
inline bytes fromHex(std::string const &_s,
                     WhenError _throw = WhenError::DontThrow) {
    unsigned s = (_s.size() >= 2 && _s[0] == '0' && _s[1] == 'x') ? 2 : 0;
    std::vector<uint8_t> ret;
    ret.reserve((_s.size() - s + 1) / 2);

    if (_s.size() % 2) {
        int h = fromHexChar(_s[s++]);
        if (h != -1)
            ret.push_back(h);
        else if (_throw == WhenError::Throw)
            BOOST_THROW_EXCEPTION(BadHexCharacter());
        else
            return bytes();
    }
    for (unsigned i = s; i < _s.size(); i += 2) {
        int h = fromHexChar(_s[i]);
        int l = fromHexChar(_s[i + 1]);
        if (h != -1 && l != -1)
            ret.push_back((byte)(h * 16 + l));
        else if (_throw == WhenError::Throw)
            BOOST_THROW_EXCEPTION(BadHexCharacter());
        else
            return bytes();
    }
    return ret;
}

/// @returns true if @a _s is a hex string.
inline bool isHex(std::string const &_s) noexcept {
    auto it = _s.begin();
    if (_s.compare(0, 2, "0x") == 0)
        it += 2;
    return std::all_of(it, _s.end(),
                       [](char c) { return fromHexChar(c) != -1; });
}

/// @returns true if @a _hash is a hash conforming to FixedHash type @a T.
template <class T> static bool isHash(std::string const &_hash) {
    return (_hash.size() == T::size * 2 ||
            (_hash.size() == T::size * 2 + 2 && _hash.substr(0, 2) == "0x")) &&
           isHex(_hash);
}

/// Converts byte array to a string containing the same (binary) data. Unless
/// the byte array happens to contain ASCII data, this won't be printable.
inline std::string asString(bytes const &_b) {
    return std::string((char const *)_b.data(),
                       (char const *)(_b.data() + _b.size()));
}

/// Converts byte array ref to a string containing the same (binary) data.
/// Unless the byte array happens to contain ASCII data, this won't be
/// printable.
inline std::string asString(bytesConstRef _b) {
    return std::string((char const *)_b.data(),
                       (char const *)(_b.data() + _b.size()));
}

/// Converts a string to a byte array containing the string's (byte) data.
inline bytes asBytes(std::string const &_b) {
    return bytes((byte const *)_b.data(),
                 (byte const *)(_b.data() + _b.size()));
}

/// Converts a string into the big-endian base-16 stream of integers (NOT
/// ASCII).
/// @example asNibbles("A")[0] == 4 && asNibbles("A")[1] == 1
inline bytes asNibbles(bytesConstRef const &_s) {
    std::vector<uint8_t> ret;
    ret.reserve(_s.size() * 2);
    for (auto i : _s) {
        ret.push_back(i / 16);
        ret.push_back(i % 16);
    }
    return ret;
}

// Algorithms for string and string-like collections.

/// Escapes a string into the C-string representation.
/// @p _all if true will escape all characters, not just the unprintable ones.
inline std::string escaped(std::string const &_s, bool _all = true) {
    static const std::map<char, char> prettyEscapes{
        {'\r', 'r'}, {'\n', 'n'}, {'\t', 't'}, {'\v', 'v'}};
    std::string ret;
    ret.reserve(_s.size() + 2);
    ret.push_back('"');
    for (auto i : _s)
        if (i == '"' && !_all)
            ret += "\\\"";
        else if (i == '\\' && !_all)
            ret += "\\\\";
        else if (prettyEscapes.count(i) && !_all) {
            ret += '\\';
            ret += prettyEscapes.find(i)->second;
        } else if (i < ' ' || _all) {
            ret += "\\x";
            ret.push_back("0123456789abcdef"[(uint8_t)i / 16]);
            ret.push_back("0123456789abcdef"[(uint8_t)i % 16]);
        } else
            ret.push_back(i);
    ret.push_back('"');
    return ret;
}

/// Determines the length of the common prefix of the two collections given.
/// @returns the number of elements both @a _t and @a _u share, in order, at the
/// beginning.
/// @example commonPrefix("Hello world!", "Hello, world!") == 5
template <class T, class _U> unsigned commonPrefix(T const &_t, _U const &_u) {
    unsigned s = std::min<unsigned>(_t.size(), _u.size());
    for (unsigned i = 0;; ++i)
        if (i == s || _t[i] != _u[i])
            return i;
    return s;
}

/// Trims a given number of elements from the front of a collection.
/// Only works for POD element types.
template <class T> void trimFront(T &_t, unsigned _elements) {
    static_assert(std::is_pod<typename T::value_type>::value, "");
    memmove(_t.data(), _t.data() + _elements,
            (_t.size() - _elements) * sizeof(_t[0]));
    _t.resize(_t.size() - _elements);
}

/// Pushes an element on to the front of a collection.
/// Only works for POD element types.
template <class T, class _U> void pushFront(T &_t, _U _e) {
    static_assert(std::is_pod<typename T::value_type>::value, "");
    _t.push_back(_e);
    memmove(_t.data() + 1, _t.data(), (_t.size() - 1) * sizeof(_e));
    _t[0] = _e;
}

/// Concatenate two vectors of elements of POD types.
template <class T>
inline std::vector<T> &operator+=(
    std::vector<typename std::enable_if<std::is_pod<T>::value, T>::type> &_a,
    std::vector<T> const &_b) {
    auto s = _a.size();
    _a.resize(_a.size() + _b.size());
    memcpy(_a.data() + s, _b.data(), _b.size() * sizeof(T));
    return _a;
}

/// Concatenate two vectors of elements.
template <class T>
inline std::vector<T> &operator+=(
    std::vector<typename std::enable_if<!std::is_pod<T>::value, T>::type> &_a,
    std::vector<T> const &_b) {
    _a.reserve(_a.size() + _b.size());
    for (auto &i : _b)
        _a.push_back(i);
    return _a;
}

/// Insert the contents of a container into a set
template <class T, class U>
std::set<T> &operator+=(std::set<T> &_a, U const &_b) {
    for (auto const &i : _b)
        _a.insert(i);
    return _a;
}

/// Insert the contents of a container into an unordered_set
template <class T, class U>
std::unordered_set<T> &operator+=(std::unordered_set<T> &_a, U const &_b) {
    for (auto const &i : _b)
        _a.insert(i);
    return _a;
}

/// Concatenate the contents of a container onto a vector
template <class T, class U>
std::vector<T> &operator+=(std::vector<T> &_a, U const &_b) {
    for (auto const &i : _b)
        _a.push_back(i);
    return _a;
}

/// Insert the contents of a container into a set
template <class T, class U> std::set<T> operator+(std::set<T> _a, U const &_b) {
    return _a += _b;
}

/// Insert the contents of a container into an unordered_set
template <class T, class U>
std::unordered_set<T> operator+(std::unordered_set<T> _a, U const &_b) {
    return _a += _b;
}

/// Concatenate the contents of a container onto a vector
template <class T, class U>
std::vector<T> operator+(std::vector<T> _a, U const &_b) {
    return _a += _b;
}

/// Concatenate two vectors of elements.
template <class T>
inline std::vector<T> operator+(std::vector<T> const &_a,
                                std::vector<T> const &_b) {
    std::vector<T> ret(_a);
    return ret += _b;
}

/// Make normal string from fixed-length string.
std::string toString(string32 const &_s);

template <class T, class U> std::vector<T> keysOf(std::map<T, U> const &_m) {
    std::vector<T> ret;
    for (auto const &i : _m)
        ret.push_back(i.first);
    return ret;
}

template <class T, class U>
std::vector<T> keysOf(std::unordered_map<T, U> const &_m) {
    std::vector<T> ret;
    for (auto const &i : _m)
        ret.push_back(i.first);
    return ret;
}

template <class T, class U> std::vector<U> valuesOf(std::map<T, U> const &_m) {
    std::vector<U> ret;
    ret.reserve(_m.size());
    for (auto const &i : _m)
        ret.push_back(i.second);
    return ret;
}

template <class T, class U>
std::vector<U> valuesOf(std::unordered_map<T, U> const &_m) {
    std::vector<U> ret;
    ret.reserve(_m.size());
    for (auto const &i : _m)
        ret.push_back(i.second);
    return ret;
}

template <class T, class V> bool contains(T const &_t, V const &_v) {
    return std::end(_t) != std::find(std::begin(_t), std::end(_t), _v);
}

template <class V>
bool contains(std::unordered_set<V> const &_set, V const &_v) {
    return _set.find(_v) != _set.end();
}

} // namespace ec
