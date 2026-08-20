#include "moth_ui/asset_id.h"
#include "moth_ui/layout/layout_entity_image.h"
#include "moth_ui/layout/layout_rect.h"

#include <catch2/catch_all.hpp>
#include <nlohmann/json.hpp>

using namespace moth_ui;

//NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

// ---- the identity itself ----------------------------------------------------

TEST_CASE("AssetId stores a string exactly", "[asset_id]") {
    AssetId const id{ std::string{ "ui/panel.png" } };
    REQUIRE(id.str() == "ui/panel.png");
    REQUIRE_FALSE(id.empty());
}

TEST_CASE("A default AssetId names nothing", "[asset_id]") {
    AssetId const id;
    REQUIRE(id.empty());
    REQUIRE(id.str().empty());
}

TEST_CASE("AssetId built from a path keeps the path as given", "[asset_id]") {
    // Relative stays relative. moth_ui no longer makes a path absolute, which is the
    // whole point of the type.
    AssetId const relative{ std::filesystem::path{ "ui/panel.png" } };
    REQUIRE(relative.str() == "ui/panel.png");

    AssetId const absolute{ std::filesystem::path{ "/games/demo/ui/panel.png" } };
    REQUIRE(absolute.str() == "/games/demo/ui/panel.png");
}

TEST_CASE("AssetId carries a value that is not a path at all", "[asset_id]") {
    // An engine names an asset by its own identity. moth_ui must not read it.
    AssetId const guid{ std::string{ "8f14e45f-ea8f-4b6d-9c1e-2a3b4c5d6e7f" } };
    REQUIRE(guid.str() == "8f14e45f-ea8f-4b6d-9c1e-2a3b4c5d6e7f");
}

TEST_CASE("AssetId is constructible from a string literal", "[asset_id]") {
    // Without a char const* overload this line does not compile at all: a literal
    // converts to std::string and to std::filesystem::path equally well, so the two
    // constructors are ambiguous. That made the most natural spelling the broken one.
    AssetId const id{ "ui/panel.png" };
    REQUIRE(id.str() == "ui/panel.png");
}

TEST_CASE("A literal and a std::string give the same identity", "[asset_id]") {
    REQUIRE(AssetId{ "ui/panel.png" } == AssetId{ std::string{ "ui/panel.png" } });
}

TEST_CASE("AssetId from a null pointer names nothing", "[asset_id]") {
    AssetId const id{ static_cast<char const*>(nullptr) };
    REQUIRE(id.empty());
}

TEST_CASE("AssetId compares by value", "[asset_id]") {
    AssetId const a{ std::string{ "ui/panel.png" } };
    AssetId const b{ std::string{ "ui/panel.png" } };
    AssetId const c{ std::string{ "ui/other.png" } };
    REQUIRE(a == b);
    REQUIRE(a != c);
}

// ---- what a layout does with it ---------------------------------------------

// Serializes an image entity, then reads it back, and returns what survived.
static AssetId roundTrip(AssetId const& stored, std::filesystem::path const& rootPath) {
    LayoutEntityImage source{ MakeDefaultLayoutRect(), stored };

    LayoutEntity::SerializeContext context;
    context.m_version = 1;
    context.m_rootPath = rootPath;

    nlohmann::json const json = source.Serialize(context);

    LayoutEntityImage restored{ MakeDefaultLayoutRect() };
    REQUIRE(restored.Deserialize(json, context));
    return restored.m_imageId;
}

TEST_CASE("A layout writes the identity it was given, unchanged", "[asset_id][layout]") {
    AssetId const stored{ std::string{ "ui/panel.png" } };
    LayoutEntityImage const source{ MakeDefaultLayoutRect(), stored };

    LayoutEntity::SerializeContext context;
    context.m_version = 1;
    context.m_rootPath = "/games/demo";

    nlohmann::json const json = source.Serialize(context);
    REQUIRE(json.at("imagePath").get<std::string>() == "ui/panel.png");
}

TEST_CASE("An identity survives a round trip unchanged", "[asset_id][layout]") {
    AssetId const stored{ std::string{ "ui/panel.png" } };
    REQUIRE(roundTrip(stored, "/games/demo") == stored);
}

TEST_CASE("The root path does not touch the identity", "[asset_id][layout]") {
    // This is the regression. LayoutEntityImage used to make the stored path relative to
    // m_rootPath on the way out and absolute against it on the way back in. So the value
    // a consumer put in was not the value it got out, and nothing but a path could be
    // stored at all.
    AssetId const stored{ std::string{ "ui/panel.png" } };
    REQUIRE(roundTrip(stored, "/games/demo") == roundTrip(stored, "/somewhere/else"));
    REQUIRE(roundTrip(stored, "") == stored);
}

TEST_CASE("An identity that is not a path survives a round trip", "[asset_id][layout]") {
    AssetId const guid{ std::string{ "8f14e45f-ea8f-4b6d-9c1e-2a3b4c5d6e7f" } };
    REQUIRE(roundTrip(guid, "/games/demo") == guid);
}

TEST_CASE("An image entity with no identity reads back empty", "[asset_id][layout]") {
    REQUIRE(roundTrip(AssetId{}, "/games/demo").empty());
}

//NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
