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

func GetBCUInfos(c *gin.Context) {
	data := make(map[string]interface{})
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data["bcus"] = models.GetBCUInfos(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetBCURank(c *gin.Context) {
	var data map[string]interface{}
	stationId := c.Query("stationId")
	valid1 := validation.Validation{}
	valid1.Required(stationId, "stationId").Message("stationId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpStationId, _ := strconv.Atoi(stationId)
		if models.ExistStationByID(tmpStationId) {
			code = e.SUCCESS
			data = models.GetBCURank(tmpStationId)
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

func GetBCUDataStat(c *gin.Context) {
	data := make(map[string]interface{})
	bcuId := c.Query("bcuId")
	valid1 := validation.Validation{}
	valid1.Required(bcuId, "bcuId").Message("bcuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBCUId, _ := strconv.Atoi(bcuId)
		if models.ExistBCUById(tmpBCUId) {
			code = e.SUCCESS
			data = models.GetBCUDataStat(tmpBCUId)
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

func GetBCUSystemStat(c *gin.Context) {
	var data models.BCUSystem
	bcuId := c.Query("bcuId")
	valid1 := validation.Validation{}
	valid1.Required(bcuId, "bcuId").Message("bcuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBCUId, _ := strconv.Atoi(bcuId)
		if models.ExistBCUById(tmpBCUId) {
			code = e.SUCCESS
			data = models.GetBCUSystemStat(tmpBCUId)
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

func GetBCUWarningStat(c *gin.Context) {
	var data [3]map[string]interface{}
	bcuId := c.Query("bcuId")
	valid1 := validation.Validation{}
	valid1.Required(bcuId, "bcuId").Message("bcuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBCUId, _ := strconv.Atoi(bcuId)
		if models.ExistBCUById(tmpBCUId) {
			code = e.SUCCESS
			data = models.GetBCUWarningStat(tmpBCUId)
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

func GetBCUSingleStat(c *gin.Context) {
	data := make(map[string]interface{})
	bcuId := c.Query("bcuId")
	valid1 := validation.Validation{}
	valid1.Required(bcuId, "bcuId").Message("bcuId不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		tmpBCUId, _ := strconv.Atoi(bcuId)
		if models.ExistBCUById(tmpBCUId) {
			code = e.SUCCESS
			data = models.GetBCUSingleStat(tmpBCUId)
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

func AddBCU(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	id := json["id"]
	stationId := json["stationId"]
	name := json["name"]
	valid1 := validation.Validation{}
	valid1.Required(stationId, "stationId").Message("储能站ID不能为空")
	valid1.Required(id, "id").Message("BCU的ID不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NO_AUTHORITY
		tmpBCUId, _ := strconv.Atoi(id)
		tmpStationId, _ := strconv.Atoi(stationId)
		if models.CheckStationAuthority(userId, tmpStationId) {
			code = e.ERROR_NOT_EXIST
			if models.ExistBCUById(tmpBCUId) {
				code = e.ERROR_ALREADY_EXIST
			} else {
				code = e.SUCCESS
				models.AddBCU(tmpBCUId, tmpStationId, name)
			}
			models.AddBCUSystem(tmpBCUId, 1, 0, 0, 0, 1, 1, 0, 0)
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

func UpdateBCU(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS

	if models.ExistBCUById(id) {
		json := make(map[string]interface{})
		_ = c.BindJSON(&json)
		stationId := json["stationId"]
		if stationId != nil {
			tmp,_ := strconv.Atoi(stationId.(string))
			if !models.ExistStationByID(tmp){
				code = e.ERROR_NOT_EXIST
			} else if !models.CheckStationAuthority(userId, tmp) {
				code = e.ERROR_NO_AUTHORITY
			} else {
				json["stationId"] = tmp
			}
		}
		models.UpdateBCU(id, json)
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func DeleteBCU(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS
	if models.ExistBCUById(id) {
		if models.CheckDeleteBCU(id) {
			code = e.SUCCESS
			models.DeleteBCU(id)
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
