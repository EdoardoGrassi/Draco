#pragma once
#ifndef DRAKO_ASSET_BUNDLE_MANIFEST_HPP
#define DRAKO_ASSET_BUNDLE_MANIFEST_HPP

#include "drako/devel/asset_types.hpp"
#include "drako/io/input_file_stream.hpp"
#include "drako/io/output_file_stream.hpp"

#include <span>

namespace drako
{
    class asset_bundle_manifest
    {
    public:
        explicit asset_bundle_manifest(const std::span<std::byte[]> blob);

        asset_bundle_manifest(const asset_bundle_manifest&) = delete;
        asset_bundle_manifest& operator=(const asset_bundle_manifest&) = delete;

        void save_to_file(io::input_file_stream&);

        void load_from_file();

    private:
        /// @brief Header section of an asset manifest
        struct _header_type
        {
            api_version   version = build_api_version();
            std::uint32_t crc;
            std::uint32_t items_count;
        };

        _header_type                       _header;
        std::unique_ptr<asset_id[]>        _ids;
        std::unique_ptr<asset_load_info[]> _infos;
    };


    void asset_bundle_manifest::load_from_file(io::input_file_stream& ifs)
    {
        io::read_from_bytes(_header, ifs);

        // TODO: use std::make_unique_for_overwrite() when is available
        _ids = std::make_unique<asset_id[]>(_header.items_count);
        io::read_from_bytes(_ids.get(), ifs);

        // TODO: use std::make_unique_for_overwrite() when is available
        _infos = std::make_unique<asset_load_info[]>(_header.items_count);
        io::read_from_bytes(_infos.get(), _header.items_count, ifs);
    }
} // namespace drako

#endif // !DRAKO_ASSET_BUNDLE_MANIFEST_HPP