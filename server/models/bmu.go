package models

import "time"

type BMU struct {
	Model
	StationId   int     `json:"stationId" gorm:"station_id"`
	BSUId       int     `json:"bsuId" gorm:"column:bsu_id;default:null"`
	BCUId       int     `json:"bcuId" gorm:"column:bcu_id;default:null"`
	Name        string  `json:"name"`
	BatteryNum  int     `json:"batteryNum" gorm:"battery_num"`
	BatteryType int     `json:"batteryType" gorm:"battery_type"`
	Status      int     `json:"status"`
	OnlineTime  time.Time  `json:"onlineTime" gorm:"online_time"`
}

type BMUInfo struct {
	Model
	StationId   int     `json:"stationId" gorm:"station_id"`
	BSUId       int     `json:"bsuId" gorm:"column:bsu_id;default:null"`
	BCUId       int     `json:"bcuId" gorm:"column:bcu_id;default:null"`
	Name        string  `json:"name"`
	BatteryNum  int     `json:"batteryNum" gorm:"battery_num"`
	BatteryType int     `json:"batteryType" gorm:"battery_type"`
	Status      int     `json:"status"`
	OnlineTime  time.Time  `json:"onlineTime" gorm:"online_time"`
	Warning     int      `json:"warning" gorm:"warning"`
	Voltage       BMUStatistics   `json:"voltage"`
	Soc           BMUStatistics   `json:"soc"`
	Current       BMUStatistics   `json:"current"`
	Smoke         BMUStatistics   `json:"smoke"`
	Temperature   BMUStatistics    `json:"temperature"`
}

type BMUWarningCalendar struct {
	Date string `json:"date" gorm:"date"`
	Value int `json:"value"`
}

type BMUSOCStat struct {
	Value int `json:"value"`
	Name string `json:"name"`
}

type BMUSingle struct {
	ValueType string `json:"valueType" gorm:"value_type"`
	PointId int `json:"pointId" gorm:"point_id"`
	Value float64 `json:"value"`
	CollTime time.Time `json:"collTime" gorm:"coll_time"`
}

type BMUStatistics struct {
	Max float64 `json:"max" gorm:"max"`
	Min float64 `json:"min" gorm:"min"`
	Avg float64 `json:"avg" gorm:"avg"`
	CollTime time.Time `json:"collTime" gorm:"coll_time"`
}

type BMUOutline struct {
	Voltage float64 `json:"voltage"`
	Soc float64 `json:"soc"`
	Current float64 `json:"current"`
	Smoke float64 `json:"smoke"`
	Temperature float64 `json:"temperature"`
}

func GetBMUById(id int) BMU {
	var bmu BMU
	db.Where("id = ?", id).First(&bmu)
	return bmu
}

func CheckBMUAuthority(userId, bmuId int) bool {
	var bmu BMU
	db.Table("bms_bmu").Select("id").Where("id = ? and station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", bmuId, userId, userId).First(&bmu)
	return bmu.ID > 0
}

func GetBMUInfos(userId int) []BMUInfo {
	var bmuInfos []BMUInfo
	db.Raw("select * from bms_bmu where station_id in (select station_id from bms_station s left join bms_employment e on s.id = e.station_id where s.owner_id = ? or e.employee_id = ?)", userId, userId).Scan(&bmuInfos)
	for index, bmuInfo := range bmuInfos {
		//之后修改：可能会存在对象查找不到的情况
		db.Raw("select max(voltage) as max,min(voltage) as min,avg(voltage) as avg,b.coll_time from (select max(coll_time) as t from bms_bmu_battery where bmu_id=?) a left join bms_bmu_battery b on a.t = b.coll_time where bmu_id=?", bmuInfo.ID, bmuInfo.ID).First(&bmuInfos[index].Voltage)
		db.Raw("select max(soc) as max,min(soc) as min,avg(soc) as avg,b.coll_time from (select max(coll_time) as t from bms_bmu_battery where bmu_id=?) a left join bms_bmu_battery b on a.t = b.coll_time where bmu_id=?", bmuInfo.ID, bmuInfo.ID).First(&bmuInfos[index].Soc)
		db.Raw("select max(value) as max,min(value) as min,avg(value) as avg,b.coll_time from (select max(coll_time) as t from bms_bmu_current where bmu_id=?) a left join bms_bmu_current b on a.t = b.coll_time where bmu_id=?", bmuInfo.ID, bmuInfo.ID).First(&bmuInfos[index].Current)
		db.Raw("select max(value) as max,min(value) as min,avg(value) as avg,b.coll_time from (select max(coll_time) as t from bms_bmu_smoke where bmu_id=?) a left join bms_bmu_smoke b on a.t = b.coll_time where bmu_id=?", bmuInfo.ID, bmuInfo.ID).First(&bmuInfos[index].Smoke)
		db.Raw("select max(value) as max,min(value) as min,avg(value) as avg,b.coll_time from (select max(coll_time) as t from bms_bmu_temperature where bmu_id=?) a left join bms_bmu_temperature b on a.t = b.coll_time where bmu_id=?", bmuInfo.ID, bmuInfo.ID).First(&bmuInfos[index].Temperature)
		db.Table("bms_warning").Where("bmu_id=? and status=1", bmuInfo.ID).Count(&bmuInfo.Warning)
	}
	return bmuInfos
}

func GetBMUDataStat(bmuId int) map[string]interface{}{
	var times [12]time.Time
	var timeStrings [12]string
	var batteries, currents, temperatures, smokes, warnings [12]int
	var flag = false
	_, _, dayNow := time.Now().Date()
	for i := 11; i >= 0; i-- {
		times[i] = time.Now().Add(-time.Hour * time.Duration(11-i))
		_, _, day := times[i].Date()
		if day != dayNow && !flag {
			flag = true
			timeStrings[i] = times[i].Format("01-02 15:04")
		} else {
			timeStrings[i] = times[i].Format("15:04")
		}
	}
	for i := 11; i > 0; i-- {
		//var countBattery, countCurrent, countTemperature, countSmoke int
		//可优化
		db.Table("bms_bmu_battery").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Count(&batteries[i])
		db.Table("bms_bmu_current").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Count(&currents[i])
		db.Table("bms_bmu_smoke").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Count(&smokes[i])
		db.Table("bms_bmu_temperature").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Count(&temperatures[i])
		//uploads[i] = countBattery + countCurrent + countSmoke
		db.Table("bms_warning").Where("bcu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Count(&warnings[i])
	}
	/*
		for i := 11; i > 0; i-- {
			uploads[i] = 3 * i
			orders[i] = i
			warnings[i] = i + 6
		}
	*/
	data := make(map[string]interface{})
	data["xAxis"] = timeStrings
	data["batteries"] = batteries
	data["currents"] = currents
	data["smokes"] = smokes
	data["temperatures"] = temperatures
	data["warnings"] = warnings
	//data["uploads"] = uploads
	return data
}

func GetBMUWarningStat(bmuId int) [3]map[string]interface{}{
	var handled, slight, serious int
	db.Table("bms_warning").Where("bmu_id = ? and status = 0 and timestampdiff(hour, coll_time, now())<=24", bmuId).Count(&handled)
	db.Table("bms_warning").Where("bmu_id = ? and status = 1 and timestampdiff(hour, coll_time, now())<=24", bmuId).Count(&slight)
	db.Table("bms_warning").Where("bmu_id = ? and status = 2 and timestampdiff(hour, coll_time, now())<=24", bmuId).Count(&serious)
	var data [3]map[string]interface{}
	data[0] = make(map[string]interface{})
	data[0]["name"] = "已处理"
	data[0]["value"] = handled
	data[1] = make(map[string]interface{})
	data[1]["name"] = "轻微异常"
	data[1]["value"] = slight
	data[2] = make(map[string]interface{})
	data[2]["name"] = "严重异常"
	data[2]["value"] = serious
	return data
}

func GetBMUWarningCalendar(bmuId int) []BMUWarningCalendar{
	var bmuWarningCalendars []BMUWarningCalendar
	db.Raw("select count(*) as value, date_format(coll_time, '%d') as date from bms_warning where bmu_id = ? and date_format(coll_time, '%d') = date_format(now(), '%d') group by date_format(coll_time, '%d')", bmuId).Scan(&bmuWarningCalendars)
	return bmuWarningCalendars
}

func GetBMUSOCStat(bmuId int) []BMUSOCStat{
	var bmuSOCStats []BMUSOCStat
	var bmuSOCStat20, bmuSOCStat50, bmuSOCStat80, bmuSOCStat100 BMUSOCStat
	db.Table("bms_bmu_battery_recent").Select("count(*) as value, '<20%' as name").Where("bmu_id = ? and soc < 20", bmuId).Scan(&bmuSOCStat20)
	db.Table("bms_bmu_battery_recent").Select("count(*) as value, '20~50%' as name").Where("bmu_id = ? and soc >= 20 and soc < 50", bmuId).Scan(&bmuSOCStat50)
	db.Table("bms_bmu_battery_recent").Select("count(*) as value, '50~80%' as name").Where("bmu_id = ? and soc >= 50 and soc < 80", bmuId).Scan(&bmuSOCStat80)
	db.Table("bms_bmu_battery_recent").Select("count(*) as value, '>80%' as name").Where("bmu_id = ? and soc >= 80", bmuId).Scan(&bmuSOCStat100)
	bmuSOCStats = append(bmuSOCStats, bmuSOCStat20, bmuSOCStat50, bmuSOCStat80, bmuSOCStat100)
	var num = 0
	var total = GetBMUById(bmuId).BatteryNum
	for _, bmuSOCStat := range bmuSOCStats {
		num += bmuSOCStat.Value
	}
	if num < total {
		bmuSOCStats = append(bmuSOCStats, BMUSOCStat{Name: "unknown", Value: total - num})
	}
	return bmuSOCStats
}

func GetBMUOutlineStat(bmuId int) map[string]interface{} {
	var times [12]time.Time
	var timeStrings [12]string
	var voltages, socs, currents, smokes, temperatures [12]float64
	var flag = false
	var bmuOutline BMUOutline
	_, _, dayNow := time.Now().Date()
	for i := 11; i >= 0; i-- {
		times[i] = time.Now().Add(-time.Hour * time.Duration(11-i))
		_, _, day := times[i].Date()
		if day != dayNow && !flag {
			flag = true
			timeStrings[i] = times[i].Format("01-02 15:04")
		} else {
			timeStrings[i] = times[i].Format("15:04")
		}
	}
	for i := 11; i > 0; i-- {
		//可优化
		db.Table("bms_bmu_battery").Select("avg(voltage) as voltage").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Scan(&bmuOutline)
		db.Table("bms_bmu_battery").Select("avg(soc) as soc").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Take(&bmuOutline)
		db.Table("bms_bmu_current").Select("avg(value) as current").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Take(&bmuOutline)
		db.Table("bms_bmu_smoke").Select("avg(value) as smoke").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Take(&bmuOutline)
		db.Table("bms_bmu_temperature").Select("avg(value) as temperature").Where("bmu_id=? and coll_time>? and coll_time<?", bmuId, times[i-1], times[i]).Take(&bmuOutline)
		voltages[i] = bmuOutline.Voltage
		socs[i] = bmuOutline.Soc
		currents[i] = bmuOutline.Current
		smokes[i] = bmuOutline.Smoke
		temperatures[i] = bmuOutline.Temperature
	}
	data := make(map[string]interface{})
	data["xAxis"] = timeStrings
	data["voltages"] = voltages
	data["socs"] = socs
	data["currents"] = currents
	data["smokes"] = smokes
	data["temperatures"] = temperatures
	return data
}

func GetBMUSingles(bmuId int) []BMUSingle {
	var bmuSingles, voltageSingles, socSingles, temperatureSingles, currentSingles, smokeSingles []BMUSingle
	db.Table("bms_bmu_battery_recent").Select("point_id, bmu_id, voltage as value, coll_time, 'Voltage' as value_type").Where("bmu_id = ?", bmuId).Scan(&voltageSingles)
	bmuSingles = append(bmuSingles, voltageSingles...)
	db.Table("bms_bmu_battery_recent").Select("point_id, bmu_id, soc as value, coll_time, 'SOC' as value_type").Where("bmu_id = ?", bmuId).Scan(&socSingles)
	bmuSingles = append(bmuSingles, socSingles...)
	db.Table("bms_bmu_temperature_recent").Select("*, 'Temperature' as value_type").Where("bmu_id = ?", bmuId).Scan(&temperatureSingles)
	bmuSingles = append(bmuSingles, temperatureSingles...)
	db.Table("bms_bmu_current").Select("*, 'Current' as value_type").Where("bmu_id = ? and coll_time = (select max(coll_time) from bms_bmu_current where bmu_id=?)", bmuId, bmuId).Scan(&currentSingles)
	bmuSingles = append(bmuSingles, currentSingles...)
	db.Table("bms_bmu_smoke").Select("*, 'Smoke' as value_type").Where("bmu_id = ? and coll_time = (select max(coll_time) from bms_bmu_smoke where bmu_id=?)", bmuId, bmuId).Scan(&smokeSingles)
	//db.Raw("select *, 'Smoke' as value_type from (select max(coll_time) as t from bms_bmu_smoke where bmu_id=? group by coll_time) a left join bms_bmu_smoke b on a.t = b.coll_time where bmu_id=?", bmuId, bmuId).First(&smokeSingles)
	bmuSingles = append(bmuSingles, smokeSingles...)
	return bmuSingles
}

func ExistBMUById(id int) bool {
	var bmu BMU
	db.Select("id").Where("id = ?", id).First(&bmu)
	return bmu.ID > 0
}

func AddBMU(id, bcuId, stationId, batteryNum, batteryType int) bool {
	db.Create(&BMU{
		Model: Model{ID: id},
		BCUId: bcuId,
		StationId: stationId,
		BatteryNum: batteryNum,
		BatteryType: batteryType,
	})
	return true
}

func AddBMUWithBSU(id, stationId, bsuId, batteryNum, batteryType int) bool {
	db.Create(&BMU{
		Model: Model{ID: id},
		StationId: stationId,
		BSUId: bsuId,
		BatteryNum: batteryNum,
		BatteryType: batteryType,
	})
	return true
}

func AddBMUWithBCU(id, stationId, bcuId, batteryNum, batteryType int) bool {
	db.Create(&BMU{
		Model: Model{ID: id},
		StationId: stationId,
		BCUId: bcuId,
		BatteryNum: batteryNum,
		BatteryType: batteryType,
	})
	return true
}

func AddBMUComplete(id, stationId, bsuId, bcuId, batteryNum, batteryType int) bool {
	db.Create(&BMU{
		Model: Model{ID: id},
		StationId: stationId,
		BSUId: bsuId,
		BCUId: bcuId,
		BatteryNum: batteryNum,
		BatteryType: batteryType,
	})
	return true
}

func DeleteBMU(id int) bool {
	db.Where("id = ?", id).Delete(&BMU{})
	return true
}

func UpdateBMU(id int, data interface{}) bool {
	db.Model(&BMU{}).Where("id = ?", id).Updates(data)
	return true
}

func UpdateBMUStatus(id int, status int) bool {
	db.Model(&BMU{}).Where("id = ?", id).Update("status", status)
	return true
}