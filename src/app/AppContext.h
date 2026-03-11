#pragma once

#include <memory>

class DatabaseManager;
class SettingsManager;
class SoundPlayer;

class UserRepository;
class DeviceRepository;
class SceneRepository;
class HistoryRepository;
class AlarmRepository;
class EnvRepository;

class ProtocolCodec;
class DeviceGatewayClient;
class SimulatedDeviceServer;

class AuthService;
class DeviceService;
class SceneService;
class HistoryService;
class AlarmService;
class AdminService;
class AiService;

class DataGenerator;

struct AppContext {
    std::unique_ptr<DatabaseManager> db;
    std::unique_ptr<SettingsManager> settings;
    std::unique_ptr<SoundPlayer> sound;

    std::unique_ptr<UserRepository> userRepo;
    std::unique_ptr<DeviceRepository> deviceRepo;
    std::unique_ptr<SceneRepository> sceneRepo;
    std::unique_ptr<HistoryRepository> historyRepo;
    std::unique_ptr<AlarmRepository> alarmRepo;
    std::unique_ptr<EnvRepository> envRepo;

    std::unique_ptr<ProtocolCodec> codec;
    std::unique_ptr<DeviceGatewayClient> gatewayClient;

    std::unique_ptr<SimulatedDeviceServer> simulatedServer;
    std::unique_ptr<DataGenerator> dataGen;

    std::unique_ptr<AuthService> authService;
    std::unique_ptr<DeviceService> deviceService;
    std::unique_ptr<SceneService> sceneService;
    std::unique_ptr<HistoryService> historyService;
    std::unique_ptr<AlarmService> alarmService;
    std::unique_ptr<AdminService> adminService;

    std::unique_ptr<AiService> aiService;
};
