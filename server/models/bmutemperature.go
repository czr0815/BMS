package models

import "time"

type BMUTemperature struct {
	ID int64 `gorm:"primary_key" json:"id"`
	PointId int `json:"pointId" gorm:"column:point_id"`
	BMUId int `json:"bmuId" gorm:"column:bmu_id"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

type BMUTemperatureRecent struct {
	PointId int `json:"pointId" gorm:"column:point_id;primary"`
	BMUId int `json:"bmuId" gorm:"column:bmu_id;primary"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

func ExistBMUTemperatureRecent(bmuId, pointId int) bool {
	var bmuTemperatureRecent BMUTemperatureRecent
	db.Select("bmu_id").Where("bmu_id = ? and point_id = ?", bmuId, pointId).First(&bmuTemperatureRecent)
	return bmuTemperatureRecent.BMUId > 0
}

func GetBMUTemperaturesByBMUId(bmuId int) (bmuTemperatures []BMUTemperature) {
	db.Where("bmu_id = ?", bmuId).Find(&bmuTemperatures)
	return
}

func AddBMUTemperature(pointId, bmuId int, value float64, time time.Time) bool {
	db.Create(&BMUTemperature{
		PointId: pointId,
		BMUId: bmuId,
		Value: value,
		CollTime: time,
	})
	return true
}

func AddBMUTemperatureRecent(pointId, bmuId int, value float64, time time.Time) bool {
	db.Create(&BMUTemperatureRecent{
		PointId: pointId,
		BMUId: bmuId,
		Value: value,
		CollTime: time,
	})
	return true
}

func UpdateBMUTemperatureRecent(pointId, bmuId int, value float64, time time.Time) bool {
	db.Model(&BMUTemperatureRecent{}).Where("bmu_id = ? and point_id = ?", bmuId, pointId).Updates(BMUTemperatureRecent{
		Value: value,
		CollTime: time,
	})
	return true
}