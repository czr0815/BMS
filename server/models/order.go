package models

import "time"

type Order struct {
	ID int64 `gorm:"primary_key" json:"id"`
	OrderType int `json:"orderType" gorm:"column:order_type"`
	StationId int `json:"stationId" gorm:"column:station_id;default:null"`
	BCUId int `json:"bcuId" gorm:"column:bcu_id;default:null"`
	Status int `json:"status"`
	Electricity float32 `json:"electricity"`
	StartTime time.Time `json:"startTime" gorm:"column:start_time;default:null"`
	EndTime time.Time `json:"endTime" gorm:"column:end_time;default:null"`
}

type OrderStationStat struct {
	StationId int `json:"stationId" gorm:"station_id"`
	StationName string `json:"stationName" gorm:"station_name"`
	ChargeSum float64 `json:"chargeSum" gorm:"charge_sum"`
	DischargeSum float64 `json:"dischargeSum" gorm:"discharge_sum"`
}

type OrderTmp struct {
	Charge float64
	Discharge float64
}

func ExistOrderById(id int) bool {
	var order Order
	db.Select("id").Where("id = ?", id).First(&order)
	return order.ID > 0
}

func GetOrderStationStat(userId int) []OrderStationStat {
	var orderStationStats []OrderStationStat
	db.Table("bms_station").Select("id as station_id, name as station_name").Where("owner_id = ? or id in (select station_id from bms_employment where employee_id = ?)", userId, userId).Scan(&orderStationStats)
	for index, orderStationStat := range orderStationStats {
		db.Table("bms_order").Select("sum(electricity) as discharge_sum").Where("order_type = 0 and bcu_id in (select id from bms_bcu where station_id = ?)", orderStationStat.StationId).Scan(&orderStationStats[index])
		db.Table("bms_order").Select("sum(electricity) as charge_sum").Where("order_type = 1 and bcu_id in (select id from bms_bcu where station_id = ?)", orderStationStat.StationId).Scan(&orderStationStats[index])
	}
	return orderStationStats
}

func GetOrderTimeStat(userId int) map[string]interface{} {
	var times [13]time.Time
	var timeStrings [12]string
	var discharges, charges [12]float64
	tmp := time.Now().AddDate(0, 0, -1)
	yesterday := time.Date(tmp.Year(), tmp.Month(), tmp.Day(), 0, 0, 0, 0, tmp.Location())
	for i := 0; i < 12; i++ {
		times[i] = yesterday.Add(time.Hour * time.Duration(i*2))
		timeStrings[i] = times[i].Format("15:04")
	}
	times[12] = yesterday.Add(time.Hour * time.Duration(24))
	for i := 11; i > 0; i-- {
		var orderTmp OrderTmp
		db.Table("bms_order").Select("sum(electricity) as discharge").Where("order_type=0 and end_time>? and end_time<? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", times[i-1], times[i], userId, userId).Scan(&orderTmp)
		db.Table("bms_order").Select("sum(electricity) as charge").Where("order_type=1 and end_time>? and end_time<? and bcu_id in (select id from bms_bcu where station_id in (select id from bms_station where owner_id = ? and id in (select station_id from bms_employment where employee_id = ?)))", times[i], times[i+1], userId, userId).Scan(&orderTmp)
		discharges[i] = orderTmp.Discharge
		charges[i] = orderTmp.Charge
	}
	data := make(map[string]interface{})
	data["xAxis"] = timeStrings
	data["discharges"] = discharges
	data["charges"] = charges
	return data
}

func GetUserOrders(userId int, orderType string, intStartTime, intEndTime int64) []Order {
	var userOrders, userTemps []Order
	if orderType == "discharge" || orderType == "all" {
		db.Table("bms_order").Select("*").Where("order_type = 0 and unix_timestamp(start_time)>=? and unix_timestamp(start_time)<=? and station_id in (select id from bms_station where owner_id = ? or id in (select id from bms_employment where employee_id = ?))", intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userOrders = append(userOrders, userTemps...)
	}
	if orderType == "charge" || orderType == "all" {
		db.Table("bms_order").Select("*").Where("order_type = 1 and unix_timestamp(start_time)>=? and unix_timestamp(start_time)<=? and station_id in (select id from bms_station where owner_id = ? or id in (select id from bms_employment where employee_id = ?))", intStartTime, intEndTime, userId, userId).Scan(&userTemps)
		userOrders = append(userOrders, userTemps...)
	}
	return userOrders
}

func GetStationOrders(stationId int, orderType string, intStartTime, intEndTime int64) []Order {
	var stationOrders, stationTemps []Order
	if orderType == "discharge" || orderType == "all" {
		db.Table("bms_order").Select("*").Where("order_type = 0 and unix_timestamp(start_time)>=? and unix_timestamp(start_time)<=? and station_id = ?", intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationOrders = append(stationOrders, stationTemps...)
	}
	if orderType == "charge" || orderType == "all" {
		db.Table("bms_order").Select("*").Where("order_type = 1 and unix_timestamp(start_time)>=? and unix_timestamp(start_time)<=? and station_id = ?", intStartTime, intEndTime, stationId).Scan(&stationTemps)
		stationOrders = append(stationOrders, stationTemps...)
	}
	return stationOrders
}

func GetBCUOrders(bcuId int, orderType string, intStartTime, intEndTime int64) []Order {
	var bcuOrders, bcuTemps []Order
	if orderType == "discharge" || orderType == "all" {
		db.Table("bms_order").Select("*").Where("order_type = 0 and unix_timestamp(start_time)>=? and unix_timestamp(start_time)<=? and bcu_id = ?", intStartTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuOrders = append(bcuOrders, bcuTemps...)
	}
	if orderType == "charge" || orderType == "all" {
		db.Table("bms_order").Select("*").Where("order_type = 1 and unix_timestamp(start_time)>=? and unix_timestamp(start_time)<=? and bcu_id = ?", intEndTime, intEndTime, bcuId).Scan(&bcuTemps)
		bcuOrders = append(bcuOrders, bcuTemps...)
	}
	return bcuOrders
}

func AddOrder(bcuId, stationId, orderType int) bool {
	db.Create(&Order{
		BCUId: bcuId,
		StationId: stationId,
		OrderType: orderType,
		Status: 0,
		Electricity: 0,
	})
	return true
}

func UpdateOrder(id int, data interface{}) bool {
	db.Model(&Order{}).Where("id = ?", id).Updates(data)
	return true
}

func DeleteOrder(id int) bool {
	db.Where("id = ?", id).Delete(&Order{})
	return true
}
