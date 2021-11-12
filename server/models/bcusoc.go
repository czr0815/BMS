package models

import "time"

type BCUSoc struct {
	ID int64 `gorm:"primary_key" json:"id"`
	BCUId int `json:"bcuId" gorm:"column:bcu_id"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

func GetBCUSocsByBCUId(bcuId int) (bcuSocs []BCUSoc) {
	db.Where("bcu_id = ?", bcuId).Find(&bcuSocs)
	return
}

func AddBCUSoc(bcuId int, value float64, time time.Time) bool {
	db.Create(&BCUSoc{
		BCUId: bcuId,
		Value: value,
		CollTime: time,
	})
	return true
}
