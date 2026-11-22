
#ifndef ECS_BITSET_HH
#define ECS_BITSET_HH

#include <cstddef>
#include <tuple>
#include <span>
#include <vector>
#include <memory>
#include <algorithm>

namespace ecs {

    /**
     * A contiguous set of bits.
     * The underlying buffer tracks only the first non-default bits. All subsequent bits are default (0 or 1).
     */
    template <typename Allocator = std::allocator<std::byte>>
    class bitset {

        std::vector<std::byte,Allocator> bytes_{};
        bool set_by_default_ = false;

        template <bool is_const>
        class reference_base {
            using bitset_type = std::conditional_t<is_const, bitset const, bitset>;
            bitset_type* bitset_;
            size_t index_;
        public:

            explicit reference_base(bitset_type* source, size_t index)
                : bitset_{source}, index_{index}
            {}

            [[nodiscard]] operator bool() const {
                return bitset_->test(index_);
            }

            [[nodiscard]] auto operator=(bool value) -> reference_base& {
                bitset_->set(index_, value); return *this;
            }

        };
        template <bool is_const> friend class reference_base;

    public:

        typedef reference_base<false> reference;
        typedef reference_base<true>  reference_const;

        bitset() = default;

        // TODO: casting between arbitrary trivially_copyable type
        // TODO: casting between spans of arbitrary trivially_copyable type

        [[nodiscard]] friend auto operator==(bitset const& a, bitset const& b)
        {
            if (a.set_by_default_ != b.set_by_default_) {
                return false;
            }

            auto const& [small, large] = a.bytes_.size() <= b.bytes_.size() ? std::tuple{a, b} : std::tuple{b, a};
            size_t const n = small.bytes_.size();

            if (n != 0 and std::memcmp(small.bytes_.data(), large.bytes_.data(), n) != 0) {
                return false;
            }
            return std::all_of(
                large.bytes_.data() + n,
                large.bytes_.data() + large.bytes_.size(),
                [&](auto byte) { return byte == large.default_byte(); }
            );
        }

    // Capacity

        /**
         * Returns the number of bits the buffer can hold without reallocating.
         */
        [[nodiscard]] auto capacity() const -> size_t {
            return bytes_.capacity() * CHAR_BIT;
        }

        /**
         * Increase the number of bits the buffer can hold without reallocating.
         */
        void reserve(size_t size) {
            // NB: We resize so the bits are immediately available.
            resize(size);
        }

        /**
         * A non-binding request to free unused capacity.
         */
        void shrink_to_fit() {
            while (not bytes_.empty() and bytes_.back() == default_byte()) { bytes_.pop_back(); }
            bytes_.shrink_to_fit();
        }

    // Element Access

        /**
         * Returns a reference to the bit at the given index.
         */
        [[nodiscard]] auto operator[](size_t index) -> reference {
            return reference {this, index};
        }

        /**
         * Returns a reference to the bit at the given index.
         */
        [[nodiscard]] auto operator[](size_t index) const -> reference_const {
            return reference_const {this, index};
        }

        /**
         * Returns the value of the bit at the given index.
         */
        [[nodiscard]] auto test(size_t index) const -> bool {
            if (index >= size()) { return set_by_default_; }
            std::byte const masked = bytes_[index / CHAR_BIT] & (std::byte{1} << (index % CHAR_BIT));
            return masked != std::byte{0};
        }

        /**
         * Determine whether all bits are set.
         */
        [[nodiscard]] auto all() const -> bool {
            if (not set_by_default_) { return false; }
            for (std::byte b : bytes_) if (b != ~std::byte{0}) { return false; }
            return true;
        }

        /**
         * Determine whether any bits are set.
         */
        [[nodiscard]] auto any() const -> bool {
            if (set_by_default_) { return true; }
            for (std::byte b : bytes_) if (b != std::byte{0}) { return true; }
            return false;
        }

        /**
         * Determine whether no bits are set.
         */
        [[nodiscard]] auto none() const -> bool {
            return not any();
        }

    // Modifiers

        /**
         * Set the bits to the result of binary AND on corresponding pairs of bits of *this and other.
         */
        auto operator&=(bitset const& other) -> bitset& {
            resize(other.size());
            for (size_t i = 0; i < bytes_.size(); ++i) { bytes_[i] &= other.bytes_[i]; }
            set_by_default_ &= other.set_by_default_;
            return *this;
        }

        /**
         * Set the bits to the result of binary OR on corresponding pairs of bits of *this and other.
         */
        auto operator|=(bitset const& other) -> bitset& {
            resize(other.size());
            for (size_t i = 0; i < bytes_.size(); ++i) { bytes_[i] |= other.bytes_[i]; }
            set_by_default_ |= other.set_by_default_;
            return *this;
        }

        /**
         * Set the bits to the result of binary XOR on corresponding pairs of bits of *this and other.
         */
        auto operator^=(bitset const& other) -> bitset& {
            resize(other.size());
            for (size_t i = 0; i < bytes_.size(); ++i) { bytes_[i] ^= other.bytes_[i]; }
            set_by_default_ ^= other.set_by_default_;
            return *this;
        }

        /**
         * Returns a temporary copy of *this with all bits flipped (binary NOT).
         */
        [[nodiscard]] auto operator~() const -> bitset {
            bitset other = *this;
            for (auto& b : other.bytes_) { b = ~b; }
            other.set_by_default_ = ~other.set_by_default_;
            return other;
        }

        /**
         * Shift the bits left by the given amount, filling with zeros.
         */
        auto operator<<=(size_t shift) -> bitset&
        {
            if (shift == 0) { return *this; }

            size_t const old_size = size();
            resize(old_size + shift);

            // TODO: Optimize this to use memcpy and bitshifts instead of per-bit operations.
            for (size_t i = old_size; i <= old_size; --i) {
                set(i + shift, test(i));
                set(i, false);
            }
            return *this;
        }

        /**
         * Shift the bits right by the given amount.
         */
        auto operator>>=(size_t shift) -> bitset&
        {
            if (shift == 0) { return *this; }

            // TODO: Optimize this to use memcpy and bitshifts instead of per-bit operations.
            for (size_t i = shift; i < size(); ++i) {
                set(i - shift, test(i));
                set(i, set_by_default_);
            }
            return *this;
        }

        [[nodiscard]] auto operator&(bitset const& other) const { return bitset{*this} &= other; }
        [[nodiscard]] auto operator|(bitset const& other) const { return bitset{*this} |= other; }
        [[nodiscard]] auto operator^(bitset const& other) const { return bitset{*this} ^= other; }
        [[nodiscard]] auto operator<<(size_t shift) const { return bitset{*this} <<= shift; }
        [[nodiscard]] auto operator>>(size_t shift) const { return bitset{*this} >>= shift; }

        /**
         * Set all bits.
         */
        auto set_all(bool value = true) -> bitset& {
            set_by_default_ = value;
            for (std::byte& b : bytes_) { b = default_byte(); }
            return *this;
        }

        /**
         * Set the bit at the given index to the given value.
         */
        auto set(size_t index, bool value = true) -> bitset& {
            if (value) {
                resize(index+1);
                bytes_[index / CHAR_BIT] |= (std::byte{1} << (index % CHAR_BIT));
            } else {
                bytes_[index / CHAR_BIT] &= ~(std::byte{1} << (index % CHAR_BIT));
            }
            return *this;
        }

        /**
         * Flips all bits.
         */
        auto flip_all() -> bitset& {
            set_by_default_ = ~set_by_default_;
            for (std::byte& b : bytes_) { b = ~b; }
            return *this;
        }

        /**
         * Flips the bit at the given index.
         */
        auto flip(size_t index) -> bitset& {
            resize(index+1);
            bytes_[index / CHAR_BIT] ^= (std::byte{1} << (index % CHAR_BIT));
            return *this;
        }

    private:

        /**
         * Returns the number of bits in the bitset.
         */
        [[nodiscard]] auto size() const -> size_t {
            return bytes_.size() * CHAR_BIT;
        }

        /**
         * Resize the underlying buffer to hold at least the given number of bits.
         * @note The buffer is never reduced in size. Use `shrink_to_fit()` to free unused capacity.
         */
        void resize(size_t size) {
            size_t const num_bytes = (size + CHAR_BIT - 1) / CHAR_BIT;
            if (num_bytes > bytes_.size()) {
                bytes_.resize(num_bytes, default_byte());
            }
        }

        /**
         * Returns the default byte value based on the set_by_default_ flag.
         */
        [[nodiscard]] auto default_byte() const -> std::byte {
            return set_by_default_ ? ~std::byte{0} : std::byte{0};
        }

    };

} // namespace ecs

#endif /* ECS_UNBOUNDED_BITSET_HH */
