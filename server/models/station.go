package models

import (
	"sort"
	"time"
)

type Station struct {
	Model
	Name string `json:"name"`
	Position string `json:"position"`
	Lng float64 `json:"lng"`
	Lat float64 `json:"lat"`
	OwnerId int `json:"ownerId" gorm:"owner_id"`
	Status int `json:"status"`
	CreateTime time.Time `json:"createTime" gorm:"create_time"`
}

type StationInfo struct {
	Model
	Name string `json:"name"`
	Position string `json:"position"`
	Lng float64 `json:"lng"`
	Lat float64 `json:"lat"`
	OwnerId int `json:"ownerId" gorm:"owner_id"`
	EmployeeId int `json:"employeeId" gorm:"employee_id"`
	Status int `json:"status"`
	CreateTime time.Time `json:"createTime" gorm:"create_time"`
	Soc float64 `json:"soc"`
	Order int `json:"order"`
	OrderDeal int `json:"orderDeal"`
	Discharge float64 `json:"discharge"`
	Charge float64 `json:"charge"`
}

type StationDischargeScatter struct {
	Hour int `json:"hour"`
	Minute int `json:"minute"`

	Electricity float64 `json:"electricity"`
}
type StationIntRank struct {
	StationId int `json:"stationId" gorm:"station_id"`
	StationName string `json:"stationName" gorm:"station_name"`
	Count int `json:"count"`
	Status int `json:"status"`
}

type StationFloatRank struct {
	StationId int `json:"stationId" gorm:"station_id"`
	StationName string `json:"stationName" gorm:"station_name"`
	Count float64 `json:"count"`
	Status int `json:"status"`
}

func CheckStationAuthority(userId, stationId int) bool {
	var station Station
	db.Table("bms_station").Select("id").Where("id = ? and (owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", stationId, userId, userId).First(&station)
	return station.ID > 0
}

func CheckStationOwnerAuthority(userId, stationId int) bool {
	var station Station
	db.Table("bms_station").Select("id").Where("id = ? and owner_id = ?", stationId, userId).First(&station)
	return station.ID > 0
}

func GetStationById(stationId int) Station {
	var station Station
	db.Where("id = ?", stationId).First(&station)
	return station
}

func GetStationInfos(userId int) []StationInfo {
	var stationInfos []StationInfo
	db.Raw("select s.id as id, s.name as name, s.position as position, s.lng as lng, s.lat as lat, s.owner_id as owner_id, s.status as status, s.create_time as create_time from bms_station s left join bms_employment e on s.id = e.station_id where s.owner_id = ? or e.employee_id = ?", userId, userId).Scan(&stationInfos)
	for index, stationInfo := range stationInfos {
		db.Raw("select avg(s.value) as soc from bms_bcu b left join bms_bcu_soc s on b.id = s.bcu_id where b.station_id = ?", stationInfo.ID).Scan(&stationInfos[index])
		db.Table("bms_employment").Select("employee_id").Where("station_id = ?", stationInfo.ID).Scan(&stationInfos[index])
		db.Table("bms_order").Where("station_id = ? and timestampdiff(hour, start_time, now())<=72", stationInfo.ID).Count(&stationInfos[index].Order)
		db.Table("bms_order").Where("station_id = ? and status = 0 and timestampdiff(hour, start_time, now())<=72", stationInfo.ID).Count(&stationInfos[index].OrderDeal)
		db.Raw("select sum(electricity) as discharge from bms_order where station_id = ? and order_type = 0 and timestampdiff(hour, start_time, now())<=72", stationInfo.ID).Scan(&stationInfos[index])
		db.Raw("select sum(electricity) as charge from bms_order where station_id = ? and order_type = 1 and timestampdiff(hour, start_time, now())<=72", stationInfo.ID).Scan(&stationInfos[index])
	}
	return stationInfos
}

func GetStationDischargeScatter(stationId int) []StationDischargeScatter {
	var stationDischargeScatters []StationDischargeScatter
	db.Raw("select hour(start_time) as hour, minute(start_time) as minute, electricity from bms_order where bcu_id in (select id from bms_bcu where station_id = ?) and end_time != 0 and timestampdiff(hour, start_time, now())<=72", stationId).Scan(&stationDischargeScatters)
	return stationDischargeScatters
}

func GetStationRank(userId int) map[string]interface{} {
	var stations []Station
	var stationWarningRanks []StationIntRank
	var stationDischargeRanks, stationChargeRanks []StationFloatRank
	db.Raw("select * from bms_station where id in (select station_id from bms_employment where employee_id = ?) or owner_id = ?", userId, userId).Scan(&stations)
	var stationIntRank StationIntRank
	var stationFloatRank StationFloatRank
	for _, station := range stations {
		stationIntRank.StationId = station.ID
		stationIntRank.StationName = station.Name
		stationIntRank.Status = station.Status
		stationFloatRank.StationId = station.ID
		stationFloatRank.StationName = station.Name
		stationFloatRank.Status = station.Status
		stationIntRank.Count = 0
		db.Raw("select count(*) as count from bms_warning where bcu_id in (select id from bms_bcu where station_id = ?) or bmu_id in (select id from bms_bmu where station_id = ?)", station.ID, station.ID).Scan(&stationIntRank)
		stationWarningRanks = append(stationWarningRanks, stationIntRank)
		stationFloatRank.Count = 0
		db.Raw("select sum(electricity) as count from bms_order where bcu_id in (select id from bms_bcu where station_id = ?) and order_type = 0))", station.ID).Scan(&stationFloatRank)
		stationDischargeRanks = append(stationDischargeRanks, stationFloatRank)
		stationFloatRank.Count = 0
		db.Raw("select sum(electricity) as count from bms_order where bcu_id in (select id from bms_bcu where station_id = ?) and order_type = 1))", station.ID).Scan(&stationFloatRank)
		stationChargeRanks = append(stationChargeRanks, stationFloatRank)
	}
	var lenRank = len(stationWarningRanks)
	if lenRank < 3 {
		for i := 0; i < 3 - lenRank; i++ {
			stationWarningRanks = append(stationWarningRanks, StationIntRank{Status: 0, StationId: 0, Count: 0})
			stationDischargeRanks = append(stationDischargeRanks, StationFloatRank{Status: 0, StationId: 0, Count: 0})
			stationChargeRanks = append(stationChargeRanks, StationFloatRank{Status: 0, StationId: 0, Count: 0})
		}
	}
	sort.SliceStable(stationWarningRanks, func(i, j int) bool {
		if stationWarningRanks[i].Count < stationWarningRanks[j].Count {
			return false
		}
		return true
	})
	sort.SliceStable(stationDischargeRanks, func(i, j int) bool {
		if stationDischargeRanks[i].Count < stationDischargeRanks[j].Count {
			return false
		}
		return true
	})
	sort.SliceStable(stationChargeRanks, func(i, j int) bool {
		if stationChargeRanks[i].Count < stationChargeRanks[j].Count {
			return false
		}
		return true
	})
	data := make(map[string]interface{})
	data["warningRank"] = stationWarningRanks
	data["dischargeRank"] = stationDischargeRanks
	data["chargeRank"] = stationChargeRanks
	return data
}

func ExistStationByID(id int) bool {
	var station Station
	db.Select("id").Where("id = ?", id).First(&station)
	return station.ID > 0
}

func AddStation(name string, lng, lat float64, position string, ownerId, status int) int {
	station := Station{
		Name: name,
		Lng: lng,
		Lat: lat,
		Position: position,
		OwnerId: ownerId,
		Status: status,
	}
	db.Create(&station)
	return station.ID
}

func DeleteStation(id int) bool {
	db.Where("id = ?", id).Delete(&Station{})
	return true
}

func CheckDeleteStation(id int) bool {
	var bmu BMU
	db.Select("id").Where("station_id = ?", id).First(&bmu)
	if bmu.ID > 0{
		return false
	}
	var bcu BCU
	db.Select("id").Where("station_id = ?", id).First(&bcu)
	if bcu.ID > 0{
		return false
	}
	var bsu BSU
	db.Select("id").Where("station_id = ?", id).First(&bsu)
	if bsu.ID > 0{
		return false
	}
	return true
}

func UpdateStation(id int, data interface{}) bool {
	db.Model(&Station{}).Where("id = ?", id).Updates(data)
	return true
}
