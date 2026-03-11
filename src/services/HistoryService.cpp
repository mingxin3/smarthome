#include "HistoryService.h"
#include "infrastructure/persistence/repositories/HistoryRepository.h"
#include "infrastructure/persistence/repositories/DeviceRepository.h"

HistoryService::HistoryService(HistoryRepository& repo, DeviceRepository& devRepo)
  : m_repo(repo), m_devRepo(devRepo) {}

QVector<OperationLog> HistoryService::listRecent(int limit) const {
  return m_repo.listRecent(limit);
}