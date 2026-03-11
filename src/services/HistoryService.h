#pragma once
#include <QVector>
#include "domain/models/OperationLog.h"

class HistoryRepository;
class DeviceRepository;

class HistoryService {
public:
  HistoryService(HistoryRepository& repo, DeviceRepository& devRepo);

  QVector<OperationLog> listRecent(int limit = 200) const;

private:
  HistoryRepository& m_repo;
  DeviceRepository& m_devRepo;
};