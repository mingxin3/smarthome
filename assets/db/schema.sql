PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT NOT NULL UNIQUE,
  password_hash TEXT NOT NULL,
  created_at TEXT
);

CREATE TABLE IF NOT EXISTS device_groups (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS devices (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  type TEXT NOT NULL,
  group_id INTEGER,
  online INTEGER NOT NULL DEFAULT 1,
  state_json TEXT NOT NULL DEFAULT '{}',
  created_at TEXT,
  FOREIGN KEY(group_id) REFERENCES device_groups(id)
);

CREATE TABLE IF NOT EXISTS scenes (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  actions_json TEXT NOT NULL DEFAULT '[]',
  created_at TEXT
);

-- HistoryRepository uses: operation_logs(ts,actor,device_id,device_name,op,result)
CREATE TABLE IF NOT EXISTS operation_logs (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  ts TEXT,
  actor TEXT,
  device_id INTEGER,
  device_name TEXT,
  op TEXT,
  result TEXT
);

-- AlarmRepository uses: alarm_records(ts,alarm_type,message)
CREATE TABLE IF NOT EXISTS alarm_records (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  ts TEXT,
  alarm_type TEXT,
  message TEXT
);

-- EnvRepository + DataGenerator use: env_records(ts,temperature,humidity,air_quality)
CREATE TABLE IF NOT EXISTS env_records (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  ts TEXT,
  temperature REAL,
  humidity REAL,
  air_quality REAL
);