package models

import (
	"sort"
	"time"
)

type BCU struct {
	Model
	StationId int `json:"stationId" gorm:"station_id"`
	BSUId int `json:"bsuId" gorm:"column:bsu_id;default:null"`
	Name string `json:"name"`
	Status int `json:"status"`
	OnlineTime time.Time `json:"onlineTime" gorm:"column:online_time"`
}

type BCUInfo struct {
	ID          int     `json:"id" gorm:"id"`
	StationId   int     `json:"stationId" gorm:"station_id"`
	BSUId       int     `json:"bsuId" gorm:"column:bsu_id;default:null"`
	Name        string  `json:"name"`
	BMUNum      int     `json:"bmuNum"`
	Status      int     `json:"status"`
	Order       int     `json:"order"`
	Charge      int     `json:"charge"`
	OnlineTime  time.Time  `json:"onlineTime" gorm:"online_time"`
	Warning     int      `json:"warning" gorm:"warning"`
	Soc         float64  `json:"soc"`
	B           float64  `json:"b"`
	Hv1         float64  `json:"hv1"`
	Hv2         float64  `json:"hv2"`
	Current     float64  `json:"current"`
	Positive    float64  `json:"positive"`
	Negative    float64  `json:"negative"`
}

type BCUIntRank struct {
	BCUId int `json:"bcuId" gorm:"bcu_id"`
	BCUName string `json:"bcuName" gorm:"bcu_name"`
	Count int `json:"count"`
	Status int `json:"status"`
}

type BCUFloatRank struct {
	BCUId int `json:"bcuId" gorm:"bcu_id"`
	BCUName string `json:"bcuName" gorm:"bcu_name"`
	Count float64 `json:"count"`
	Status int `json:"status"`
}

type BCUSingle struct {
	Max float64 `json:"max" gorm:"max"`
	Min float64 `json:"min" gorm:"min"`
	Avg float64 `json:"avg" gorm:"avg"`
}

func CheckBCUAuthority(userId, bcuId int) bool {
	var bcu BCU
	db.Table("bms_bcu").Select("id").Where("id = ? and station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", bcuId, userId, userId).First(&bcu)
	return bcu.ID > 0
}

func GetStationForBCU(bcuId int) Station {
	var station Station
	db.Table("bms_bcu").Select("station_id aS id").Where("id = ?", bcuId).Scan(&station)
	return station
}

func GetBCUById(bcuId int) BCU {
	var bcu BCU
	db.Where("id = ?", bcuId).First(&bcu)
	return bcu
}

func GetBCUInfos(userId int) []BCUInfo {
	var bcuInfos []BCUInfo
	db.Raw("select * from bms_bcu where station_id in (select station_id from bms_station s left join bms_employment e on s.id = e.station_id where s.owner_id = ? or e.employee_id = ?)", userId, userId).Scan(&bcuInfos)
	for index, bcuInfo := range bcuInfos {
		db.Table("bms_bcu_voltage").Select("value as b").Where("bcu_id=? and point_id=? and coll_time=(select max(coll_time) from bms_bcu_voltage where bcu_id=?)", bcuInfo.ID, PointB, bcuInfo.ID).Scan(&bcuInfos[index])
		db.Table("bms_bcu_voltage").Select("value as hv1").Where("bcu_id=? and point_id=? and coll_time=(select max(coll_time) from bms_bcu_voltage where bcu_id=?)", bcuInfo.ID, PointHV1, bcuInfo.ID).Scan(&bcuInfos[index])
		db.Table("bms_bcu_voltage").Select("value as hv2").Where("bcu_id=? and point_id=? and coll_time=(select max(coll_time) from bms_bcu_voltage where bcu_id=?)", bcuInfo.ID, PointHV2, bcuInfo.ID).Scan(&bcuInfos[index])
		db.Table("bms_bcu_soc").Select("value as soc").Where("bcu_id=? and coll_time=(select max(coll_time) from bms_bcu_soc where bcu_id=?)", bcuInfo.ID, bcuInfo.ID).Scan(&bcuInfos[index])
		db.Table("bms_bcu_current").Select("value as current").Where("bcu_id=? and coll_time=(select max(coll_time) from bms_bcu_current where bcu_id=?)", bcuInfo.ID, bcuInfo.ID).Scan(&bcuInfos[index])
		db.Table("bms_bcu_insulation").Select("value as positive").Where("bcu_id=? and point_id=? and coll_time=(select max(coll_time) from bms_bcu_insulation where bcu_id=?)", bcuInfo.ID, PositivePoint, bcuInfo.ID).Scan(&bcuInfos[index])
		db.Table("bms_bcu_insulation").Select("value as negative").Where("bcu_id=? and point_id=? and coll_time=(select max(coll_time) from bms_bcu_insulation where bcu_id=?)", bcuInfo.ID, NegativePoint, bcuInfo.ID).Scan(&bcuInfos[index])
		db.Table("bms_bmu").Where("bcu_id = ?", bcuInfo.ID).Count(&bcuInfos[index].BMUNum)
	}
	return bcuInfos
}

func GetBCURank(stationId int) map[string]interface{} {
	var bcus []BCU
	var bcuWarningRanks, bcuOrderRanks []BCUIntRank
	var bcuDischargeRanks []BCUFloatRank
	db.Table("bms_bcu").Where("station_id = ?", stationId).Scan(&bcus)
	var bcuIntRank BCUIntRank
	var bcuFloatRank BCUFloatRank
	for _, bcu := range bcus {
		bcuIntRank.BCUId = bcu.ID
		bcuIntRank.BCUName = bcu.Name
		bcuIntRank.Status = bcu.Status
		bcuFloatRank.BCUId = bcu.ID
		bcuFloatRank.BCUName = bcu.Name
		bcuFloatRank.Status = bcu.Status
		bcuIntRank.Count = 0
		db.Table("bms_warning").Where("bcu_id = ? and timestampdiff(hour, coll_time, now())<=24", bcu.ID).Count(&bcuIntRank.Count)
		bcuWarningRanks = append(bcuWarningRanks, bcuIntRank)
		bcuIntRank.Count = 0
		db.Table("bms_order").Where("bcu_id = ? and timestampdiff(hour, start_time, now())<=24", bcu.ID).Count(&bcuIntRank.Count)
		bcuOrderRanks = append(bcuOrderRanks, bcuIntRank)
		bcuFloatRank.Count = 0
		db.Table("bms_order").Select("sum(electricity) as count").Where("bcu_id = ? and timestampdiff(hour, start_time, now())<=24", bcu.ID).Scan(&bcuFloatRank)
		bcuDischargeRanks = append(bcuDischargeRanks, bcuFloatRank)
	}
	var lenRank = len(bcuWarningRanks)
	if lenRank < 3 {
		for i := 0; i < 3 - lenRank; i++ {
			bcuWarningRanks = append(bcuWarningRanks, BCUIntRank{Status: 0, BCUId: 0, Count: 0})
			bcuOrderRanks = append(bcuOrderRanks, BCUIntRank{Status: 0, BCUId: 0, Count: 0})
			bcuDischargeRanks = append(bcuDischargeRanks, BCUFloatRank{Status: 0, BCUId: 0, Count: 0})
		}
	}
	sort.SliceStable(bcuWarningRanks, func(i, j int) bool {
		if bcuWarningRanks[i].Count < bcuWarningRanks[j].Count {
			return false
		}
		return true
	})
	sort.SliceStable(bcuOrderRanks, func(i, j int) bool {
		if bcuOrderRanks[i].Count < bcuOrderRanks[j].Count {
			return false
		}
		return true
	})
	sort.SliceStable(bcuDischargeRanks, func(i, j int) bool {
		if bcuDischargeRanks[i].Count < bcuDischargeRanks[j].Count {
			return false
		}
		return true
	})
	data := make(map[string]interface{})
	data["warningRank"] = bcuWarningRanks
	data["orderRank"] = bcuOrderRanks
	data["dischargeRank"] = bcuDischargeRanks
	return data
}

func GetBCUDataStat(bcuId int) map[string]interface{} {
	var times [12]time.Time
	var timeStrings [12]string
	var uploads, warnings, orders [12]int
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
		var tmp1, tmp2, tmp3 int
		//可优化
		db.Table("bms_bcu_voltage").Where("bcu_id=? and coll_time>? and coll_time<?", bcuId, times[i-1], times[i]).Count(&tmp1)
		db.Table("bms_bcu_soc").Where("bcu_id=? and coll_time>? and coll_time<?", bcuId, times[i-1], times[i]).Count(&tmp2)
		db.Table("bms_bcu_insulation").Where("bcu_id=? and coll_time>? and coll_time<?", bcuId, times[i-1], times[i]).Count(&tmp3)
		uploads[i] = tmp1 + tmp2 +tmp3
		db.Table("bms_warning").Where("bcu_id=? and coll_time>? and coll_time<?", bcuId, times[i-1], times[i]).Count(&warnings[i])
		db.Table("bms_order").Where("bcu_id=? and start_time>? and start_time<?", bcuId, times[i-1], times[i]).Count(&orders[i])
	}
	data := make(map[string]interface{})
	data["xAxis"] = timeStrings
	data["orders"] = orders
	data["warnings"] = warnings
	data["uploads"] = uploads
	return data
}

func GetBCUWarningStat(bcuId int) [3]map[string]interface{} {
	var handled, slight, serious int
	db.Table("bms_warning").Where("bcu_id = ? and status = 0 and timestampdiff(hour, coll_time, now())<=24", bcuId).Count(&handled)
	db.Table("bms_warning").Where("bcu_id = ? and status = 1 and timestampdiff(hour, coll_time, now())<=24", bcuId).Count(&slight)
	db.Table("bms_warning").Where("bcu_id = ? and status = 2 and timestampdiff(hour, coll_time, now())<=24", bcuId).Count(&serious)
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

func GetBCUSingleStat(bcuId int) map[string]interface{} {
	var bmus []BMU
	var bcuVoltageSingle, tmpVoltageSingle, bcuTempertaureSingle, tmpTemperatureSingle BCUSingle
	db.Table("bms_bmu").Select("id").Where("bcu_id=?", bcuId).Scan(&bmus)
	for index, bmu := range bmus {
		db.Table("bms_bmu_battery_recent").Select("avg(voltage) as avg, max(voltage) as max, min(voltage) as min").Where("bmu_id=?", bmu.ID).Scan(&tmpVoltageSingle)
		if tmpVoltageSingle.Max > bcuVoltageSingle.Max {
			bcuVoltageSingle.Max = tmpVoltageSingle.Max
		}
		if tmpVoltageSingle.Min < bcuVoltageSingle.Min {
			bcuVoltageSingle.Min = tmpVoltageSingle.Min
		}
		bcuVoltageSingle.Avg = (bcuVoltageSingle.Avg * float64(index) + tmpVoltageSingle.Avg) / (float64(index) + 1)

		db.Table("bms_bmu_temperature_recent").Select("avg(value) as avg, max(value) as max, min(value) as min").Where("bmu_id = ?", bmu.ID).Scan(&tmpTemperatureSingle)
		if tmpTemperatureSingle.Max > bcuTempertaureSingle.Max {
			bcuTempertaureSingle.Max = tmpTemperatureSingle.Max
		}
		if tmpTemperatureSingle.Min > bcuTempertaureSingle.Min {
			bcuTempertaureSingle.Min = tmpTemperatureSingle.Min
		}
		bcuTempertaureSingle.Avg = (bcuTempertaureSingle.Avg * float64(index) + tmpTemperatureSingle.Avg) / (float64(index) + 1)
	}
	data := make(map[string]interface{})
	data["voltage"] = bcuVoltageSingle
	data["temperature"] = bcuTempertaureSingle
	return data
}

func ExistBCUById(id int) bool {
	var bcu BCU
	db.Select("id").Where("id = ?", id).First(&bcu)
	return bcu.ID > 0
}

func AddBCU(id, stationId int, name string) bool {
	db.Create(&BCU{
		Model: Model{ID: id},
		StationId: stationId,
		Name: name,
	})
	return true
}

func AddBCUComplete(id, stationId, bsuId int) bool {
	db.Create(&BCU{
		Model: Model{ID: id},
		StationId: stationId,
		BSUId: bsuId,
	})
	return true
}

func DeleteBCU(id int) bool {
	db.Where("id = ?", id).Delete(&BCU{})
	return true
}

func CheckDeleteBCU(id int) bool {
	var bmu BMU
	db.Select("id").Where("bcu_id = ?", id).First(&bmu)
	if bmu.ID > 0{
		return false
	}
	return true
}

func UpdateBCU(id int, data interface{}) bool {
	db.Model(&BCU{}).Where("id = ?", id).Updates(data)
	return true
}