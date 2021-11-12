package models

import (
	"time"
)

type Collect struct {
	ID int64 `json:"valueId" gorm:"column:value_id"`
	Device string `json:"device"`
	DeviceId int `json:"deviceId" gorm:"column:device_id"`
	ValueType string `json:"valueType" gorm:"column:value_type"`
	PointId int `json:"pointId" gorm:"column:point_id"`
	Value float64 `json:"value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time"`
}

type DashboardOrder struct {
	Mon int `json:"mon"`
	Tue int `json:"tue"`
	Wed int `json:"wed"`
	Thu int `json:"thu"`
	Fri int `json:"fri"`
	Sat int `json:"sat"`
	Sun int `json:"sun"`
}

type DashboardStatus struct {
	Sum int `json:"sum"`
	Charge int `json:"charge"`
	Discharge int `json:"discharge"`
}

func GetStationCollects(stationId int, valueType string, startTime, endTime int64) []Collect {
	var stationCollects, stationTemps []Collect
	stationCollects = append(stationCollects, stationTemps...)
	return stationCollects
}

func GetBCUCollects(bcuId int, valueType string, startTime, endTime int64) []Collect {
	var bcuCollects, bcuTemps []Collect
	if valueType == "BCUVoltage" || valueType == "all" {
		db.Raw("select id as value_id, 'BCU' as device, bcu_id as 'device_id', 'Voltage' as value_type, point_id, value, coll_time from bms_bcu_voltage where bcu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bcuId, startTime, endTime).Scan(&bcuTemps)
		bcuCollects = append(bcuCollects, bcuTemps...)
	}
	if valueType == "BCUSOC" || valueType == "all" {
		db.Raw("select id as value_id, 'BCU' as device, bcu_id as 'device_id', 'SOC' as value_type, 0 as point_id, value, coll_time from bms_bcu_soc where bcu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bcuId, startTime, endTime).Scan(&bcuTemps)
		bcuCollects = append(bcuCollects, bcuTemps...)
	}
	if valueType == "BCUCurrent" || valueType == "all" {
		db.Raw("select id as value_id, 'BCU' as device, bcu_id as 'device_id', 'Current' as value_type, 0 as point_id, value, coll_time from bms_bcu_current where bcu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bcuId, startTime, endTime).Scan(&bcuTemps)
		bcuCollects = append(bcuCollects, bcuTemps...)
	}
	if valueType == "BCUInsulation" || valueType == "all" {
		db.Raw("select id as value_id, 'BCU' as device, bcu_id as 'device_id', 'Insulation' as value_type, point_id, value, coll_time from bms_bcu_insulation where bcu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bcuId, startTime, endTime).Scan(&bcuTemps)
		bcuCollects = append(bcuCollects, bcuTemps...)
	}
	return bcuCollects
}

func GetBMUCollects(bmuId int, valueType string, startTime, endTime int64) []Collect {
	var bmuCollects, bmuTemps []Collect
	if valueType == "BMUVoltage" || valueType == "all" {
		db.Raw("select id as value_id, 'BMU' as device, bmu_id as device_id, 'Voltage' as value_type, point_id, voltage as value, coll_time from bms_bmu_battery where bmu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bmuId, startTime, endTime).Scan(&bmuTemps)
		bmuCollects = append(bmuCollects, bmuTemps...)
	}
	if valueType == "BMUSOC" || valueType == "all" {
		db.Raw("select id as value_id, 'BMU' as device, bmu_id as device_id, 'SOC' as value_type, point_id, soc as value, coll_time from bms_bmu_battery where bmu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bmuId, startTime, endTime).Scan(&bmuTemps)
		bmuCollects = append(bmuCollects, bmuTemps...)
	}
	if valueType == "BMUCurrent" || valueType == "all" {
		db.Raw("select id as value_id, 'BMU' as device, bmu_id as device_id, 'Current' as value_type, 0 as point_id, value, coll_time from bms_bmu_current where bmu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bmuId, startTime, endTime).Scan(&bmuTemps)
		bmuCollects = append(bmuCollects, bmuTemps...)
	}
	if valueType == "BMUSmoke" || valueType == "all" {
		db.Raw("select id as value_id, 'BMU' as device, bmu_id as device_id, 'Smoke' as value_type, point_id, value, coll_time from bms_bmu_smoke where bmu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bmuId, startTime, endTime).Scan(&bmuTemps)
		bmuCollects = append(bmuCollects, bmuTemps...)
	}
	if valueType == "BMUTemperature" || valueType == "all" {
		db.Raw("select id as value_id, 'BMU' as device, bmu_id as device_id, 'Temperature' as value_type, point_id, value, coll_time from bms_bmu_temperature where bmu_id=? and unix_timestamp(coll_time)>=? and unix_timestamp(coll_time)<=?", bmuId, startTime, endTime).Scan(&bmuTemps)
		bmuCollects = append(bmuCollects, bmuTemps...)
	}
	return bmuCollects
}

func GetDashboardOrder(userId int) map[string]interface{} {
	var curr, last DashboardOrder
	data := make(map[string]interface{})
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))-1)", userId, userId).Count(&curr.Mon)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))-2)", userId, userId).Count(&curr.Tue)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))-3)", userId, userId).Count(&curr.Wed)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))-4)", userId, userId).Count(&curr.Thu)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))-5)", userId, userId).Count(&curr.Fri)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))-6)", userId, userId).Count(&curr.Sat)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))-7)", userId, userId).Count(&curr.Sun)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))+6)", userId, userId).Count(&last.Mon)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))+5)", userId, userId).Count(&last.Tue)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))+4)", userId, userId).Count(&last.Wed)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))+3)", userId, userId).Count(&last.Thu)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))+2)", userId, userId).Count(&last.Fri)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w'))+1)", userId, userId).Count(&last.Sat)
	db.Table("bms_order").Where("bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)) and subdate(start_time,if(date_format(start_time,'%w')=0,7,date_format(start_time,'%w')))", userId, userId).Count(&last.Sun)
	data["curr"] = curr
	data["last"] = last
	return data
}

func GetDashboardStatus(userId int) map[string]interface{} {
	var now, before DashboardStatus
	data := make(map[string]interface{})
	db.Table("bms_bcu").Where("station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?)", userId, userId).Count(&now.Sum)
	db.Table("bms_order").Where("end_time = 0 and order_type = 0 and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?))", userId, userId).Count(&now.Discharge)
	db.Table("bms_order").Where("end_time = 0 and order_type = 1 and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?))", userId, userId).Count(&now.Charge)
	before.Sum = now.Sum
	var start, end int
	db.Table("bms_order").Where("timestampdiff(hour, start_time, now())<=1 and order_type = 0 and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?))", userId, userId).Count(&start)
	db.Table("bms_order").Where("timestampdiff(hour, end_time, now())<=1 and order_type = 0 and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?))", userId, userId).Count(&end)
	before.Charge = now.Charge + start - end
	db.Table("bms_order").Where("timestampdiff(hour, start_time, now())<=1 and order_type = 1 and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?))", userId, userId).Count(&start)
	db.Table("bms_order").Where("timestampdiff(hour, end_time, now())<=1 and order_type = 1 and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?))", userId, userId).Count(&end)
	before.Discharge = now.Discharge + start - end
	data["now"] = now
	data["before"] = before
	return data
}