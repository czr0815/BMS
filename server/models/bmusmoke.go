package models

import "time"

type BMUSmoke struct {
	ID int64 `gorm:"primary_key" json:"id"`
	PointId int `json:"pointId" gorm:"column:point_id"`
	BMUId int `json:"bmuId" gorm:"column:bmu_id"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

func GetBMUSmokesByBMUId(bmuId int) (bmuSmokes []BMUSmoke) {
	db.Where("bmu_id = ?", bmuId).Find(&bmuSmokes)
	return
}

func AddBMUSmoke(pointId, bmuId int, value float64, time time.Time) bool {
	db.Create(&BMUSmoke{
		PointId: pointId,
		BMUId: bmuId,
		Value: value,
		CollTime: time,
	})
	return true
}
