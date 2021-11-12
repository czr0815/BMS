package models

type User struct {
	Model
	Username string `json:"username"`
	Password string `json:"password"`
	Telephone string `json:"telephone"`
	Avatar int `json:"avatar"`
	Des string `json:"des"`
}

type UserInfo struct {
	Model
	Username string `json:"username"`
	Telephone string `json:"telephone"`
	Avatar int `json:"avatar"`
	Des string `json:"des"`
	OwnNum int `json:"ownNum"`
	ManageNum int `json:"manageNum"`
	Discharge float64 `json:"discharge"`
	Charge float64 `json:"charge"`
}

func Login(telephone, password string) (User, bool) {
	var user User
	db.Where("telephone = ? AND password = ?", telephone, password).First(&user)
	return user, user.ID > 0
}

func GetUserInfo(id int) UserInfo {
	var userInfo UserInfo
	db.Table("bms_user").Where("id = ?", id).Scan(&userInfo)
	db.Table("bms_station").Where("owner_id = ?", id).Count(&userInfo.OwnNum)
	db.Table("bms_employment").Where("employee_id = ?", id).Count(&userInfo.ManageNum)
	db.Table("bms_order").Select("sum(electricity) as discharge").Where("order_type = 0 and station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", id, id).Scan(&userInfo)
	db.Table("bms_order").Select("sum(electricity) as charge").Where("order_type = 1 and station_id in (select id from bms_station where owner_id = ? or id in (select station_id from bms_employment where employee_id = ?))", id, id).Scan(&userInfo)
	return userInfo
}

func GetUserByTelephone(telephone string) User {
	var user User
	db.Where("telephone = ?").First(&user)
	return user
}

func ExistUserByID(id int) bool {
	var user User
	db.Select("id").Where("id = ?", id).First(&user)
	return user.ID > 0
}

func ExistUserByTelephone(telephone string) bool {
	var user User
	db.Select("id").Where("telephone = ?", telephone).First(&user)
	return user.ID > 0
}

func AddUser(username, password, telephone, des string, avatar int) bool {
	db.Create(&User{
		Username: username,
		Password: password,
		Telephone: telephone,
		Avatar: avatar,
		Des: des,
	})
	return true
}

func DeleteUser(id int) bool {
	db.Where("id = ?", id).Delete(&User{})
	return true
}

func UpdateUser(id int, data interface{}) bool {
	db.Model(&User{}).Where("id = ?", id).Updates(data)
	return true
}
