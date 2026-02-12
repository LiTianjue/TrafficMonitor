#pragma once

#include "crow.h"
#include "crow/common.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>

namespace auth
{
    namespace detail
    {
        // MD5 implementation adapted from a public domain reference implementation.
        class Md5
        {
        public:
            using uint4 = uint32_t;
            using uint1 = uint8_t;

            Md5()
            {
                reset();
            }

            void update(const unsigned char* input, std::size_t length)
            {
                std::size_t index = (count_[0] >> 3) & 0x3F;

                if ((count_[0] += static_cast<uint4>(length << 3)) < static_cast<uint4>(length << 3))
                {
                    count_[1]++;
                }

                count_[1] += static_cast<uint4>(length >> 29);

                std::size_t partLen = 64 - index;
                std::size_t i = 0;

                if (length >= partLen)
                {
                    std::memcpy(&buffer_[index], input, partLen);
                    transform(buffer_);

                    for (i = partLen; i + 63 < length; i += 64)
                    {
                        transform(&input[i]);
                    }

                    index = 0;
                }

                std::memcpy(&buffer_[index], &input[i], length - i);
            }

            void update(const char* input, std::size_t length)
            {
                update(reinterpret_cast<const unsigned char*>(input), length);
            }

            void update(const std::string& input)
            {
                update(reinterpret_cast<const unsigned char*>(input.data()), input.size());
            }

            Md5& finalize()
            {
                static unsigned char padding[64] = {0x80};

                if (!finalized_)
                {
                    unsigned char bits[8];
                    encode(bits, count_, 8);

                    std::size_t index = (count_[0] >> 3) & 0x3f;
                    std::size_t padLen = (index < 56) ? (56 - index) : (120 - index);
                    update(padding, padLen);
                    update(bits, 8);

                    encode(digest_, state_, 16);

                    finalized_ = true;
                }
                return *this;
            }

            std::string hexdigest() const
            {
                if (!finalized_)
                {
                    return {};
                }

                std::ostringstream oss;
                for (unsigned char c : digest_)
                {
                    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
                }
                return oss.str();
            }

        private:
            void reset()
            {
                finalized_ = false;

                count_[0] = 0;
                count_[1] = 0;

                state_[0] = 0x67452301;
                state_[1] = 0xefcdab89;
                state_[2] = 0x98badcfe;
                state_[3] = 0x10325476;
            }

            static inline uint4 F(uint4 x, uint4 y, uint4 z)
            {
                return (x & y) | (~x & z);
            }

            static inline uint4 G(uint4 x, uint4 y, uint4 z)
            {
                return (x & z) | (y & ~z);
            }

            static inline uint4 H(uint4 x, uint4 y, uint4 z)
            {
                return x ^ y ^ z;
            }

            static inline uint4 I(uint4 x, uint4 y, uint4 z)
            {
                return y ^ (x | ~z);
            }

            static inline uint4 rotate_left(uint4 x, int n)
            {
                return (x << n) | (x >> (32 - n));
            }

            static void FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, int s, uint4 ac)
            {
                a += F(b, c, d) + x + ac;
                a = rotate_left(a, s);
                a += b;
            }

            static void GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, int s, uint4 ac)
            {
                a += G(b, c, d) + x + ac;
                a = rotate_left(a, s);
                a += b;
            }

            static void HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, int s, uint4 ac)
            {
                a += H(b, c, d) + x + ac;
                a = rotate_left(a, s);
                a += b;
            }

            static void II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, int s, uint4 ac)
            {
                a += I(b, c, d) + x + ac;
                a = rotate_left(a, s);
                a += b;
            }

            void transform(const unsigned char block[64])
            {
                uint4 a = state_[0];
                uint4 b = state_[1];
                uint4 c = state_[2];
                uint4 d = state_[3];
                uint4 x[16];

                decode(x, block, 64);

                // Round 1
                FF(a, b, c, d, x[0], 7, 0xd76aa478);
                FF(d, a, b, c, x[1], 12, 0xe8c7b756);
                FF(c, d, a, b, x[2], 17, 0x242070db);
                FF(b, c, d, a, x[3], 22, 0xc1bdceee);
                FF(a, b, c, d, x[4], 7, 0xf57c0faf);
                FF(d, a, b, c, x[5], 12, 0x4787c62a);
                FF(c, d, a, b, x[6], 17, 0xa8304613);
                FF(b, c, d, a, x[7], 22, 0xfd469501);
                FF(a, b, c, d, x[8], 7, 0x698098d8);
                FF(d, a, b, c, x[9], 12, 0x8b44f7af);
                FF(c, d, a, b, x[10], 17, 0xffff5bb1);
                FF(b, c, d, a, x[11], 22, 0x895cd7be);
                FF(a, b, c, d, x[12], 7, 0x6b901122);
                FF(d, a, b, c, x[13], 12, 0xfd987193);
                FF(c, d, a, b, x[14], 17, 0xa679438e);
                FF(b, c, d, a, x[15], 22, 0x49b40821);

                // Round 2
                GG(a, b, c, d, x[1], 5, 0xf61e2562);
                GG(d, a, b, c, x[6], 9, 0xc040b340);
                GG(c, d, a, b, x[11], 14, 0x265e5a51);
                GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
                GG(a, b, c, d, x[5], 5, 0xd62f105d);
                GG(d, a, b, c, x[10], 9, 0x02441453);
                GG(c, d, a, b, x[15], 14, 0xd8a1e681);
                GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
                GG(a, b, c, d, x[9], 5, 0x21e1cde6);
                GG(d, a, b, c, x[14], 9, 0xc33707d6);
                GG(c, d, a, b, x[3], 14, 0xf4d50d87);
                GG(b, c, d, a, x[8], 20, 0x455a14ed);
                GG(a, b, c, d, x[13], 5, 0xa9e3e905);
                GG(d, a, b, c, x[2], 9, 0xfcefa3f8);
                GG(c, d, a, b, x[7], 14, 0x676f02d9);
                GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);

                // Round 3
                HH(a, b, c, d, x[5], 4, 0xfffa3942);
                HH(d, a, b, c, x[8], 11, 0x8771f681);
                HH(c, d, a, b, x[11], 16, 0x6d9d6122);
                HH(b, c, d, a, x[14], 23, 0xfde5380c);
                HH(a, b, c, d, x[1], 4, 0xa4beea44);
                HH(d, a, b, c, x[4], 11, 0x4bdecfa9);
                HH(c, d, a, b, x[7], 16, 0xf6bb4b60);
                HH(b, c, d, a, x[10], 23, 0xbebfbc70);
                HH(a, b, c, d, x[13], 4, 0x289b7ec6);
                HH(d, a, b, c, x[0], 11, 0xeaa127fa);
                HH(c, d, a, b, x[3], 16, 0xd4ef3085);
                HH(b, c, d, a, x[6], 23, 0x04881d05);
                HH(a, b, c, d, x[9], 4, 0xd9d4d039);
                HH(d, a, b, c, x[12], 11, 0xe6db99e5);
                HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
                HH(b, c, d, a, x[2], 23, 0xc4ac5665);

                // Round 4
                II(a, b, c, d, x[0], 6, 0xf4292244);
                II(d, a, b, c, x[7], 10, 0x432aff97);
                II(c, d, a, b, x[14], 15, 0xab9423a7);
                II(b, c, d, a, x[5], 21, 0xfc93a039);
                II(a, b, c, d, x[12], 6, 0x655b59c3);
                II(d, a, b, c, x[3], 10, 0x8f0ccc92);
                II(c, d, a, b, x[10], 15, 0xffeff47d);
                II(b, c, d, a, x[1], 21, 0x85845dd1);
                II(a, b, c, d, x[8], 6, 0x6fa87e4f);
                II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
                II(c, d, a, b, x[6], 15, 0xa3014314);
                II(b, c, d, a, x[13], 21, 0x4e0811a1);
                II(a, b, c, d, x[4], 6, 0xf7537e82);
                II(d, a, b, c, x[11], 10, 0xbd3af235);
                II(c, d, a, b, x[2], 15, 0x2ad7d2bb);
                II(b, c, d, a, x[9], 21, 0xeb86d391);

                state_[0] += a;
                state_[1] += b;
                state_[2] += c;
                state_[3] += d;

                std::memset(x, 0, sizeof x);
            }

            static void encode(unsigned char* output, const uint4* input, std::size_t len)
            {
                for (std::size_t i = 0, j = 0; j < len; ++i, j += 4)
                {
                    output[j] = static_cast<unsigned char>(input[i] & 0xff);
                    output[j + 1] = static_cast<unsigned char>((input[i] >> 8) & 0xff);
                    output[j + 2] = static_cast<unsigned char>((input[i] >> 16) & 0xff);
                    output[j + 3] = static_cast<unsigned char>((input[i] >> 24) & 0xff);
                }
            }

            static void decode(uint4* output, const unsigned char* input, std::size_t len)
            {
                for (std::size_t i = 0, j = 0; j < len; ++i, j += 4)
                {
                    output[i] = static_cast<uint4>(input[j]) | (static_cast<uint4>(input[j + 1]) << 8) |
                                (static_cast<uint4>(input[j + 2]) << 16) | (static_cast<uint4>(input[j + 3]) << 24);
                }
            }

            bool finalized_{false};
            unsigned char buffer_[64]{};
            uint4 count_[2]{};
            uint4 state_[4]{};
            unsigned char digest_[16]{};
        };

        inline std::string md5_hex(const std::string& input)
        {
            Md5 md5;
            md5.update(input);
            md5.finalize();
            return md5.hexdigest();
        }

        inline std::string trim(const std::string& value)
        {
            const auto begin = value.find_first_not_of(" \t\r\n");
            if (begin == std::string::npos)
            {
                return {};
            }
            const auto end = value.find_last_not_of(" \t\r\n");
            return value.substr(begin, end - begin + 1);
        }

        inline std::string to_lower(std::string value)
        {
            std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
                return static_cast<char>(std::tolower(ch));
            });
            return value;
        }

        inline uint64_t next_random()
        {
            static std::mt19937_64 engine(std::random_device{}());
            static std::mutex mutex;
            std::lock_guard<std::mutex> lock(mutex);
            return engine();
        }
    } // namespace detail

    struct DigestAuthMiddleware
    {
        struct context
        {};

        DigestAuthMiddleware() = default;

        void before_handle(crow::request& req, crow::response& res, context&)
        {
            if (res.is_completed())
            {
                return;
            }

            if (authorize(req))
            {
                return;
            }

            res.code = crow::status::UNAUTHORIZED;
            res.body = "Unauthorized";
            res.set_header("WWW-Authenticate", build_challenge());
            res.end();
        }

        void after_handle(crow::request&, crow::response&, context&)
        {}

    private:
        static constexpr const char* realm_ = "Restricted Area";
        static constexpr const char* username_ = "admin";
        static constexpr const char* password_ = "admin";

        bool authorize(const crow::request& req) const
        {
            const std::string& header = req.get_header_value("Authorization");
            if (header.empty())
            {
                return false;
            }

            if (header.size() < 7 || header.compare(0, 7, "Digest ") != 0)
            {
                return false;
            }

            auto params = parse_digest_header(header.substr(7));
            if (params.empty())
            {
                return false;
            }

            if (auto it = params.find("username"); it == params.end() || it->second != username_)
            {
                return false;
            }

            if (auto it = params.find("realm"); it == params.end() || it->second != realm_)
            {
                return false;
            }

            const auto nonceIt = params.find("nonce");
            const auto uriIt = params.find("uri");
            const auto responseIt = params.find("response");
            if (nonceIt == params.end() || uriIt == params.end() || responseIt == params.end())
            {
                return false;
            }

            const std::string ha1 = detail::md5_hex(std::string(username_) + ":" + realm_ + ":" + password_);
            const std::string ha2 = detail::md5_hex(crow::method_name(req.method) + ":" + uriIt->second);

            std::string expected;
            const auto qopIt = params.find("qop");
            if (qopIt != params.end() && !qopIt->second.empty())
            {
                const auto ncIt = params.find("nc");
                const auto cnonceIt = params.find("cnonce");
                if (ncIt == params.end() || cnonceIt == params.end())
                {
                    return false;
                }
                expected = detail::md5_hex(ha1 + ":" + nonceIt->second + ":" + ncIt->second + ":" + cnonceIt->second + ":" + qopIt->second + ":" + ha2);
            }
            else
            {
                expected = detail::md5_hex(ha1 + ":" + nonceIt->second + ":" + ha2);
            }

            return expected == detail::to_lower(responseIt->second);
        }

        std::unordered_map<std::string, std::string> parse_digest_header(const std::string& value) const
        {
            std::unordered_map<std::string, std::string> result;
            std::size_t pos = 0;

            while (pos < value.size())
            {
                while (pos < value.size() && (value[pos] == ',' || std::isspace(static_cast<unsigned char>(value[pos]))))
                {
                    ++pos;
                }
                if (pos >= value.size())
                {
                    break;
                }

                const auto equalPos = value.find('=', pos);
                if (equalPos == std::string::npos)
                {
                    break;
                }

                std::string key = detail::trim(value.substr(pos, equalPos - pos));
                pos = equalPos + 1;

                if (pos >= value.size())
                {
                    break;
                }

                std::string parsedValue;
                if (value[pos] == '"')
                {
                    ++pos;
                    std::size_t end = pos;
                    while (end < value.size())
                    {
                        if (value[end] == '"' && (end == pos || value[end - 1] != '\\'))
                        {
                            break;
                        }
                        ++end;
                    }
                    parsedValue = value.substr(pos, end - pos);
                    pos = (end < value.size()) ? end + 1 : value.size();
                }
                else
                {
                    const auto commaPos = value.find(',', pos);
                    parsedValue = detail::trim(value.substr(pos, commaPos - pos));
                    pos = (commaPos == std::string::npos) ? value.size() : commaPos;
                }

                if (!key.empty())
                {
                    result[key] = parsedValue;
                }
            }

            return result;
        }

        std::string build_challenge() const
        {
            const std::string nonce = generate_nonce();
            std::ostringstream oss;
            oss << "Digest realm=\"" << realm_ << "\", qop=\"auth\", nonce=\"" << nonce << "\", opaque=\"" << opaque() << "\"";
            return oss.str();
        }

        std::string generate_nonce() const
        {
            const auto now = std::chrono::system_clock::now().time_since_epoch().count();
            const auto randomValue = detail::next_random();
            return detail::md5_hex(std::to_string(now) + ":" + std::to_string(randomValue));
        }

        static const std::string& opaque()
        {
            static const std::string value = detail::md5_hex(realm_);
            return value;
        }
    };
} // namespace auth


