
#ifndef ECS_SPARSET_HH
#define ECS_SPARSET_HH

#include <vector>
#include <span>
#include <utility>
#include <limits>
#include <format>
#include <stdexcept>

namespace ecs {

    template <typename T>
    class sparset {

        std::vector<size_t> sparse_{};
        std::vector<size_t> dense_{};
        std::vector<T> data_;

        static constexpr auto novalue = std::numeric_limits<size_t>::max();

    public:

        sparset() = default;

    // Capacity

        /**
         * Checks whether this container has no elements.
         */
        [[nodiscard]] auto empty() const -> bool {
            return dense_.empty();
        }

        /**
         * Return the number of elements in this container.
         */
        [[nodiscard]] auto size() const -> size_t {
            return dense_.size();
        }

        /**
         * Increase the total number of elements that the underlying storage can hold without reallocation.
         */
        void reserve(size_t capacity) {
            dense_.reserve(capacity);
            data_.reserve(capacity);
        }

        /**
         * Return the number of elements that the underlying storage can hold without requiring reallocation.
         */
        [[nodiscard]] auto capacity() const -> size_t {
            return dense_.capacity();
        }

        /**
         * A non-binding request to remove unused capacity.
         * If reallocation occurs, all iterators and references to elements are invalid.
         */
        void shrink_to_fit() {
            dense_.shrink_to_fit();
            data_.shrink_to_fit();
        }

    // Lookup

        /**
         * Determine if an element exists at the given key.
         */
        [[nodiscard]] auto contains(size_t key) const -> bool {
            return key < sparse_.size() and sparse_[key] != novalue;
        }

    // Element Access

        /**
         * Returns a reference to the element indexed at the given key.
         * If this container does not have an element at this key, **behaviour is undefined**.
         */
        [[nodiscard]] auto operator[](size_t key) & -> T& {
            return data_[sparse_[key]];
        }

        /**
         * Returns a reference to the element indexed at the given key.
         * If this container does not have an element at this key, **behaviour is undefined**.
         */
        [[nodiscard]] auto operator[](size_t key) const& -> T const& {
            return data_[sparse_[key]];
        }

        /**
         * Returns a reference to the element indexed with the given key.
         * If this container does not have an element at this key, throws an exception of std::out_of_range.
         */
        [[nodiscard]] auto at(size_t key) & -> T& {
            assert_containment_or_throw(key);
            return operator[](key);
        }

        /**
         * Returns a reference to the element indexed with the given key.
         * If this container does not have an element at this key, throws an exception of std::out_of_range.
         */
        [[nodiscard]] auto at(size_t key) const& -> T const& {
            assert_containment_or_throw(key);
            return operator[](key);
        }

        /**
         * Access the underlying contiguous storage of elements.
         */
        [[nodiscard]] auto span() & -> std::span<T> {
            return std::span{data_};
        }

        /**
         * Access the underlying contiguous storage of elements.
         */
        [[nodiscard]] auto span() const& -> std::span<T const> {
            return std::span{data_};
        }

    // Modifiers

        /**
         * Construct an element in-place with the given arguments, if the key does not already index an element.
         * @returns A pair consisting of a reference to the element and whether construction took place.
         */
        template <typename... Args>
        auto emplace(size_t key, Args&&... args) -> std::pair<T&, bool>
        {
            if (contains(key)) {
                return { data_[sparse_[key]], false };
            }

            if (key >= sparse_.size()) {
                sparse_.resize(key + 1, novalue);
            }

            sparse_[key] = dense_.size();
            dense_.push_back(key);
            data_.emplace_back(std::forward<Args>(args)...);

            return { data_.back(), true };
        }

        /**
         * Removes the element indexed at the given key.
         * If this container does not have an element at the key, **behaviour is undefined**.
         */
        void erase(size_t key)
        {
            size_t index = sparse_[key];
            sparse_[key] = novalue;

            if (index + 1 < dense_.size()) {
                dense_[index] = dense_.back();
                data_[index] = std::move(data_.back());
                sparse_[dense_[index]] = index;
            }

            dense_.pop_back();
            data_.pop_back();
        }

        /**
         * Erase all elements.
         */
        void clear()
        {
            sparse_.clear();
            dense_.clear();
            data_.clear();
        }

    private:

        void assert_containment_or_throw(size_t key) const {
            if (not contains(key)) throw std::out_of_range{std::format(
                "ecs::sparset::containment_check: key (which is {}) is not contained within this sparset"
                , key
            )};
        }

    };

} // namespace ecs

#endif /* ECS_SPARSET_HH */
