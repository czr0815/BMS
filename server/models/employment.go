package models

import (
	"time"
)

type Employment struct{
	Model
	EmployerId int `json:"employerId" gorm:"employer_id"`
	EmployeeId int `json:"employeeId" gorm:"employee_id"`
	StationId int `json:"stationId" gorm:"station_id"`
	StartTime time.Time `json:"startTime" gorm:"column:start_time;default:null"`
}

type APIEmployment struct {
	Model
	EmployerId int `json:"employerId" gorm:"employer_id"`
	EmployerName string `json:"employerName" gorm:"employer_name"`
	EmployerTelephone string `json:"employerTelephone" gorm:"employer_telephone"`
	EmployeeId int `json:"employeeId" gorm:"employee_id"`
	EmployeeName string `json:"employeeName" gorm:"employee_name"`
	EmployeeTelephone string `json:"employeeTelephone" gorm:"employee_telephone"`
	StationId int `json:"stationId" gorm:"station_id"`
	StationName string `json:"stationName" gorm:"station_name"`
	StartTime time.Time `json:"startTime" gorm:"start_time"`
}

func ExistEmploymentByInfo(stationId int) bool {
	var employment Employment
	db.Select("id").Where("station_id = ?", stationId).First(&employment)
	return employment.ID > 0
}

func GetEmployments(userId int) []APIEmployment {
	var apiEmployments []APIEmployment
	db.Table("bms_employment").Where("employer_id = ? or employee_id = ?", userId, userId).Scan(&apiEmployments)
	users := make(map[int]User)
	stations := make(map[int]Station)
	for index, apiEmployment := range apiEmployments {
		if user, ok := users[apiEmployment.EmployeeId]; ok {
			apiEmployments[index].EmployeeName = user.Username
			apiEmployments[index].EmployeeTelephone = user.Telephone
		} else {
			var user User
			db.Table("bms_user").Where("id = ?", apiEmployment.EmployeeId).First(&user)
			users[apiEmployment.EmployeeId] = user
			apiEmployments[index].EmployeeName = user.Username
			apiEmployments[index].EmployeeTelephone = user.Telephone
		}

		if user, ok := users[apiEmployment.EmployerId]; ok {
			apiEmployments[index].EmployerName = user.Username
			apiEmployments[index].EmployerTelephone = user.Telephone
		} else {
			var user User
			db.Table("bms_user").Where("id = ?", apiEmployment.EmployerId).First(&user)
			users[apiEmployment.EmployerId] = user
			apiEmployments[index].EmployerName = user.Username
			apiEmployments[index].EmployerTelephone = user.Telephone
		}

		if station, ok := stations[apiEmployment.StationId]; ok {
			apiEmployments[index].EmployeeName = station.Name
		} else {
			var station Station
			db.Table("bms_station").Where("id = ?", apiEmployment.StationId).First(&station)
			stations[apiEmployment.StationId] = station
			apiEmployments[index].StationName = station.Name
		}
	}
	return apiEmployments
}

func GetEmployees(employerId int) (apiEmployments []APIEmployment) {
	db.Raw("select bms_employment.id as id, bms_employment.employer_id as employer_id, bms_employment.employee_id as employee_id, bms_employment.station_id as station_id, bms_employment.start_time as start_time, bms_user.username as username, bms_user.telephone as telephone, bms_user.avatar as avatar, bms_user.des as des from bms_employment inner join bms_user on bms_employment.employee_id = bms_user.id where employer_id = ?", employerId).Scan(&apiEmployments)
	//db.Table("bms_employment").Select("bms_employment.id as id, bms_employment.employer_id as employer_id, bms_employment.employee_id as employee_id, bms_employment.station_id as station_id, bms_employment.start_time as start_time, bms_user.username as username, bms_user.telephone as telephone, bms_user.avatar as avatar, bms_user.desc as desc").Joins("inner join bms_user on bms_employment.employee_id = bms_user.id where bms_employment.employer_id = ?", employerId).Scan(&apiEmployments)
	return
}

func GetEmployers(employeeId int) (apiEmployments []APIEmployment) {
	db.Raw("select bms_employment.id as id, bms_employment.employer_id as employer_id, bms_employment.employee_id as employee_id, bms_employment.station_id as station_id, bms_employment.start_time as start_time, bms_user.username as username, bms_user.telephone as telephone, bms_user.avatar as avatar, bms_user.des as des from bms_employment inner join bms_user on bms_employment.employer_id = bms_user.id where employee_id = ?", employeeId).Scan(&apiEmployments)
	//db.Table("bms_employment").Select("bms_employment.id as id, bms_employment.employer_id as employer_id, bms_employment.employee_id as employee_id, bms_employment.station_id as station_id, bms_employment.start_time as start_time, bms_user.username as username, bms_user.telephone as telephone, bms_user.avatar as avatar, bms_user.desc as desc").Joins("inner join bms_user on bms_employment.employer_id = bms_user.id where bms_employment.employee_id = ?", employeeId).Scan(&apiEmployments)
	return
}

func AddEmployment(employerId, employeeId, stationId int) bool {
	db.Create(&Employment{
		EmployerId: employerId,
		EmployeeId: employeeId,
		StationId: stationId,
	})
	return true
}

func DeleteEmployment(id int) bool {
	db.Where("id = ?", id).Delete(&Employment{})
	return true
}

func UpdateEmployment(ownerId, stationId int, data interface{}) bool {
	db.Model(&Employment{}).Where("employer_id = ? and station_id = ?", ownerId, stationId).Updates(data)
	return true
}