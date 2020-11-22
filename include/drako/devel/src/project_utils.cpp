#include "drako/devel/project_utils.hpp"

#include "drako/devel/project_types.hpp"
#include "drako/devel/uuid.hpp"
#include "drako/devel/uuid_engine.hpp"
#include "drako/file_formats/dson.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace _fs = std::filesystem;

namespace drako::editor
{

    fs::path _external_asset_to_metafile(const _fs::path& source);

    fs::path _guid_to_metafile(const uuid& guid) noexcept;

    void _create_project_info_file(const fs::path& where)
    {
        std::ofstream ofs{ where / "project.dkproj" };
        throw std::runtime_error{ "Not implemented." };
    }


    [[nodiscard]] project_info load_project_info(const _fs::path& metafile)
    {
        if (!_fs::exists(metafile))
            throw std::invalid_argument{ DRAKO_STRINGIZE(metafile) };

        if (!_fs::is_regular_file(metafile))
            throw std::invalid_argument{ DRAKO_STRINGIZE(metafile) };

        std::ifstream file{ metafile };
        project_info  info;

        throw std::runtime_error{ "Not implemented." };
    }

    [[nodiscard]] internal_asset_info save_asset_metafile(const _fs::path& metafile)
    {
        if (!_fs::exists(metafile))
            throw std::runtime_error{ "File already exists." };

        throw std::runtime_error{ DRAKO_STRINGIZE(save_asset_metafile) ": not implemented." };
    }

    [[nodiscard]] internal_asset_info load_asset_metafile(const _fs::path& metafile)
    {
        std::ifstream file{ metafile };

        // TODO: remove narrowing conversion, check if the file is too big
        const auto size  = static_cast<std::size_t>(_fs::file_size(metafile));
        auto       bytes = std::make_unique<char[]>(size);

        file.read(bytes.get(), size);
        if (!file)
            throw std::runtime_error{ "Failed deserialization." };

        const std::string_view view{ bytes.get(), size };
        dson::object           serialized{ view };

        internal_asset_info info;
        serialized >> info;
        return info;
    }

    /*
    void load_asset_index_cache(const project& p)
    {
        std::ifstream f{ p.cache_directory() / "asset_index" };
        f.exceptions(std::ios_base::failbit | std::ios_base::badbit);

        while (f)
        {
            _fs::path path;
            f >> path;
        }
    }
    */

    void make_project_tree(const fs::path& root)
    {
        assert(fs::is_directory(root));

        if (fs::exists(root))
            throw std::runtime_error{ "Directory already exists." };

        try
        {
            fs::create_directories(root);
            fs::create_directory(root / project_asset_dir);
            fs::create_directory(root / project_cache_dir);
            fs::create_directory(root / project_meta_dir);

            project_info  info{};
            std::ofstream ofs{ root / project_config_file };
        }
        catch (const fs::filesystem_error&)
        {
            fs::remove_all(root);
            throw;
        }
    }

    void import_external_asset(const project_info& p, const fs::path& src)
    {
        if (!fs::exists(src) || !fs::is_regular_file(src))
            throw std::invalid_argument{ DRAKO_STRINGIZE(src) };

        try
        {
            const auto meta_file_path = _external_asset_to_metafile(src);
            if (fs::exists(meta_file_path))
                throw std::logic_error("Conflict with already existing asset");

            external_asset_info info{};

            std::ofstream meta_file{ meta_file_path };
        }
        catch (...)
        {
        }
    }

    void create_imported_asset(const project& p,
        const std::vector<fs::path>&          inputs,
        const std::vector<fs::path>&          outputs)
    {
        assert(!std::empty(inputs));
        assert(!std::empty(outputs));


        const auto guid = uuid_v1_engine{}();

        // TODO: end impl
    }


    void create_asset(const project& p, const fs::path& asset)
    {
        assert(fs::is_regular_file(asset));
        const auto guid           = uuid_v1_engine{}();
        const auto meta_file_path = p.meta_directory() / to_string(guid);

        try
        {
            internal_asset_info info{};
            std::ofstream       os{ meta_file_path };
            dson::object        serialized;

            os << (serialized << info);

            // TODO: also load asset in memory database
        }
        catch (...)
        {
            std::filesystem::remove(meta_file_path);
            throw;
        }
    }


    [[nodiscard]] project create_project_tree(const fs::path& where)
    {
        if (!fs::is_directory(where))
            throw std::invalid_argument{ DRAKO_STRINGIZE(where) };

        return project{ where };
    }

    void load_project(project& p)
    {
        // scan the whole meta folder for metafiles
        for (const auto& f : _fs::directory_iterator{ p.meta_directory() })
        {
            try
            {
                if (_fs::is_regular_file(f) && f.path().extension() == ".dkmeta")
                {
                    const auto info = load_asset_metafile(f);

                    _fs::path asset_file{ f.path() };
                    asset_file.replace_extension(); // remove .dkmeta
                    const auto size = static_cast<std::size_t>(_fs::file_size(asset_file));

                    p.assets.guids.push_back(info.uuid);
                    p.assets.names.push_back(info.name);
                    p.assets.sizes.push_back(size);
                    p.assets.paths.push_back(asset_file);
                }
            }
            catch (const _fs::filesystem_error& e)
            {
                std::cout << "Error occurred while loading meta file:\n"
                          << '\t' << e.what() << '\n'
                          << "\twith path1: " << e.path1() << '\n'
                          << "\twith path2: " << e.path2() << '\n';
            }
        }
        std::cout << "Project loaded!\n";
    }

    void save_project(const project& p)
    {
        throw std::runtime_error{ "Not implemented yet!" };
    }

    [[nodiscard]] fs::path guid_to_path(const project& p, const uuid& asset)
    {
        return p.cache_directory() / to_string(asset);
    }

    // build_error build_devel_project(const project_info& p);

} // namespace drako::editor