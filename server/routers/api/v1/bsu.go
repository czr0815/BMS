package v1

import (
	"gin-example/models"
	"gin-example/pkg/e"
	"github.com/astaxie/beego/validation"
	"github.com/gin-gonic/gin"
	"github.com/unknwon/com"
	"log"
	"net/http"
	"strconv"
)

func GetBSUsForUser(c *gin.Context) {
	data := make(map[string]interface{})
	userId := c.Query("userId")
	tmp, _ := strconv.Atoi(userId)
	code := e.ERROR_NOT_EXIST
	if models.ExistUserByID(tmp) {
		code = e.SUCCESS
		data["bsus"] = models.GetBSUsForUser(tmp)
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetBSUsByStationId(c *gin.Context) {
	data := make(map[string]interface{})
	stationId := c.Query("stationId")
	valid1 := validation.Validation{}
	valid1.Required(stationId, "stationId").Message("储能站ID不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.SUCCESS
		tmp1, _ := strconv.Atoi(stationId)
		data["bsus"] = models.GetBSUsByStationId(tmp1)
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

func GetBSUSlaves(c *gin.Context) {
	data := make(map[string]interface{})
	id := c.Query("id")
	valid := validation.Validation{}
	valid.Required(id, "id").Message("BSU的ID不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		code = e.SUCCESS
		tmp1, _ := strconv.Atoi(id)
		data["slaves"], data["type"] = models.GetBSUSlave(tmp1)
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

func AddBSU(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	userId := json["userId"]
	id := json["id"]
	stationId := json["stationId"]
	valid := validation.Validation{}
	valid.Required(stationId, "name").Message("储能站ID不能为空")
	valid.Required(id, "id").Message("BSU的ID不能为空")
	valid.Required(userId, "userId").Message("操作人员的ID不能为空")
	code := e.INVALID_PARAMS
	if !valid.HasErrors() {
		code = e.ERROR_NO_AUTHORITY
		tmpUserId, _ := strconv.Atoi(userId)
		tmpBSUId, _ := strconv.Atoi(id)
		tmpStationId, _ := strconv.Atoi(stationId)
		if models.CheckStationAuthority(tmpUserId, tmpStationId) {
			code = e.SUCCESS
			models.AddBSU(tmpBSUId, tmpStationId)
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

func UpdateBSU(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS

	if models.ExistBSUById(id) {
		code = e.SUCCESS
		json := make(map[string]interface{})
		_ = c.BindJSON(&json)
		stationId := json["stationId"]
		status := json["status"]
		if stationId != nil {
			tmp, _ := strconv.Atoi(stationId.(string))
			json["stationId"] = tmp
		}
		if status != nil {
			tmp, _ := strconv.Atoi(status.(string))
			json["status"] = tmp
		}
		models.UpdateBSU(id, json)
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func DeleteBSU(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS
	if models.ExistBSUById(id) {
		if models.CheckDeleteBSU(id) {
			code = e.SUCCESS
			models.DeleteBSU(id)
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
