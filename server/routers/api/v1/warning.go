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

func GetWarningStationStat(c *gin.Context) {
	var data []models.WarningStationStat
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetWarningStationStat(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetWarningTimeStat(c *gin.Context) {
	data := make(map[string]interface{})
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetWarningTimeStat(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetWarnings(c *gin.Context) {
	var data []models.Warning
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	device := c.Query("device")
	deviceId := c.Query("deviceId")
	valueType:= c.Query("valueType")
	startTime := c.Query("startTime")
	endTime := c.Query("endTime")
	valid := validation.Validation{}
	valid.Required(device, "device").Message("device不能为空")
	valid.Required(deviceId, "deviceId").Message("deviceId不能为空")
	valid.Required(valueType, "valueType").Message("valueType不能为空")
	valid.Required(startTime, "startTime").Message("startTime不能为空")
	valid.Required(endTime, "endTime").Message("endTime不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		intStartTime, _ := strconv.ParseInt(startTime, 10, 64)
		intEndTime, _ := strconv.ParseInt(endTime, 10, 64)
		tmpDeviceId, _ := strconv.Atoi(deviceId)
		if tmpDeviceId == 0 {
			code = e.SUCCESS
			data = models.GetUserWarnings(userId, valueType, intStartTime, intEndTime)
		} else if device == "Station" {
			if !models.ExistStationByID(tmpDeviceId) {
				code = e.ERROR_NOT_EXIST
			} else {
				if models.CheckStationAuthority(userId, tmpDeviceId) {
					code = e.SUCCESS
					data = models.GetStationWarnings(tmpDeviceId, valueType, intStartTime, intEndTime)
				} else {
					code = e.ERROR_NO_AUTHORITY
				}
			}
		} else if device == "BCU" {
			if !models.ExistBCUById(tmpDeviceId) {
				code = e.ERROR_NOT_EXIST
			} else {
				if models.CheckBCUAuthority(userId, tmpDeviceId) {
					code = e.SUCCESS
					data = models.GetBCUWarnings(tmpDeviceId, valueType, intStartTime, intEndTime)
				} else {
					code = e.ERROR_NO_AUTHORITY
				}
			}
		} else if device == "BMU" {
			if !models.ExistBMUById(tmpDeviceId) {
				code = e.ERROR_NOT_EXIST
			} else {
				if models.CheckBMUAuthority(userId, tmpDeviceId) {
					code = e.SUCCESS
					data = models.GetBMUWarnings(tmpDeviceId, valueType, intStartTime, intEndTime)
				} else {
					code = e.ERROR_NO_AUTHORITY
				}
			}
		}
	} else {
		for _, err := range valid.Errors {
			log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
		}
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func AddWarningInBMU(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	pointId := json["pointId"]
	bmuId := json["bmuId"]
	value := json["value"]
	valueType := json["valueType"]
	valid := validation.Validation{}
	valid.Required(pointId, "pointId").Message("pointId不能为空")
	valid.Required(bmuId, "bmuId").Message("bmuId不能为空")
	valid.Required(valueType, "valueType").Message("valueType不能为空")
	valid.Required(value, "value").Message("value不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpPointId, _ := strconv.Atoi(pointId)
		tmpBMUId, _ := strconv.Atoi(bmuId)
		tmpValueType, _ := strconv.Atoi(valueType)
		if models.ExistBMUById(tmpBMUId){
			code = e.SUCCESS
			models.AddWarningInBMU(tmpPointId, tmpBMUId, tmpValueType, value)
		}
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

func AddWarningInBCU(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	pointId := json["pointId"]
	bmuId := json["bmuId"]
	bcuId := json["bcuId"]
	value := json["value"]
	valueType := json["valueType"]
	valid := validation.Validation{}
	valid.Required(pointId, "pointId").Message("pointId不能为空")
	valid.Required(bmuId, "bmuId").Message("bmuId不能为空")
	valid.Required(bcuId, "bcuId").Message("bcuId不能为空")
	valid.Required(valueType, "valueType").Message("valueType不能为空")
	valid.Required(value, "value").Message("value不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpPointId, _ := strconv.Atoi(pointId)
		tmpBMUId, _ := strconv.Atoi(bmuId)
		tmpBCUId, _ := strconv.Atoi(bcuId)
		tmpValueType, _ := strconv.Atoi(valueType)
		if models.ExistBMUById(tmpBMUId) && models.ExistBCUById(tmpBCUId){
			code = e.SUCCESS
			models.AddWarningInBCU(tmpPointId, tmpBMUId, tmpBCUId, tmpValueType, value)
		}
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

func AddWarningInBSU(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	pointId := json["pointId"]
	bmuId := json["bmuId"]
	bcuId := json["bcuId"]
	bsuId := json["bsuId"]
	value := json["value"]
	valueType := json["valueType"]
	valid := validation.Validation{}
	valid.Required(pointId, "pointId").Message("pointId不能为空")
	valid.Required(bmuId, "bmuId").Message("bmuId不能为空")
	valid.Required(bcuId, "bcuId").Message("bcuId不能为空")
	valid.Required(bcuId, "bsuId").Message("bsuId不能为空")
	valid.Required(valueType, "valueType").Message("valueType不能为空")
	valid.Required(value, "value").Message("value不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpPointId, _ := strconv.Atoi(pointId)
		tmpBMUId, _ := strconv.Atoi(bmuId)
		tmpBCUId, _ := strconv.Atoi(bcuId)
		tmpBSUId, _ := strconv.Atoi(bsuId)
		tmpValueType, _ := strconv.Atoi(valueType)
		if models.ExistBMUById(tmpBMUId) && models.ExistBCUById(tmpBCUId) && models.ExistBSUById(tmpBSUId) {
			code = e.SUCCESS
			models.AddWarningInBSU(tmpPointId, tmpBMUId, tmpBCUId, tmpBSUId, tmpValueType, value)
		}
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

func UpdateWarning(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS

	if models.ExistWarningById(id) {
		code = e.SUCCESS
		json := make(map[string]interface{})
		_ = c.BindJSON(&json)
		status := json["status"]
		operator := json["operator"]
		operation := json["operation"]
		if status != nil {
			tmpStatus, _ := strconv.Atoi(status.(string))
			json["status"] = tmpStatus
		}
		if operator != nil {
			tmpOperator, _ := strconv.Atoi(operator.(string))
			json["operator"] = tmpOperator
		}
		if operation != nil {
			tmpOperation, _ := strconv.Atoi(operation.(string))
			json["operation"] = tmpOperation
		}
		models.UpdateWarning(id, json)
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}