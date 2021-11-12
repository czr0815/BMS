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

func GetBMUInfos(c *gin.Context) {
	data := make(map[string]interface{})
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data["bmus"] = models.GetBMUInfos(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetBMUDataStat(c *gin.Context) {
	data := make(map[string]interface{})
	bmuId := c.Query("bmuId")
	valid1 := validation.Validation{}
	valid1.Required(bmuId, "bmuId").Message("bmuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBMUId, _ := strconv.Atoi(bmuId)
		if models.ExistBMUById(tmpBMUId) {
			code = e.SUCCESS
			data = models.GetBMUDataStat(tmpBMUId)
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

func GetBMUWarningStat(c *gin.Context) {
	var data [3]map[string]interface{}
	bmuId := c.Query("bmuId")
	valid1 := validation.Validation{}
	valid1.Required(bmuId, "bmuId").Message("bmuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBMUId, _ := strconv.Atoi(bmuId)
		if models.ExistBMUById(tmpBMUId) {
			code = e.SUCCESS
			data = models.GetBMUWarningStat(tmpBMUId)
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

func GetBMUWarningCalendar(c *gin.Context) {
	var data []models.BMUWarningCalendar
	bmuId := c.Query("bmuId")
	valid1 := validation.Validation{}
	valid1.Required(bmuId, "bmuId").Message("bmuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBMUId, _ := strconv.Atoi(bmuId)
		if models.ExistBMUById(tmpBMUId) {
			code = e.SUCCESS
			data = models.GetBMUWarningCalendar(tmpBMUId)
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

func GetBMUSOCStat(c *gin.Context) {
	var data []models.BMUSOCStat
	bmuId := c.Query("bmuId")
	valid1 := validation.Validation{}
	valid1.Required(bmuId, "bmuId").Message("bmuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBMUId, _ := strconv.Atoi(bmuId)
		if models.ExistBMUById(tmpBMUId) {
			code = e.SUCCESS
			data = models.GetBMUSOCStat(tmpBMUId)
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

func GetBMUOutlineStat(c *gin.Context) {
	var data map[string]interface{}
	bmuId := c.Query("bmuId")
	valid1 := validation.Validation{}
	valid1.Required(bmuId, "bmuId").Message("bmuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBMUId, _ := strconv.Atoi(bmuId)
		if models.ExistBMUById(tmpBMUId) {
			code = e.SUCCESS
			data = models.GetBMUOutlineStat(tmpBMUId)
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

func GetBMUSingles(c *gin.Context) {
	var data []models.BMUSingle
	bmuId := c.Query("bmuId")
	valid1 := validation.Validation{}
	valid1.Required(bmuId, "bmuId").Message("bmuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBMUId, _ := strconv.Atoi(bmuId)
		if models.ExistBMUById(tmpBMUId) {
			code = e.SUCCESS
			data = models.GetBMUSingles(tmpBMUId)
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

func AddBMU(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	id := json["id"]
	bcuId := json["bcuId"]
	batteryNum := json["batteryNum"]
	batteryType := json["batteryType"]
	valid1 := validation.Validation{}
	valid1.Required(id, "id").Message("BMU的ID不能为空")
	valid1.Required(batteryNum, "batteryNum").Message("电池数量不能为空")
	valid1.Required(batteryType, "batteryType").Message("电池种类不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NO_AUTHORITY
		tmpBMUId, _ := strconv.Atoi(id)
		tmpBCUId, _ := strconv.Atoi(bcuId)
		tmpBatteryNum, _ := strconv.Atoi(batteryNum)
		tmpBatteryType, _ := strconv.Atoi(batteryType)
		if !models.ExistBCUById(tmpBCUId) {
			code = e.ERROR_NOT_EXIST
		} else if !models.CheckBCUAuthority(userId, tmpBCUId) {
			code = e.ERROR_NO_AUTHORITY
		} else if models.ExistBMUById(tmpBMUId) {
			code = e.ERROR_ALREADY_EXIST
		} else {
			code = e.SUCCESS
			var stationId = models.GetStationForBCU(tmpBCUId)
			models.AddBMU(tmpBMUId, tmpBCUId, stationId.ID, tmpBatteryNum, tmpBatteryType)
		}
	} else {
		for _, err := range valid1.Errors {
			log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
		}
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func UpdateBMU(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS

	if models.ExistBMUById(id) {
		json := make(map[string]interface{})
		_ = c.BindJSON(&json)
		bcuId := json["bcuId"]
		batteryNum := json["batteryNum"]
		batteryType := json["batteryType"]
		if bcuId != nil {
			tmp, _ := strconv.Atoi(bcuId.(string))
			if !models.ExistBCUById(tmp) {
				code = e.ERROR_NOT_EXIST
			} else if !models.CheckBCUAuthority(userId, tmp) {
				code = e.ERROR_NO_AUTHORITY
			} else {
				json["bcuId"] = tmp
			}
		}
		if batteryNum != nil {
			tmp, _ := strconv.Atoi(batteryNum.(string))
			json["batteryNum"] = tmp
		}
		if batteryType != nil {
			tmp, _ := strconv.Atoi(batteryType.(string))
			json["batteryType"] = tmp
		}
		models.UpdateBMU(id, json)
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func DeleteBMU(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS
	if models.ExistBMUById(id) {
		models.DeleteBMU(id)
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
