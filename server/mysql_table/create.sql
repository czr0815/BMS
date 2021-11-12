use bms;

CREATE TABLE IF NOT EXISTS `bms_image` (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `url` varchar(100) NOT NULL COMMENT '图片地址',
    PRIMARY KEY(`id`)
) ENGINe=InnoDB DEFAULT CHARSET=utf8 COMMENT '图片';

CREATE TABLE IF NOT EXISTS `bms_user` (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `username` varchar(10) NOT NULL COMMENT '用户名',
    `password` varchar(16) NOT NULL COMMENT '密码',
    `telephone` char(11) NOT NULL COMMENT '电话号码',
    `avatar` int(10) unsigned NOT NULL COMMENT '头像id',
    `des` tinytext NOT NULL COMMENT '用户描述',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`avatar`) references bms_image(`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT '用户信息';

CREATE TABLE IF NOT EXISTS `bms_station` (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `name` varchar(20) NOT NULL COMMENT '储能站名称',
    `position` varchar(40) NOT NULL COMMENT '储能站地址',
    `lng` double NOT NULL COMMENT '地理位置经度',
    `lat` double NOT NULL COMMENT '地理位置纬度',
    `owner_id` int(10) unsigned NOT NULL COMMENT '从属用户',
    `status` tinyint(1) NOT NULL DEFAULT 0 COMMENT '当前运行状态',
    `create_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`owner_id`) references bms_user (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT '储能站';

CREATE TABLE IF NOT EXISTS `bms_employment` (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `employer_id` int(10) unsigned NOT NULL COMMENT '雇主ID',
    `employee_id` int(10) unsigned NOT NULL COMMENT '被雇佣人ID',
    `station_id` int(10) unsigned NOT NULL COMMENT '相关储能站ID',
    `start_time` timestamp DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '创建和变更时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`employer_id`) references bms_user (`id`),
    FOREIGN KEY (`employee_id`) references bms_user(`id`),
    FOREIGN KEY (`station_id`) references bms_station(`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT '用户雇佣关系';

CREATE TABLE IF NOT EXISTS `bms_bsu` (
    `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
    `station_id` int(10) unsigned NOT NULL COMMENT '从属储能站',
    `status` tinyint(1) NOT NULL DEFAULT 0 COMMENT '当前运行状态',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`station_id`) references bms_station (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BSU';

CREATE TABLE IF NOT EXISTS `bms_bcu` (
    `id` int(10) unsigned NOT NULL  AUTO_INCREMENT,
    `station_id` int(10) unsigned NOT NULL COMMENT '从属储能站',
    `bsu_id` int(10) unsigned COMMENT '从属BSU',
    'name' varchar(30) DEFAULT '默认BCU' COMMENT 'BCU名称',
    `status` tinyint(1) NOT NULL DEFAULT 0 COMMENT '当前运行状态',
    'online_time' timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '最近上线时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`station_id`) references bms_station (`id`),
    FOREIGN KEY (`bsu_id`) references bms_bsu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BCU';

CREATE TABLE IF NOT EXISTS `bms_bmu` (
    `id` int(10) unsigned NOT NULL  AUTO_INCREMENT,
    `station_id` int(10) unsigned NOT NULL COMMENT '从属储能站',
    `bsu_id` int(10) unsigned COMMENT '从属BSU',
    `bcu_id` int(10) unsigned COMMENT '从属BCU',
    'name' varchar(30) DEFAULT '默认BMU' COMMENT 'BMU名称',
    `battery_num` tinyint(2) unsigned NOT NULL COMMENT '电池串数',
    `battery_type` tinyint(2) unsigned NOT NULL COMMENT '电池类型',
    `status` tinyint(1) NOT NULL DEFAULT 0 COMMENT '当前运行状态',
    `online_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '最近上线时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`station_id`) references bms_station (`id`),
    FOREIGN KEY (`bsu_id`) references bms_bsu (`id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BMU';

CREATE TABLE IF NOT EXISTS `bms_bmu_battery` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bmu_id` int(10) unsigned NOT NULL COMMENT '从属BMU',
    `voltage` DECIMAL(7,4) NOT NULL COMMENT '电压值',
    `soc` DECIMAL(4,1) NOT NULL COMMENT 'SOC值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bmu_id`) references bms_bmu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BMU电压收集记录';

CREATE TABLE IF NOT EXISTS `bms_bmu_battery_recent` (
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bmu_id` int(10) unsigned NOT NULL COMMENT '从属BMU',
    `voltage` DECIMAL(7,4) NOT NULL COMMENT '电压值',
    `soc` DECIMAL(4,1) NOT NULL COMMENT 'SOC值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`point_id`, `bmu_id`),
    FOREIGN KEY (`bmu_id`) references bms_bmu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BMU最近的电压收集记录';

CREATE TABLE IF NOT EXISTS `bms_bmu_temperature` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bmu_id` int(10) unsigned NOT NULL COMMENT '从属BMU',
    `value` DECIMAL(4,1) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bmu_id`) references bms_bmu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BMU温度收集记录';

CREATE TABLE IF NOT EXISTS `bms_bmu_temperature_recent` (
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bmu_id` int(10) unsigned NOT NULL COMMENT '从属BMU',
    `value` DECIMAL(4,1) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`point_id`, `bmu_id`),
    FOREIGN KEY (`bmu_id`) references bms_bmu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BMU最近的温度收集记录';

CREATE TABLE IF NOT EXISTS `bms_bmu_smoke` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bmu_id` int(10) unsigned NOT NULL COMMENT '从属BMU',
    `value` DECIMAL(4,1) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bmu_id`) references bms_bmu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BMU烟雾收集记录';

CREATE TABLE IF NOT EXISTS `bms_bmu_current` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `bmu_id` int(10) unsigned NOT NULL COMMENT '从属BMU',
    `value` DECIMAL(6,2) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bmu_id`) references bms_bmu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BMU电流收集记录';

CREATE TABLE IF NOT EXISTS `bms_bcu_voltage` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bcu_id` int(10) unsigned NOT NULL COMMENT '从属BCU',
    `value` DECIMAL(6,2) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BCU总压收集记录';

CREATE TABLE IF NOT EXISTS `bms_bcu_soc` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `bcu_id` int(10) unsigned NOT NULL COMMENT '从属BCU',
    `value` DECIMAL(4,1) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BCU的SOC收集记录';

CREATE TABLE IF NOT EXISTS `bms_bcu_current` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `bcu_id` int(10) unsigned NOT NULL COMMENT '从属BCU',
    `value` DECIMAL(6,2) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BCU的电流收集记录';

CREATE TABLE IF NOT EXISTS `bms_bcu_insulation` (
    `id` bigint(20) unsigned NOT NULL  AUTO_INCREMENT,
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bcu_id` int(10) unsigned NOT NULL COMMENT '从属BCU',
    `value` smallint(5) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BCU绝缘收集记录';

CREATE TABLE IF NOT EXISTS `bms_bcu_system` (
    `bcu_id` int(10) unsigned NOT NULL COMMENT '从属BCU',
    `update_time` timestamp DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENt '更新时间',
    `online` tinyint(1) unsigned NOT NULL COMMENT '在线状态',
    `charge` tinyint(1) unsigned NOT NULL COMMENT '充电状态',
    `cc` tinyint(1) unsigned NOT NULL COMMENT '枪口状态',
    `heat` tinyint(1) unsigned NOT NULL COMMENT '加热制冷',
    `system` tinyint(1) unsigned NOT NULL COMMENT '系统运行',
    `interior` tinyint(1) unsigned NOT NULL COMMENT '内部通信',
    `voltage` DECIMAL(6,2) NOT NULL COMMENT '充电机输出电压',
    `current` DECIMAL(6,2) NOT NULL COMMENT '充电机输出电流',
    PRIMARY KEY (`bcu_id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT 'BCU系统状态';

CREATE TABLE IF NOT EXISTS `bms_warning` (
    `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
    `point_id` smallint(3) unsigned NOT NULL COMMENT '数据采集点ID',
    `bsu_id` int(10) unsigned COMMENT '从属BSU',
    `bcu_id` int(10) unsigned COMMENT '从属BCU',
    `bmu_id` int(10) unsigned NOT NULL COMMENT '从属BMU',
    `value_type` tinyint(2) NOT NULL COMMENT '数据类型标识',
    `value` varchar(10) NOT NULL COMMENT '数据值',
    `coll_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '数据产生时间',
    `status` tinyint(2) NOT NULL DEFAULT 0 COMMENT '当前处理状态',
    `operator` int(10) unsigned COMMENT '处理人员',
    `operation` tinyint(2) COMMENT '处理措施',
    `oper_time` timestamp DEFAULT 0 ON UPDATE CURRENT_TIMESTAMP COMMENT '处理报警时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`bsu_id`) references bms_bsu (`id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`),
    FOREIGN KEY (`bmu_id`) references bms_bmu (`id`),
    FOREIGN KEY (`operator`) references bms_user (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT '报警记录';

CREATE TABLE IF NOT EXISTS `bms_order` (
    `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
    `order_type` tinyint(2) NOT NULL DEFAULT 0 COMMENT '订单类型',
    `station_id` int(10) unsigned COMMENT '从属BSU',
    `bcu_id` int(10) unsigned COMMENT '从属BCU',
    `status` tinyint(2) NOT NULL DEFAULT 1 COMMENT '当前进行状态',
    `electricity` float NOt NULL DEFAULT 0 COMMENT '订单电量',
    `start_time` timestamp DEFAULT CURRENT_TIMESTAMP COMMENT '开始时间',
    `end_time` timestamp DEFAULT 0 ON UPDATE CURRENT_TIMESTAMP COMMENT '结束时间',
    PRIMARY KEY (`id`),
    FOREIGN KEY (`station_id`) references bms_station (`id`),
    FOREIGN KEY (`bcu_id`) references bms_bcu (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT '充放电订单记录';