#pragma once

#include <filesystem>
#include <string>
#include <utility>

namespace moth_ui {
    /**
     * @brief Names an asset that a layout refers to, without saying what the name means.
     *
     * A layout stores an image reference as one of these. moth_ui carries the value and
     * never reads it. The consumer decides what it is: a project-relative path for a game
     * that loads loose files, or an engine asset identity for one that loads a cooked tree.
     *
     * @warning Construction is explicit on purpose. A layout used to store a
     *          @c std::filesystem::path, and moth_ui rewrote it on the way in and out. It
     *          no longer does, so whoever builds an AssetId owns normalizing what goes in
     *          it. An implicit conversion would let every old call site compile unchanged
     *          and quietly store a different value.
     *
     * @code
     * // A game that reads loose files names an image by a project-relative path.
     * auto const id = moth_ui::AssetId{ std::filesystem::relative(picked, projectRoot) };
     *
     * // An engine that reads a cooked tree names it by whatever its own identity is.
     * auto const id = moth_ui::AssetId{ guid.to_string() };
     * @endcode
     */
    class AssetId {
    public:
        /// @brief Constructs an empty identity, which names nothing.
        AssetId() = default;

        /**
         * @brief Constructs an identity from a string.
         * @param value The identity, stored as given.
         */
        explicit AssetId(std::string value)
            : m_value(std::move(value)) {
        }

        /**
         * @brief Constructs an identity from a string literal.
         *
         * Without this overload, @c AssetId{"ui/icon"} is ambiguous. A string literal
         * converts to @c std::string and to @c std::filesystem::path equally well, and
         * neither wins. So the most natural spelling was the one that did not compile.
         *
         * @param value The identity, stored as given. A null pointer names nothing.
         */
        explicit AssetId(char const* value)
            : m_value(value != nullptr ? value : "") {
        }

        /**
         * @brief Constructs an identity from a path.
         *
         * The path is stored as it arrives. This applies no normalizing of any kind, so
         * pass the path you want the layout file to hold.
         *
         * @param path The identity, stored as given.
         */
        explicit AssetId(std::filesystem::path const& path)
            : m_value(path.string()) {
        }

        /**
         * @brief Returns the identity as a string.
         * @return The stored value.
         */
        std::string const& str() const { return m_value; }

        /**
         * @brief Returns the identity as a path.
         *
         * This is a convenience for a consumer that treats an identity as a path. It says
         * nothing about whether this identity is one.
         *
         * @return The stored value, as a path.
         */
        std::filesystem::path path() const { return std::filesystem::path{ m_value }; }

        /**
         * @brief Reports whether this identity names nothing.
         * @return @c true when the stored value is empty.
         */
        bool empty() const { return m_value.empty(); }

        /**
         * @brief Compares two identities by their stored value.
         * @param other The identity to compare against.
         * @return @c true when both hold the same string.
         */
        bool operator==(AssetId const& other) const { return m_value == other.m_value; }

        /**
         * @brief Compares two identities by their stored value.
         * @param other The identity to compare against.
         * @return @c true when the two hold different strings.
         */
        bool operator!=(AssetId const& other) const { return !(*this == other); }

    private:
        std::string m_value; ///< The identity, exactly as the consumer gave it.
    };
}
