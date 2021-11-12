package routers

import (
	"gin-example/middleware"
	v1 "gin-example/routers/api/v1"
	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
	"net/http"
	"time"

	"gin-example/pkg/setting"
)

func InitRouter() *gin.Engine {
	r := gin.New()

	r.Use(cors.New(cors.Config{
		AllowOrigins:     []string{"*"},
		AllowMethods:     []string{"PUT", "PATCH", "GET", "POST", "HEAD", "OPTIONS", "DELETE"},
		AllowHeaders:     []string{"Origin", "Authorization", "Content-Type", "x-token"},
		ExposeHeaders:    []string{"Content-Length"},
		AllowCredentials: true,
		AllowOriginFunc: func(origin string) bool {
			return true
		},
		MaxAge: 12 * time.Hour,
	}))

	r.Use(gin.Logger())

	r.Use(gin.Recovery())

	r.StaticFS("/static", http.Dir("./static"))

	gin.SetMode(setting.RunMode)

	r.GET("/sendVerifyCode", v1.SendVerifyCode)
	r.POST("/loginByPW", v1.LoginByPW)
	r.POST("/loginByVC", v1.LoginByVC)
	r.POST("/register", v1.Register)
	r.POST("/addCollect", v1.AddCollect)
	r.POST("/addOrder", v1.AddOrder)
	r.PUT("/updateOrder/:id", v1.UpdateOrder)

	apiv1 := r.Group("/api/v1")
	apiv1.Use(middleware.JWT())
	{
		apiv1.GET("/getUserInfo", v1.GetUserInfo)
		apiv1.POST("/updateUser", v1.UpdateUser)
		apiv1.DELETE("/deleteUser/:id", v1.DeleteUser)

		apiv1.POST("/addImage", v1.AddImage)

		apiv1.GET("/getEmployments", v1.GetEmployments)

		apiv1.GET("/getDashboardOrder", v1.GetDashboardOrder)
		apiv1.GET("/getDashboardStatus", v1.GetDashboardStatus)

		apiv1.GET("/getStationInfos", v1.GetStationInfos)
		apiv1.GET("/getStationDischargeScatter", v1.GetStationDischargeScatter)
		apiv1.GET("/getStationRank", v1.GetStationRank)
		apiv1.POST("/addStation", v1.AddStation)
		apiv1.PUT("/updateStation/:id", v1.UpdateStation)
		apiv1.DELETE("/deleteStation/:id", v1.DeleteStation)

		apiv1.GET("/getBSUsForUser", v1.GetBSUsForUser)
		apiv1.GET("/getBSUsByStationId", v1.GetBSUsByStationId)
		apiv1.GET("/getBSUSlaves", v1.GetBSUSlaves)
		apiv1.POST("/addBSU", v1.AddBSU)
		apiv1.PUT("/updateBSU/:id", v1.UpdateBSU)
		apiv1.DELETE("/deleteBSU/:id", v1.DeleteBSU)

		apiv1.GET("/getBCUInfos", v1.GetBCUInfos)
		apiv1.GET("/getBCURank", v1.GetBCURank)
		apiv1.GET("/getBCUDataStat", v1.GetBCUDataStat)
		apiv1.GET("/getBCUSystemStat", v1.GetBCUSystemStat)
		apiv1.GET("/getBCUWarningStat", v1.GetBCUWarningStat)
		apiv1.GET("/getBCUSingleStat", v1.GetBCUSingleStat)
		apiv1.POST("/addBCU", v1.AddBCU)
		apiv1.PUT("/updateBCU/:id", v1.UpdateBCU)
		apiv1.DELETE("/deleteBCU/:id", v1.DeleteBCU)

		apiv1.GET("/getBMUInfos", v1.GetBMUInfos)
		apiv1.GET("/getBMUDataStat", v1.GetBMUDataStat)
		apiv1.GET("/getBMUWarningStat", v1.GetBMUWarningStat)
		apiv1.GET("/getBMUWarningCalendar", v1.GetBMUWarningCalendar)
		apiv1.GET("/getBMUSOCStat", v1.GetBMUSOCStat)
		apiv1.GET("/getBMUOutlineStat", v1.GetBMUOutlineStat)
		apiv1.GET("/getBMUSingles", v1.GetBMUSingles)
		apiv1.POST("/addBMU", v1.AddBMU)
		apiv1.PUT("/updateBMU/:id", v1.UpdateBMU)
		apiv1.DELETE("/deleteBMU/:id", v1.DeleteBMU)

		apiv1.GET("/getCollects", v1.GetCollects)

		apiv1.GET("/getWarningStationStat", v1.GetWarningStationStat)
		apiv1.GET("/getWarningTimeStat", v1.GetWarningTimeStat)
		apiv1.GET("/getWarnings", v1.GetWarnings)
		apiv1.POST("/addWarningInBMU", v1.AddWarningInBMU)
		apiv1.POST("/addWarningInBCU", v1.AddWarningInBCU)
		apiv1.POST("/addWarningInBSU", v1.AddWarningInBSU)
		apiv1.PUT("/updateWarning/:id", v1.UpdateWarning)

		apiv1.GET("/getOrderStationStat", v1.GetOrderStationStat)
		apiv1.GET("/getOrderTimeStat", v1.GetOrderTimeStat)
		apiv1.GET("/getOrders", v1.GetOrders)
		apiv1.DELETE("/deleteOrder/:id", v1.DeleteOrder)

		apiv1.GET("/getScreenOrderStat", v1.GetScreenOrderStat)
		apiv1.GET("/getScreenWarningStat", v1.GetScreenWarningStat)
		apiv1.GET("/getScreenOverallStat", v1.GetScreenOverallStat)
		apiv1.GET("/getScreenStationStat", v1.GetScreenStationStat)
		apiv1.GET("/getScreenBatteryStat", v1.GetScreenBatteryStat)
	}
	return r
}