#pragma once
#include <QString>

enum class AlarmType {
  DeviceOffline,
  EnvThreshold
};

inline QString toString(AlarmType t) {
  switch (t) {
    case AlarmType::DeviceOffline: return "device_offline";
    case AlarmType::EnvThreshold: return "env_threshold";
  }
  return "unknown";
}