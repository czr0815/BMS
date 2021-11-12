package models

import "time"

type BCUCurrent struct {
	ID int64 `gorm:"primary_key" json:"id"`
	BCUId int `json:"bmuId" gorm:"column:bcu_id"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

func GetBCUCurrentsByBCUId(bcuId int) (bcuCurrents []BCUCurrent) {
	db.Where("bcu_id = ?", bcuId).Find(&bcuCurrents)
	return
}

func AddBCUCurrent(bcuId int, value float64, time time.Time) bool {
	db.Create(&BCUCurrent{
		BCUId: bcuId,
		Value: value,
		CollTime: time,
	})
	return true
}
