package models

import "time"

type BMUBattery struct {
	ID int64 `gorm:"primary_key" json:"id"`
	PointId int `json:"pointId" gorm:"column:point_id"`
	BMUId int `json:"bmuId" gorm:"column:bmu_id"`
	Voltage float64 `json:"vol" gorm:"column:voltage"`
	Soc float64 `json:"soc" gorm:"column:soc"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

type BMUBatteryRecent struct {
	PointId int `json:"pointId" gorm:"column:point_id;primary_key"`
	BMUId int `json:"bmuId" gorm:"column:bmu_id;primary_key"`
	Voltage float64 `json:"vol" gorm:"column:voltage"`
	Soc float64 `json:"soc" gorm:"column:soc"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time"`
}

func ExistBMUBatteryRecent(bmuId, pointId int) bool {
	var bmuBatteryRecent BMUBatteryRecent
	db.Select("bmu_id").Where("bmu_id = ? and point_id = ?", bmuId, pointId).First(&bmuBatteryRecent)
	return bmuBatteryRecent.BMUId > 0
}

func GetBMUBatteriesByBMUId(bmuId int) (bmuBatteries []BMUBattery) {
	db.Where("bmu_id = ?", bmuId).Find(&bmuBatteries)
	return
}

func AddBMUBattery(pointId, bmuId int, voltage, soc float64, time time.Time) bool {
	db.Create(&BMUBattery{
		PointId: pointId,
		BMUId: bmuId,
		Voltage: voltage,
		Soc: soc,
		CollTime: time,
	})
	return true
}

func AddBMUBatteryRecent(pointId, bmuId int, voltage, soc float64, time time.Time) bool {
	db.Create(&BMUBatteryRecent{
		PointId: pointId,
		BMUId: bmuId,
		Voltage: voltage,
		Soc: soc,
		CollTime: time,
	})
	return true
}

func UpdateBMUBatteryRecent(pointId, bmuId int, voltage, soc float64, time time.Time) bool {
	db.Model(&BMUBatteryRecent{}).Where("bmu_id = ? and point_id = ?", bmuId, pointId).Updates(BMUBatteryRecent{
		Voltage: voltage,
		Soc: soc,
		CollTime: time,
	})
	return true
}
