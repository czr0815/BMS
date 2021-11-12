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

func GetOrderStationStat(c *gin.Context) {
	var data []models.OrderStationStat
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetOrderStationStat(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetOrderTimeStat(c *gin.Context) {
	data := make(map[string]interface{})
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetOrderTimeStat(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetOrders(c *gin.Context) {
	var data []models.Order
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	device := c.Query("device")
	deviceId := c.Query("deviceId")
	orderType:= c.Query("orderType")
	startTime := c.Query("startTime")
	endTime := c.Query("endTime")
	valid := validation.Validation{}
	valid.Required(device, "device").Message("device不能为空")
	valid.Required(deviceId, "deviceId").Message("deviceId不能为空")
	valid.Required(orderType, "orderType").Message("orderType不能为空")
	valid.Required(startTime, "startTime").Message("startTime不能为空")
	valid.Required(endTime, "endTime").Message("endTime不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		intStartTime, _ := strconv.ParseInt(startTime, 10, 64)
		intEndTime, _ := strconv.ParseInt(endTime, 10, 64)
		tmpDeviceId, _ := strconv.Atoi(deviceId)
		if tmpDeviceId == 0 {
			code = e.SUCCESS
			data = models.GetUserOrders(userId, orderType, intStartTime, intEndTime)
		} else if device == "Station" {
			if !models.ExistStationByID(tmpDeviceId) {
				code = e.ERROR_NOT_EXIST
			} else {
				if models.CheckStationAuthority(userId, tmpDeviceId) {
					code = e.SUCCESS
					data = models.GetStationOrders(tmpDeviceId, orderType, intStartTime, intEndTime)
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
					data = models.GetBCUOrders(tmpDeviceId, orderType, intStartTime, intEndTime)
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

func AddOrder(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	bcuId := json["bcuId"]
	stationId := json["stationId"]
	orderType := json["orderType"]
	valid := validation.Validation{}
	valid.Required(bcuId, "bcuId").Message("bcuId不能为空")
	valid.Required(bcuId, "stationId").Message("stationId不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		code = e.SUCCESS
		tmpBCUId, _ := strconv.Atoi(bcuId)
		tmpStationId, _ := strconv.Atoi(stationId)
		tmpOrderType, _ := strconv.Atoi(orderType)
		models.AddOrder(tmpBCUId, tmpStationId, tmpOrderType)
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

func UpdateOrder(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS

	if models.ExistOrderById(id) {
		code = e.SUCCESS
		json := make(map[string]interface{})
		_ = c.BindJSON(&json)
		status := json["status"]
		electricity := json["electricity"]
		if status != nil {
			tmp, _ := strconv.Atoi(status.(string))
			json["status"] = tmp
		}
		if electricity != nil {
			tmp, _ := strconv.ParseFloat(electricity.(string), 64)
			json["electricity"] = tmp
		}
		models.UpdateOrder(id, json)
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func DeleteOrder(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS
	if models.ExistOrderById(id) {
		models.DeleteOrder(id)
		code = e.SUCCESS
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}
