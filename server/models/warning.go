package models

import "time"

type Warning struct {
	ID int64 `gorm:"primary_key" json:"id"`
	PointId int `json:"pointId" gorm:"point_id"`
	BSUId int `json:"bsuId" gorm:"column:bsu_id;default:null"`
	BCUId int `json:"bcuId" gorm:"column:bcu_id;default:null"`
	BMUId int `json:"bmuId" gorm:"column:bmu_id"`
	ValueType int `json:"valueType" gorm:"value_type"`
	Value string `json:"value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
	Status int `json:"status" gorm:"default:null"`
	Operator int `json:"operator" gorm:"default:null"`
	Operation int `json:"operation" gorm:"default:null"`
	OperTime time.Time `json:"operTime" gorm:"column:oper_time;default:null"`
}

type WarningStationStat struct {
	StationId int `json:"stationId" gorm:"station_id"`
	StationName string `json:"stationName" gorm:"station_name"`
	BCUVoltageNum int `json:"bcuVoltageNum" gorm:"bcu_voltage_num"`
	BCUInsulationNum int `json:"bcuInsulationNum" gorm:"bcu_insulation_num"`
	BMUVoltageNum int `json:"bmuVoltageNum" gorm:"bmu_voltage_num"`
	BMUCurrentNum int `json:"bmuCurrentNum" gorm:"bmu_current_num"`
	BMUTemperatureNum int `json:"bmuTemperatureNum" gorm:"bmu_temperature_num"`
	BMUSmokeNum int `json:"bmuSmokeNum" gorm:"bmu_smoke_num"`
}

const (
	BCUVoltageCode = 1
	BCUInsulationCode = 2
	BMUVoltageCode = 11
	BMUCurrentCode = 12
	BMUTemperatureCode = 13
	BMUSmokeCode = 14
)

func GetWarningStationStat(userId int) []WarningStationStat{
	var warningStationStats []WarningStationStat
	db.Table("bms_station").Select("id as station_id, name as station_name").Where("owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)", userId, userId).Scan(&warningStationStats)
	for index, warningStationStat := range warningStationStats {
		db.Table("bms_warning").Select("count(*) as bcu_voltage_num").Where("value_type = ? and bcu_id in (select id from bms_bcu where station_id = ?)",BCUVoltageCode, warningStationStat.StationId).Scan(&warningStationStats[index])
		db.Table("bms_warning").Select("count(*) as bcu_insulation_num").Where("value_type = ? and bcu_id in (select id from bms_bcu where station_id = ?)",BCUInsulationCode, warningStationStat.StationId).Scan(&warningStationStats[index])
		db.Table("bms_warning").Select("count(*) as bmu_voltage_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id = ?)",BMUVoltageCode, warningStationStat.StationId).Scan(&warningStationStats[index])
		db.Table("bms_warning").Select("count(*) as bmu_current_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id = ?)",BMUCurrentCode, warningStationStat.StationId).Scan(&warningStationStats[index])
		db.Table("bms_warning").Select("count(*) as bmu_temperature_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id = ?)",BMUTemperatureCode, warningStationStat.StationId).Scan(&warningStationStats[index])
		db.Table("bms_warning").Select("count(*) as bmu_smoke_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id = ?)",BMUSmokeCode, warningStationStat.StationId).Scan(&warningStationStats[index])
	}
	return warningStationStats
}

func GetWarningTimeStat(userId int) map[string]interface{} {
	var times [8]time.Time
	var timeStrings [7]string
	var bcuVoltages, bcuInsulations, bmuVoltages, bmuCurrents, bmuTemperatures, bmuSmokes [7]int
	times[7] = time.Now()
	for i := 0; i < 7; i++ {
		var tmp = time.Now().AddDate(0, 0, i-7)
		times[i] = time.Date(tmp.Year(), tmp.Month(), tmp.Day(), 0, 0, 0, 0, tmp.Location())
		timeStrings[i] = times[i].Format("01-02")
	}
	for i := 0; i < 7; i++ {
		db.Table("bms_warning").Where("value_type=? and coll_time>? and coll_time<? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", BCUVoltageCode, times[i], times[i+1], userId, userId).Count(&bcuVoltages[i])
		db.Table("bms_warning").Where("value_type=? and coll_time>? and coll_time<? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", BCUInsulationCode, times[i], times[i+1], userId, userId).Count(&bcuInsulations[i])
		db.Table("bms_warning").Where("value_type=? and coll_time>? and coll_time<? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", BMUVoltageCode, times[i], times[i+1], userId, userId).Count(&bmuVoltages[i])
		db.Table("bms_warning").Where("value_type=? and coll_time>? and coll_time<? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", BMUCurrentCode, times[i], times[i+1], userId, userId).Count(&bmuCurrents[i])
		db.Table("bms_warning").Where("value_type=? and coll_time>? and coll_time<? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", BMUTemperatureCode, times[i], times[i+1], userId, userId).Count(&bmuTemperatures[i])
		db.Table("bms_warning").Where("value_type=? and coll_time>? and coll_time<? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", BMUSmokeCode, times[i], times[i+1], userId, userId).Count(&bmuSmokes[i])
	}
	data := make(map[string]interface{})
	data["xAxis"] = timeStrings
	data["bcuVoltages"] = bcuVoltages
	data["bcuInsulations"] = bcuInsulations
	data["bmuVoltages"] = bmuVoltages
	data["bmuCurrents"] = bmuCurrents
	data["bmuTemperatures"] = bmuTemperatures
	data["bmuSmokes"] = bmuSmokes
	return data
}

func GetUserWarnings(userId int, valueType string, intStartTime, intEndTime int64) []Warning {
	var userWarnings, userTemps []Warning
	if valueType == "BCUVoltage" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))", BCUVoltageCode, intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userWarnings = append(userWarnings, userTemps...)
	}
	if valueType == "BCUInsulation" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))", BCUInsulationCode, intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userWarnings = append(userWarnings, userTemps...)
	}
	if valueType == "BMUVoltage" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))", BMUVoltageCode, intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userWarnings = append(userWarnings, userTemps...)
	}
	if valueType == "BMUCurrent" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))", BMUCurrentCode, intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userWarnings = append(userWarnings, userTemps...)
	}
	if valueType == "BMUTemperature" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))", BMUTemperatureCode, intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userWarnings = append(userWarnings, userTemps...)
	}
	if valueType == "BMUSmoke" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))", BMUSmokeCode, intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userWarnings = append(userWarnings, userTemps...)
	}
	return userWarnings
}

func GetStationWarnings(stationId int, valueType string, intStartTime, intEndTime int64) []Warning {
	var stationWarnings, stationTemps []Warning
	if valueType == "BCUVoltage" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bcu_id in (select id from bms_bcu where station_id = ?)", BCUVoltageCode, intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationWarnings = append(stationWarnings, stationTemps...)
	}
	if valueType == "BCUInsulation" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bcu_id in (select id from bms_bcu where station_id = ?)", BCUInsulationCode, intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationWarnings = append(stationWarnings, stationTemps...)
	}
	if valueType == "BMUVoltage" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id = ?)", BMUVoltageCode, intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationWarnings = append(stationWarnings, stationTemps...)
	}
	if valueType == "BMUCurrent" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id = ?)", BMUCurrentCode, intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationWarnings = append(stationWarnings, stationTemps...)
	}
	if valueType == "BMUTemperature" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id = ?)", BMUTemperatureCode, intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationWarnings = append(stationWarnings, stationTemps...)
	}
	if valueType == "BMUSmoke" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where station_id = ?)", BMUSmokeCode, intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationWarnings = append(stationWarnings, stationTemps...)
	}
	return stationWarnings
}

func GetBCUWarnings(bcuId int, valueType string, intStartTime, intEndTime int64) []Warning {
	var bcuWarnings, bcuTemps []Warning
	if valueType == "BCUVoltage" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bcu_id = ?", BCUVoltageCode, intStartTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuWarnings = append(bcuWarnings, bcuTemps...)
	}
	if valueType == "BCUInsulation" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bcu_id = ?", BCUInsulationCode, intStartTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuWarnings = append(bcuWarnings, bcuTemps...)
	}
	if valueType == "BMUVoltage" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where bcu_id = ?)", BMUVoltageCode, intStartTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuWarnings = append(bcuWarnings, bcuTemps...)
	}
	if valueType == "BMUCurrent" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where bcu_id = ?)", BMUCurrentCode, intStartTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuWarnings = append(bcuWarnings, bcuTemps...)
	}
	if valueType == "BMUTemperature" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where bcu_id = ?)", BMUTemperatureCode, intStartTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuWarnings = append(bcuWarnings, bcuTemps...)
	}
	if valueType == "BMUSmoke" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id in (select id from bms_bmu where bcu_id = ?)", BMUSmokeCode, intStartTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuWarnings = append(bcuWarnings, bcuTemps...)
	}
	return bcuWarnings
}

func GetBMUWarnings(bmuId int, valueType string, intStartTime, intEndTime int64) []Warning {
	var bmuWarnings, bmuTemps []Warning
	if valueType == "BMUVoltage" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id = ?", BMUVoltageCode, intStartTime, intEndTime, bmuId).Scan(&bmuTemps)
		bmuWarnings = append(bmuWarnings, bmuTemps...)
	}
	if valueType == "BMUCurrent" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id = ?", BMUCurrentCode, intStartTime, intEndTime, bmuId).Scan(&bmuTemps)
		bmuWarnings = append(bmuWarnings, bmuTemps...)
	}
	if valueType == "BMUTemperature" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id = ?", BMUTemperatureCode, intStartTime, intEndTime, bmuId).Scan(&bmuTemps)
		bmuWarnings = append(bmuWarnings, bmuTemps...)
	}
	if valueType == "BMUSmoke" || valueType == "all" {
		db.Table("bms_warning").Select("*").Where("value_type = ? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=? and bmu_id = ?", BMUSmokeCode, intStartTime, intEndTime, bmuId).Scan(&bmuTemps)
		bmuWarnings = append(bmuWarnings, bmuTemps...)
	}
	return bmuWarnings
}

func ExistWarningById(id int) bool {
	var warning Warning
	db.Select("id").Where("id = ?", id).First(&warning)
	return warning.ID > 0
}

func AddWarningInBMU(pointId, bmuId, valueType int, value string) bool {
	db.Create(&Warning{
		PointId: pointId,
		BMUId: bmuId,
		ValueType: valueType,
		Value: value,
	})
	return true
}

func AddWarningInBCU(pointId, bmuId, bcuId, valueType int, value string) bool {
	db.Create(&Warning{
		PointId: pointId,
		BMUId: bmuId,
		BCUId: bcuId,
		ValueType: valueType,
		Value: value,
	})
	return true
}

func AddWarningInBSU(pointId, bmuId, bcuId, bsuId, valueType int, value string) bool {
	db.Create(&Warning{
		PointId: pointId,
		BMUId: bmuId,
		BCUId: bcuId,
		BSUId: bsuId,
		ValueType: valueType,
		Value: value,
	})
	return true
}

func UpdateWarning(id int, data interface{}) bool {
	db.Model(&Warning{}).Where("id = ?", id).Updates(data)
	return true
}

func DeleteWarning(id int) bool {
	db.Where("id = ?", id).Delete(&Warning{})
	return true
}