package models

import "time"

type BCUSystem struct {
	BCUId int `gorm:"primary_key" json:"bcuId"`
	Online int `json:"online"`
	Charge int `json:"charge"`
	Cc int `json:"cc"`
	Heat int `json:"heat"`
	System int `json:"system"`
	Interior int `json:"interior"`
	Voltage float64 `json:"voltage"`
	Current float64 `json:"current"`
	UpdateTime time.Time `json:"updateTime" gorm:"column:update_time;default:null"`
}

func GetBCUSystemStat(bcuId int) BCUSystem {
	var bcuSystem BCUSystem
	db.Table("bms_bcu_system").Where("bcu_id = ?", bcuId).Find(&bcuSystem)
	return bcuSystem
}

func AddBCUSystem(bcuId, online, charge, cc, heat, system, interior int, voltage, current float64) bool {
	db.Create(&BCUSystem{
		BCUId: bcuId,
		Online: online,
		Charge: charge,
		Cc: cc,
		Heat: heat,
		System: system,
		Interior: interior,
		Voltage: voltage,
		Current: current,
	})
	return true
}

func UpdateBCUSystem(bcuId int, data interface{}) bool {
	db.Model(&BCUSystem{}).Where("bcu_id = ?", bcuId).Updates(data)
	return true
}