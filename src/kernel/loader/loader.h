#pragma once

#include <karm-logger/logger.h>
#include <karm-media/icon.h>
#include <karm-media/image.h>
#include <karm-sys/context.h>
#include <karm-sys/file.h>
#include <vaev-json/json.h>

namespace Loader {

Res<Sys::File> openUrl(Mime::Url const &url);

struct Blob {
    Mime::Url url;
    Vaev::Json::Value props;

    static Res<Blob> fromJson(Vaev::Json::Value const &json) {
        if (json.isStr())
            return Ok(Blob{
                .url = Mime::Url::parse(json.asStr()),
                .props = NONE,
            });

        if (not json.isObject())
            return Error::invalidInput("expected object");

        return Ok(Blob{
            .url = Mime::Url::parse(try$(json.get("url").take<String>())),
            .props = json.get("props"),
        });
    }
};

struct Entry {
    Union<None, Mdi::Icon, Media::Image> icon = NONE;
    String name;
    Blob kernel;
    Vec<Blob> blobs;

    static Res<Entry> fromJson(Vaev::Json::Value const &json) {
        if (not json.isObject())
            return Error::invalidInput("expected object");

        Entry entry = {};

        auto maybeIcon = json.get("icon").take<String>();
        if (maybeIcon) {
            auto maybeImage = Media::loadImage(Mime::Url::parse(*maybeIcon));
            if (not maybeImage) {
                entry.icon = Mdi::byName(*maybeIcon).unwrap();
            } else {
                entry.icon = maybeImage.unwrap();
            }
        }

        entry.name = try$(json.get("name").take<String>());
        auto kernelJson = json.get("kernel");
        entry.kernel = try$(Blob::fromJson(kernelJson));

        auto blobsJson = try$(json.get("blobs").take<Vaev::Json::Array>());
        for (auto const &blobJson : blobsJson) {
            auto blob = try$(Blob::fromJson(blobJson));
            entry.blobs.pushBack(blob);
        }

        return Ok(entry);
    }
};

struct Configs {
    Opt<String> title;
    Opt<String> subtitle;
    Vec<Entry> entries;

    static Res<Configs> fromJson(Vaev::Json::Value const &json) {
        if (not json.isObject()) {
            return Error::invalidInput("expected array");
        }

        Configs configs = {};

        configs.title = json.get("title").take<String>();
        configs.subtitle = json.get("subtitle").take<String>();

        auto entriesJson = try$(json.get("entries").take<Vaev::Json::Array>());
        for (auto const &entryJson : entriesJson) {
            auto entry = try$(Entry::fromJson(entryJson));
            configs.entries.pushBack(entry);
        }

        return Ok(configs);
    }
};

Res<> showMenu(Sys::Context &ctx, Configs const &c);

Res<> loadEntry(Entry const &);

} // namespace Loader
