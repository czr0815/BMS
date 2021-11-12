package models

import "time"

type BCUInsulation struct {
	ID int64 `gorm:"primary_key" json:"id"`
	BCUId int `json:"bcuId" gorm:"column:bcu_id"`
	PointId int `json:"pointId" gorm:"column:point_id"`
	Value float64 `json:"val" gorm:"column:value"`
	CollTime time.Time `json:"collTime" gorm:"column:coll_time;default:null"`
}

const (
	PositivePoint = 1
	NegativePoint = 2
)

func GetBCUInsulationsByBCUId(bcuId int) (bcuInsulations []BCUInsulation) {
	db.Table("bms_bcu_insulation").Where("bcu_id = ?", bcuId).Find(&bcuInsulations)
	return
}

func AddBCUInsulation(bcuId, pointId int, value float64, time time.Time) bool {
	db.Table("bms_bcu_insulation").Create(&BCUInsulation{
		BCUId: bcuId,
		PointId: pointId,
		Value: value,
		CollTime: time,
	})
	return true
}
