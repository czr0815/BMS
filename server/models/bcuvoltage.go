package models

import "time"

type BCUVoltage struct {
	ID int64 `gorm:"primary_key" json:"id"`
	PointId int `json:"pointId" gorm:"column:point_id"`
	BCUId int `json:"bcuId" gorm:"column:bcu_id"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

const (
	PointB = 1
	PointHV1 = 2
	PointHV2 = 3
)

func GetBCUVoltagesByBCUId(bcuId int) (bcuVoltages []BCUVoltage) {
	db.Where("bcu_id = ?", bcuId).Find(&bcuVoltages)
	return
}

func AddBCUVoltage(pointId, bcuId int, value float64, time time.Time) bool {
	db.Create(&BCUVoltage{
		PointId: pointId,
		BCUId: bcuId,
		Value: value,
		CollTime: time,
	})
	return true
}