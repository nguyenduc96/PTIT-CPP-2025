/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 */

#ifndef CRYPTO_MD5_H
#define CRYPTO_MD5_H

#include <string>
#include <cstdint>
#include <array>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace crypto
{
    namespace md5_internal
    {
        // MD5 Constants - RFC 1321
        constexpr uint32_t S11 = 7;
        constexpr uint32_t S12 = 12;
        constexpr uint32_t S13 = 17;
        constexpr uint32_t S14 = 22;
        constexpr uint32_t S21 = 5;
        constexpr uint32_t S22 = 9;
        constexpr uint32_t S23 = 14;
        constexpr uint32_t S24 = 20;
        constexpr uint32_t S31 = 4;
        constexpr uint32_t S32 = 11;
        constexpr uint32_t S33 = 16;
        constexpr uint32_t S34 = 23;
        constexpr uint32_t S41 = 6;
        constexpr uint32_t S42 = 10;
        constexpr uint32_t S43 = 15;
        constexpr uint32_t S44 = 21;

        // F, G, H and I are basic MD5 functions
        inline uint32_t F(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x & y) | (~x & z);
        }

        inline uint32_t G(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x & z) | (y & ~z);
        }

        inline uint32_t H(uint32_t x, uint32_t y, uint32_t z)
        {
            return x ^ y ^ z;
        }

        inline uint32_t I(uint32_t x, uint32_t y, uint32_t z)
        {
            return y ^ (x | ~z);
        }

        // Rotates x left n bits
        inline uint32_t rotate_left(uint32_t x, int n)
        {
            return (x << n) | (x >> (32 - n));
        }

        // FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4
        inline void FF(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = rotate_left(a + F(b, c, d) + x + ac, s) + b;
        }

        inline void GG(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = rotate_left(a + G(b, c, d) + x + ac, s) + b;
        }

        inline void HH(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = rotate_left(a + H(b, c, d) + x + ac, s) + b;
        }

        inline void II(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = rotate_left(a + I(b, c, d) + x + ac, s) + b;
        }
    }

    /**
     * @class MD5
     * @brief Implementation of the MD5 message-digest algorithm
     */
    class MD5
    {
    public:
        // MD5 produces a 16-byte (128-bit) hash value
        static constexpr size_t DIGEST_SIZE = 16;
        using DigestType = std::array<uint8_t, DIGEST_SIZE>;

        /**
         * @brief Default constructor
         */
        MD5()
        {
            reset();
        }

        /**
         * @brief Constructor with initialization data
         * @param data The data to hash
         * @param length The length of the data in bytes
         */
        MD5(const void *data, size_t length)
        {
            reset();
            update(data, length);
            finalize();
        }

        /**
         * @brief Constructor with string input
         * @param str The string to hash
         */
        explicit MD5(const std::string &str)
        {
            reset();
            update(str);
            finalize();
        }

        /**
         * @brief Reset the hash state
         */
        void reset()
        {
            finalized_ = false;
            buffer_index_ = 0;
            bit_length_ = 0;

            // Initialize state (RFC 1321)
            state_[0] = 0x67452301;
            state_[1] = 0xefcdab89;
            state_[2] = 0x98badcfe;
            state_[3] = 0x10325476;
        }

        /**
         * @brief Update the hash with more data
         * @param data The data to hash
         * @param length The length of the data in bytes
         */
        void update(const void *data, size_t length)
        {
            if (finalized_)
            {
                reset();
            }

            // Convert length to bits
            bit_length_ += (static_cast<uint64_t>(length) << 3);

            const uint8_t *input = static_cast<const uint8_t *>(data);

            // Process complete blocks
            while (length >= 64 - buffer_index_)
            {
                const size_t copy_len = 64 - buffer_index_;
                std::memcpy(buffer_ + buffer_index_, input, copy_len);
                transform(buffer_);
                input += copy_len;
                length -= copy_len;
                buffer_index_ = 0;
            }

            // Buffer remaining data
            if (length > 0)
            {
                std::memcpy(buffer_ + buffer_index_, input, length);
                buffer_index_ += length;
            }
        }

        /**
         * @brief Update the hash with string data
         * @param str The string to hash
         */
        void update(const std::string &str)
        {
            update(str.data(), str.length());
        }

        /**
         * @brief Finalize the hash calculation
         * @return Reference to this object
         */
        MD5 &finalize()
        {
            if (!finalized_)
            {
                pad_and_finalize();
                finalized_ = true;
            }
            return *this;
        }

        /**
         * @brief Get the raw digest bytes
         * @return The 16-byte digest array
         */
        const DigestType &digest() const
        {
            return digest_;
        }

        /**
         * @brief Get the digest as a hex string
         * @return 32-character hexadecimal representation
         */
        std::string hexdigest() const
        {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');

            for (const auto &byte : digest_)
            {
                ss << std::setw(2) << static_cast<int>(byte);
            }

            return ss.str();
        }

        /**
         * @brief Static function to compute MD5 hash of a string
         * @param str The input string
         * @return 32-character hexadecimal MD5 hash
         */
        static std::string hash(const std::string &str)
        {
            return MD5(str).hexdigest();
        }

    private:
        // 512-bit message block
        uint8_t buffer_[64];

        // Message block buffer index
        size_t buffer_index_;

        // 64-bit message length
        uint64_t bit_length_;

        // Hash state
        uint32_t state_[4];

        // Whether the hash has been finalized
        bool finalized_;

        // The resulting digest
        DigestType digest_;

        // Process a 64-byte message block
        void transform(const uint8_t block[64])
        {
            using namespace md5_internal;

            uint32_t a = state_[0];
            uint32_t b = state_[1];
            uint32_t c = state_[2];
            uint32_t d = state_[3];

            uint32_t x[16];

            // Convert block to 16 32-bit values
            for (int i = 0; i < 16; i++)
            {
                x[i] = static_cast<uint32_t>(block[i * 4]) |
                       (static_cast<uint32_t>(block[i * 4 + 1]) << 8) |
                       (static_cast<uint32_t>(block[i * 4 + 2]) << 16) |
                       (static_cast<uint32_t>(block[i * 4 + 3]) << 24);
            }

            // Round 1
            FF(a, b, c, d, x[0], S11, 0xd76aa478);
            FF(d, a, b, c, x[1], S12, 0xe8c7b756);
            FF(c, d, a, b, x[2], S13, 0x242070db);
            FF(b, c, d, a, x[3], S14, 0xc1bdceee);
            FF(a, b, c, d, x[4], S11, 0xf57c0faf);
            FF(d, a, b, c, x[5], S12, 0x4787c62a);
            FF(c, d, a, b, x[6], S13, 0xa8304613);
            FF(b, c, d, a, x[7], S14, 0xfd469501);
            FF(a, b, c, d, x[8], S11, 0x698098d8);
            FF(d, a, b, c, x[9], S12, 0x8b44f7af);
            FF(c, d, a, b, x[10], S13, 0xffff5bb1);
            FF(b, c, d, a, x[11], S14, 0x895cd7be);
            FF(a, b, c, d, x[12], S11, 0x6b901122);
            FF(d, a, b, c, x[13], S12, 0xfd987193);
            FF(c, d, a, b, x[14], S13, 0xa679438e);
            FF(b, c, d, a, x[15], S14, 0x49b40821);

            // Round 2
            GG(a, b, c, d, x[1], S21, 0xf61e2562);
            GG(d, a, b, c, x[6], S22, 0xc040b340);
            GG(c, d, a, b, x[11], S23, 0x265e5a51);
            GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
            GG(a, b, c, d, x[5], S21, 0xd62f105d);
            GG(d, a, b, c, x[10], S22, 0x02441453);
            GG(c, d, a, b, x[15], S23, 0xd8a1e681);
            GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
            GG(a, b, c, d, x[9], S21, 0x21e1cde6);
            GG(d, a, b, c, x[14], S22, 0xc33707d6);
            GG(c, d, a, b, x[3], S23, 0xf4d50d87);
            GG(b, c, d, a, x[8], S24, 0x455a14ed);
            GG(a, b, c, d, x[13], S21, 0xa9e3e905);
            GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
            GG(c, d, a, b, x[7], S23, 0x676f02d9);
            GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);

            // Round 3
            HH(a, b, c, d, x[5], S31, 0xfffa3942);
            HH(d, a, b, c, x[8], S32, 0x8771f681);
            HH(c, d, a, b, x[11], S33, 0x6d9d6122);
            HH(b, c, d, a, x[14], S34, 0xfde5380c);
            HH(a, b, c, d, x[1], S31, 0xa4beea44);
            HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
            HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
            HH(b, c, d, a, x[10], S34, 0xbebfbc70);
            HH(a, b, c, d, x[13], S31, 0x289b7ec6);
            HH(d, a, b, c, x[0], S32, 0xeaa127fa);
            HH(c, d, a, b, x[3], S33, 0xd4ef3085);
            HH(b, c, d, a, x[6], S34, 0x04881d05);
            HH(a, b, c, d, x[9], S31, 0xd9d4d039);
            HH(d, a, b, c, x[12], S32, 0xe6db99e5);
            HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
            HH(b, c, d, a, x[2], S34, 0xc4ac5665);

            // Round 4
            II(a, b, c, d, x[0], S41, 0xf4292244);
            II(d, a, b, c, x[7], S42, 0x432aff97);
            II(c, d, a, b, x[14], S43, 0xab9423a7);
            II(b, c, d, a, x[5], S44, 0xfc93a039);
            II(a, b, c, d, x[12], S41, 0x655b59c3);
            II(d, a, b, c, x[3], S42, 0x8f0ccc92);
            II(c, d, a, b, x[10], S43, 0xffeff47d);
            II(b, c, d, a, x[1], S44, 0x85845dd1);
            II(a, b, c, d, x[8], S41, 0x6fa87e4f);
            II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
            II(c, d, a, b, x[6], S43, 0xa3014314);
            II(b, c, d, a, x[13], S44, 0x4e0811a1);
            II(a, b, c, d, x[4], S41, 0xf7537e82);
            II(d, a, b, c, x[11], S42, 0xbd3af235);
            II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
            II(b, c, d, a, x[9], S44, 0xeb86d391);

            // Add the transformed values to the state
            state_[0] += a;
            state_[1] += b;
            state_[2] += c;
            state_[3] += d;
        }

        // Helper function to copy and pad final block
        void pad_and_finalize()
        {
            // Save number of bits
            uint64_t saved_length = bit_length_;

            // Pad data block with 1 bit followed by zeros
            const size_t index = buffer_index_;
            buffer_[index] = 0x80; // 10000000
            buffer_index_++;

            // Pad with zeros until we have 56 bytes (448 bits)
            const size_t padding_len = buffer_index_ <= 56 ? 56 - buffer_index_ : 120 - buffer_index_;

            std::memset(buffer_ + buffer_index_, 0, padding_len);
            buffer_index_ += padding_len;

            // Append length (64 bits)
            uint32_t length_low = static_cast<uint32_t>(saved_length);
            uint32_t length_high = static_cast<uint32_t>(saved_length >> 32);

            for (int i = 0; i < 4; i++)
            {
                buffer_[buffer_index_++] = (length_low >> (i * 8)) & 0xFF;
            }

            for (int i = 0; i < 4; i++)
            {
                buffer_[buffer_index_++] = (length_high >> (i * 8)) & 0xFF;
            }

            // Process the final block(s)
            transform(buffer_);

            // If we had to pad beyond 56 bytes, we need another block
            if (index > 55)
            {
                std::memset(buffer_, 0, 56);
                transform(buffer_);
            }

            // Store state in digest
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    digest_[i * 4 + j] = static_cast<uint8_t>((state_[i] >> (j * 8)) & 0xFF);
                }
            }
        }
    };

} // namespace crypto

#endif // CRYPTO_MD5_H