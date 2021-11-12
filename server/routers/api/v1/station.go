package v1

import (
	"gin-example/models"
	"gin-example/pkg/e"
	"gin-example/pkg/util"
	"github.com/astaxie/beego/validation"
	"github.com/gin-gonic/gin"
	"github.com/unknwon/com"
	"log"
	"net/http"
	"strconv"
)

func GetStationInfos(c *gin.Context) {
	data := make(map[string]interface{})
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data["stations"] = models.GetStationInfos(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetStationDischargeScatter(c *gin.Context) {
	var data interface{}
	stationId := c.Query("stationId")
	valid1 := validation.Validation{}
	valid1.Required(stationId, "stationId").Message("stationId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpStationId, _ := strconv.Atoi(stationId)
		if models.ExistStationByID(tmpStationId) {
			code = e.SUCCESS
			data = models.GetStationDischargeScatter(tmpStationId)
		}
	} else {
		for _, err := range valid1.Errors {
			log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
		}
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetStationRank(c *gin.Context) {
	data := make(map[string]interface{})
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetStationRank(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func AddStation(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	name := json["name"]
	lng := json["lng"]
	lat := json["lat"]
	position := json["position"]
	employeeId := json["employeeId"]
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	ownerId := claims.UserId

	valid := validation.Validation{}
	valid.Required(name, "name").Message("储能站名称不能为空")
	valid.Required(lng, "lng").Message("地理经度不能为空")
	valid.Required(lat, "lat").Message("地理纬度不能为空")
	valid.Required(position, "position").Message("位置不能为空")
	valid.Required(employeeId, "employeeId").Message("管理员的ID不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		code = e.SUCCESS
		tmpLng, _ := strconv.ParseFloat(lng, 64)
		tmpLat, _ := strconv.ParseFloat(lat, 64)
		stationId := models.AddStation(name, tmpLng, tmpLat, position, ownerId, 0)
		tmpEmployeeId, _ := strconv.Atoi(employeeId)
		models.AddEmployment(ownerId, tmpEmployeeId, stationId)
	} else {
		for _, err := range valid.Errors {
			log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
		}
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func UpdateStation(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS

	if models.ExistStationByID(id) {
		json := make(map[string]interface{})
		_ = c.BindJSON(&json)
		name := json["name"]
		position := json["position"]
		lng := json["lng"]
		lat := json["lat"]
		employeeId := json["employeeId"]
		jsonStation := make(map[string]interface{})
		jsonEmployment := make(map[string]interface{})
		if lng != nil {
			tmp, _ := strconv.ParseFloat(lng.(string), 64)
			jsonStation["lng"] = tmp
		}
		if lat != nil {
			tmp, _ := strconv.ParseFloat(lat.(string), 64)
			jsonStation["lat"] = tmp
		}
		if name != nil {
			jsonStation["name"] = name
		}
		if position != nil {
			jsonStation["position"] = position
		}
		models.UpdateStation(id, jsonStation)
		if employeeId != nil {
			tmp, _ := strconv.Atoi(employeeId.(string))
			if !models.CheckStationOwnerAuthority(userId, id) {
				code = e.ERROR_NO_AUTHORITY
			} else if !models.ExistUserByID(tmp) {
				code = e.ERROR_NOT_EXIST
			} else {
				if models.ExistEmploymentByInfo(id) {
					jsonEmployment["employeeId"] = tmp
					models.UpdateEmployment(userId, id, json)
				} else {
					models.AddEmployment(userId, tmp, id)
				}
			}
		}
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func DeleteStation(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS

	if models.ExistStationByID(id) {
		if models.CheckDeleteStation(id) {
			models.DeleteStation(id)
			code = e.SUCCESS
		} else {
			code = e.ERROR_CANNOT_DELETE
		}
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}