package v1

import (
	"gin-example/models"
	"gin-example/pkg/e"
	"gin-example/pkg/util"
	"github.com/astaxie/beego/validation"
	"github.com/gin-gonic/gin"
	"github.com/thedevsaddam/gojsonq/v2"
	"io/ioutil"
	"log"
	"net/http"
	"strconv"
	"strings"
	"time"
)

func AddCollect(c *gin.Context) {
	body, _ := ioutil.ReadAll(c.Request.Body)
	/*
	fmt.Println("---header/--- \r\n")
	for k,v :=range c.Request.Header {
		fmt.Println(k,v)
	}
	fmt.Println("---body/--- \r\n")
	fmt.Println(string(body))
	*/
	bodys := strings.Split(string(body), "smartiot")
	//fmt.Println(bodys)
	code := e.INVALID_PARAMS
	for _, str := range bodys {
		device := gojsonq.New().FromString(str).Find("device")
		if device == nil {
			break
		}
		valid := validation.Validation{}
		valid.Required(device, "device").Message("device不能为空")
		if !valid.HasErrors() {
			if device == "bmu" {
				bmuId := gojsonq.New().FromString(str).Find("bmuId")
				cells := gojsonq.New().FromString(str).Find("cell")
				temperatures := gojsonq.New().FromString(str).Find("temp")
				smokes := gojsonq.New().FromString(str).Find("smoke")
				current := gojsonq.New().FromString(str).Find("curr")
				time := time.Now()
				valid1 := validation.Validation{}
				valid1.Required(bmuId, "bmuId").Message("bmuId不能为空")
				if !valid1.HasErrors() {
					code = e.ERROR_NOT_EXIST
					tmpBMUId, _ := strconv.Atoi(bmuId.(string))
					if models.ExistBMUById(tmpBMUId) {
						code = e.SUCCESS
						if cells != nil {
							for _, cell := range cells.([]interface{}) {
								tmpPointId, _ := strconv.Atoi(cell.(map[string]interface{})["id"].(string))
								tmpVoltage := cell.(map[string]interface{})["vol"].(float64)
								tmpSoc := cell.(map[string]interface{})["soc"].(float64)
								models.AddBMUBattery(tmpPointId, tmpBMUId, tmpVoltage, tmpSoc, time)
								models.AddBMUBatteryRecent(tmpPointId, tmpBMUId, tmpVoltage, tmpSoc, time)
								if models.ExistBMUBatteryRecent(tmpBMUId, tmpPointId) {
									models.UpdateBMUBatteryRecent(tmpPointId, tmpBMUId, tmpVoltage, tmpSoc, time)
								} else {
									models.AddBMUBatteryRecent(tmpPointId, tmpBMUId, tmpVoltage, tmpSoc, time)
								}
							}
						}

						if temperatures != nil {
							for _, temp := range temperatures.([]interface{}) {
								tmpPointId, _ := strconv.Atoi(temp.(map[string]interface{})["id"].(string))
								tmpTemperature := temp.(map[string]interface{})["val"].(float64)
								models.AddBMUTemperature(tmpPointId, tmpBMUId, tmpTemperature, time)
								if models.ExistBMUTemperatureRecent(tmpBMUId, tmpPointId) {
									models.UpdateBMUTemperatureRecent(tmpPointId, tmpBMUId, tmpTemperature, time)
								} else {
									models.AddBMUTemperatureRecent(tmpPointId, tmpBMUId, tmpTemperature, time)
								}
							}
						}

						if smokes != nil {
							for _, smoke := range smokes.([]interface{}) {
								tmpPointId, _ := strconv.Atoi(smoke.(map[string]interface{})["id"].(string))
								tmpSmoke := smoke.(map[string]interface{})["val"].(float64)
								models.AddBMUSmoke(tmpPointId, tmpBMUId, tmpSmoke, time)
							}
						}

						if current != nil {
							models.AddBMUCurrent(tmpBMUId, current.(float64), time)
						}
					}
				} else {
					for _, err := range valid1.Errors {
						log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
					}
				}
			} else if device == "bcu" {
				bcuId := gojsonq.New().FromString(str).Find("bcuId")
				voltages := gojsonq.New().FromString(str).Find("volt")
				soc := gojsonq.New().FromString(str).Find("soc")
				current := gojsonq.New().FromString(str).Find("curr")
				insulations := gojsonq.New().FromString(str).Find("insu")
				system := gojsonq.New().FromString(str).Find("system")
				time := time.Now()
				valid2 := validation.Validation{}
				valid2.Required(bcuId, "bcuId").Message("bcuId不能为空")
				if !valid2.HasErrors() {
					code = e.ERROR_NOT_EXIST
					tmpBCUId, _ := strconv.Atoi(bcuId.(string))
					if models.ExistBCUById(tmpBCUId) {
						code = e.SUCCESS
						if voltages != nil {
							for _, voltage := range voltages.([]interface{}) {
								tmpPointId, _ := strconv.Atoi(voltage.(map[string]interface{})["id"].(string))
								tmpVoltage := voltage.(map[string]interface{})["val"].(float64)
								models.AddBCUVoltage(tmpPointId, tmpBCUId, tmpVoltage, time)
							}
						}

						if soc != nil {
							models.AddBCUSoc(tmpBCUId, soc.(float64), time)
						}

						if current != nil {
							models.AddBCUCurrent(tmpBCUId, current.(float64), time)
						}

						if insulations != nil {
							for _, insulation := range insulations.([]interface{}) {
								tmpPointId, _ := strconv.Atoi(insulation.(map[string]interface{})["id"].(string))
								tmpInsulation := insulation.(map[string]interface{})["val"].(float64)
								models.AddBCUInsulation(tmpBCUId, tmpPointId, tmpInsulation, time)
							}
						}
						if system != nil {
							models.UpdateBCUSystem(tmpBCUId, system)
						}
					}
				} else {
					for _, err := range valid.Errors {
						log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
					}
				}
			}
		} else {
			for _, err := range valid.Errors {
				log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
			}
		}
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func GetCollects(c *gin.Context) {
	var data []models.Collect
	device := c.Query("device")
	deviceId := c.Query("deviceId")
	valueType := c.Query("valueType")
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
		code = e.ERROR_NOT_EXIST
		intStartTime, _ := strconv.ParseInt(startTime, 10, 64)
		intEndTime, _ := strconv.ParseInt(endTime, 10, 64)
		if device == "Station" {
			tmpStationId, _ := strconv.Atoi(deviceId)
			if tmpStationId == 0 {
				//.....查找该用户所有stationId
				code = e.SUCCESS
				data = models.GetStationCollects(tmpStationId, valueType, intStartTime, intEndTime)
			}
		} else if device == "BCU" {
			tmpBCUId, _ := strconv.Atoi(deviceId)
			if tmpBCUId == 0 {
				//..........查找该用户相关的所有bmuId，遍历查询
			} else if models.ExistBCUById(tmpBCUId) {
				code = e.SUCCESS
				data = models.GetBCUCollects(tmpBCUId, valueType, intStartTime, intEndTime)
			}
		} else if device == "BMU" {
			tmpBMUId, _ := strconv.Atoi(deviceId)
			if tmpBMUId == 0 {
				//........查找该用户相关的所有bcuId
			} else if models.ExistBMUById(tmpBMUId) {
				code = e.SUCCESS
				data = models.GetBMUCollects(tmpBMUId, valueType, intStartTime, intEndTime)
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

func GetDashboardOrder(c *gin.Context) {
	data := make(map[string]interface{})
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetDashboardOrder(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func GetDashboardStatus(c *gin.Context) {
	var data map[string]interface{}
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetDashboardStatus(userId)

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}