#pragma once
#include <QString>

enum class DeviceType {
  Light,
  AirConditioner,
  Curtain,
  Camera,
  SensorTH
};

inline QString toString(DeviceType t) {
  switch (t) {
    case DeviceType::Light: return "light";
    case DeviceType::AirConditioner: return "ac";
    case DeviceType::Curtain: return "curtain";
    case DeviceType::Camera: return "camera";
    case DeviceType::SensorTH: return "sensor_th";
  }
  return "unknown";
}