-- users
INSERT OR IGNORE INTO users(username, password_hash, created_at)
VALUES('admin', 'TEMP_PLACEHOLDER', datetime('now'));

-- device groups
INSERT OR IGNORE INTO device_groups(name) VALUES ('客厅');
INSERT OR IGNORE INTO device_groups(name) VALUES ('卧室');
INSERT OR IGNORE INTO device_groups(name) VALUES ('厨房');

-- default devices
INSERT OR IGNORE INTO devices(name, type, group_id, online, state_json, created_at)
VALUES
('客厅灯', 'light', 1, 1, '{"on":false,"brightness":50}', datetime('now')),
('卧室灯', 'light', 2, 1, '{"on":true,"brightness":80}', datetime('now')),
('空调', 'ac', 1, 1, '{"on":false,"temp":26,"mode":"cool"}', datetime('now')),
('窗帘', 'curtain', 1, 1, '{"position":30}', datetime('now')),
('温湿度传感器', 'sensor_th', 1, 1, '{"temperature":25.0,"humidity":50.0}', datetime('now'));

-- scenes (actions_json format)
INSERT OR IGNORE INTO scenes(name, actions_json, created_at)
VALUES
('回家模式',
 '[{"deviceId":1,"state":{"on":true,"brightness":80}},
   {"deviceId":3,"state":{"on":true,"temp":26,"mode":"cool"}},
   {"deviceId":4,"state":{"position":80}}]',
 datetime('now'));

INSERT OR IGNORE INTO scenes(name, actions_json, created_at)
VALUES
('睡眠模式',
 '[{"deviceId":1,"state":{"on":false,"brightness":0}},
   {"deviceId":3,"state":{"on":true,"temp":28,"mode":"sleep"}},
   {"deviceId":4,"state":{"position":0}}]',
 datetime('now'));

INSERT OR IGNORE INTO scenes(name, actions_json, created_at)
VALUES
('离家模式',
 '[{"deviceId":1,"state":{"on":false,"brightness":0}},
   {"deviceId":2,"state":{"on":false,"brightness":0}},
   {"deviceId":3,"state":{"on":false}},
   {"deviceId":4,"state":{"position":0}}]',
 datetime('now'));