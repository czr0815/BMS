package models

type ItemInt struct {
	Name string `json:"name"`
	Value int `json:"value"`
}

type ItemFloat struct {
	Name string `json:"name"`
	Value float64 `json:"value"`
}

type ScreenWarning struct {
	BCUVoltageNum int `json:"bcuVoltageNum" gorm:"bcu_voltage_num"`
	BCUInsulationNum int `json:"bcuInsulationNum" gorm:"bcu_insulation_num"`
	BMUVoltageNum int `json:"bmuVoltageNum" gorm:"bmu_voltage_num"`
	BMUCurrentNum int `json:"bmuCurrentNum" gorm:"bmu_current_num"`
	BMUTemperatureNum int `json:"bmuTemperatureNum" gorm:"bmu_temperature_num"`
	BMUSmokeNum int `json:"bmuSmokeNum" gorm:"bmu_smoke_num"`
}

type ScreenOverall struct {
	Order int `json:"order" gorm:"ord"`
	Discharge float64 `json:"discharge"`
	Charge float64 `json:"charge"`
	Station int
	BCU int
	BMU int
}

type ScreenStation struct {
	StationName string `json:"stationName" gorm:"station_name"`
	StationId int `json:"stationId" gorm:"station_id"`
	Soc float64 `json:"soc"`
}

type ScreenBattery struct {
	Abundant int `json:"abundant"`
	Medium int `json:"medium"`
	Less int `json:"less"`
	Scare int `json:"scare"`
}

func GetScreenOrderStat(userId int) map[string]interface{} {
	data := make(map[string]interface{})
	data["option"] = GetOrderTimeStat(userId)
	var finish, process, total int
	db.Table("bms_order").Where("end_time = 0 and station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Count(&process)
	db.Table("bms_order").Where("end_time != 0 and station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Count(&finish)
	db.Table("bms_order").Where("station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Count(&total)
	data["total"] = total
	data["finish"] = finish
	data["process"] = process
	data["invalid"] = total - finish - process
	return data
}

func GetScreenWarningStat(userId int) ScreenWarning {
	var screenWarning ScreenWarning
	db.Table("bms_warning").Select("count(*) as bcu_voltage_num").Where("value_type = ? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))",BCUVoltageCode, userId, userId).Scan(&screenWarning)
	db.Table("bms_warning").Select("count(*) as bcu_insulation_num").Where("value_type = ? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))",BCUInsulationCode, userId, userId).Scan(&screenWarning)
	db.Table("bms_warning").Select("count(*) as bmu_voltage_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))",BMUVoltageCode, userId, userId).Scan(&screenWarning)
	db.Table("bms_warning").Select("count(*) as bmu_current_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))",BMUCurrentCode, userId, userId).Scan(&screenWarning)
	db.Table("bms_warning").Select("count(*) as bmu_temperature_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))",BMUTemperatureCode, userId, userId).Scan(&screenWarning)
	db.Table("bms_warning").Select("count(*) as bmu_smoke_num").Where("value_type = ? and bmu_id in (select id from bms_bmu where station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)))",BMUSmokeCode, userId, userId).Scan(&screenWarning)
	return screenWarning
}

func GetScreenOverallStat(userId int) map[string]interface{} {
	var screenOverall ScreenOverall
	db.Table("bms_order").Select("count(*) as ord, sum(electricity) as charge").Where("station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenOverall)
	db.Table("bms_order").Select("sum(electricity) as discharge").Where("order_type = 0 and station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenOverall)
	screenOverall.Charge = screenOverall.Discharge - screenOverall.Charge
	db.Table("bms_station").Select("count(*) as station").Where("owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)", userId, userId).Scan(&screenOverall)
	db.Table("bms_bcu").Select("count(*) as bcu").Where("station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenOverall)
	db.Table("bms_bmu").Select("count(*) as bmu").Where("station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenOverall)
	data := make(map[string]interface{})
	data["order"] = ItemInt{Name: "订单数目",  Value: screenOverall.Order}
	data["discharge"] = ItemFloat{Name: "放电量", Value: screenOverall.Discharge}
	data["charge"] = ItemFloat{Name: "充电量", Value: screenOverall.Charge}
	data["station"] = ItemInt{Name: "储能站数目", Value: screenOverall.Station}
	data["bcu"] = ItemInt{Name: "BCU数目", Value: screenOverall.BCU}
	data["bmu"] = ItemInt{Name: "BMU数目", Value: screenOverall.BMU}
	return data
}

func GetScreenStationStat(userId int) []ScreenStation {
	var screenStations []ScreenStation
	db.Table("bms_station").Select("id as station_id, name as station_name").Where("owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)", userId, userId).Scan(&screenStations)
	for index, screenStation := range screenStations {
		db.Raw("select avg(s.value) as soc from bms_bcu b left join bms_bcu_soc s on b.id = s.bcu_id where b.station_id = ?", screenStation.StationId).Scan(&screenStations[index])
	}
	return screenStations
}

func GetScreenBatteryStat(userId int) ScreenBattery {
	var screenBattery ScreenBattery
	db.Raw("select count(*) as abundant from bms_bcu b left join bms_bcu_soc s on b.id = s.bcu_id where s.soc > 60 and b.station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenBattery)
	db.Raw("select count(*) as medium from bms_bcu b left join bms_bcu_soc s on b.id = s.bcu_id where s.soc > 35 and s.soc < 60 and b.station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenBattery)
	db.Raw("select count(*) as less from bms_bcu b left join bms_bcu_soc s on b.id = s.bcu_id where s.soc > 10 and s.soc < 35 and b.station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenBattery)
	db.Raw("select count(*) as scare from bms_bcu b left join bms_bcu_soc s on b.id = s.bcu_id where s.soc < 10 and b.station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", userId, userId).Scan(&screenBattery)
	return screenBattery
}