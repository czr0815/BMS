package models

import "time"

type BMUCurrent struct {
	ID int64 `gorm:"primary_key" json:"id"`
	BMUId int `json:"bmuId" gorm:"column:bmu_id"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

func GetBMUCurrentsByBMUId(bmuId int) (bmuCurrents []BMUCurrent) {
	db.Where("bmu_id = ?", bmuId).Find(&bmuCurrents)
	return
}

func AddBMUCurrent(bmuId int, value float64, time time.Time) bool {
	db.Create(&BMUCurrent{
		BMUId: bmuId,
		Value: value,
		CollTime: time,
	})
	return true
}