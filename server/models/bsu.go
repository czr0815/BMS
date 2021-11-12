package models

type BSU struct {
	Model
	StationId int `json:"stationId" gorm:"station_id"`
	Status int `json:"status"`
}

func GetBSUsForUser(userId int) (bsus []BSU){
	var employments []Employment
	db.Table("bms_employment").Where("employer_id = ? or employee_id =?", userId, userId).Scan(&employments)
	for _, e := range employments {
		var tmp []BSU
		db.Where("station_id = ?", e.StationId).Find(&tmp)
		for _, t := range tmp {
			bsus = append(bsus, t)
		}
	}
	return
}

func GetBSUsByStationId(stationId int) (bsus []BSU){
	db.Table("bms_bsu").Where("station_id = ?", stationId).Scan(&bsus)
	return
}

func ExistBSUById(id int) bool {
	var bsu BSU
	db.Select("id").Where("id = ?", id).First(&bsu)
	return bsu.ID > 0
}

func GetBSUSlave(id int) (interface{}, string) {
	var bcus []BCU
	db.Table("bms_bcu").Where("bsu_id = ?", id).First(&bcus)
	if len(bcus) > 0 && bcus[0].ID > 0 {
		return  bcus, "bcu"
	}

	var bmus []BMU
	db.Table("bms_bmu").Where("bmu_id = ?", id).First(&bmus)
	if len(bmus) > 0 && bmus[0].ID > 0 {
		return  bmus, "bmu"
	}
	return nil, "nil"
}

func AddBSU(id, stationId int) bool {
	db.Create(&BSU{
		Model: Model{ID: id},
		StationId: stationId,
	})
	return true
}

func DeleteBSU(id int) bool {
	db.Where("id = ?", id).Delete(&BSU{})
	return true
}

func CheckDeleteBSU(id int) bool {
	var bmu BMU
	db.Select("id").Where("bsu_id = ?", id).First(&bmu)
	if bmu.ID > 0{
		return false
	}
	var bcu BCU
	db.Select("id").Where("bsu_id = ?", id).First(&bcu)
	if bcu.ID > 0{
		return false
	}
	return true
}

func UpdateBSU(id int, data interface{}) bool {
	db.Model(&BSU{}).Where("id = ?", id).Updates(data)
	return true
}
