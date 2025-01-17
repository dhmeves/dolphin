// Copyright 2023 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "VideoCommon/GraphicsModSystem/Config/GraphicsModAsset.h"

#include "Common/Logging/Log.h"
#include "Common/StringUtil.h"

void GraphicsModAssetConfig::SerializeToConfig(picojson::object& json_obj) const
{
  json_obj["name"] = picojson::value{m_asset_id};

  picojson::object serialized_data;
  for (const auto& [name, path] : m_map)
  {
    serialized_data[name] = picojson::value{PathToString(path)};
  }
  json_obj["data"] = picojson::value{serialized_data};
}

bool GraphicsModAssetConfig::DeserializeFromConfig(const picojson::object& obj)
{
  auto name_iter = obj.find("name");
  if (name_iter == obj.end())
  {
    ERROR_LOG_FMT(VIDEO, "Failed to load mod configuration file, specified asset has no name");
    return false;
  }
  if (!name_iter->second.is<std::string>())
  {
    ERROR_LOG_FMT(VIDEO, "Failed to load mod configuration file, specified asset has a name "
                         "that is not a string");
    return false;
  }
  m_asset_id = name_iter->second.to_str();

  auto data_iter = obj.find("data");
  if (data_iter == obj.end())
  {
    ERROR_LOG_FMT(VIDEO, "Failed to load mod configuration file, specified asset '{}' has no data",
                  m_asset_id);
    return false;
  }
  if (!data_iter->second.is<picojson::object>())
  {
    ERROR_LOG_FMT(VIDEO,
                  "Failed to load mod configuration file, specified asset '{}' has data "
                  "that is not an object",
                  m_asset_id);
    return false;
  }
  for (const auto& [key, value] : data_iter->second.get<picojson::object>())
  {
    if (!value.is<std::string>())
    {
      ERROR_LOG_FMT(VIDEO,
                    "Failed to load mod configuration file, specified asset '{}' has data "
                    "with a value for key '{}' that is not a string",
                    m_asset_id, key);
      return false;
    }
    m_map[key] = value.to_str();
  }

  return true;
}
