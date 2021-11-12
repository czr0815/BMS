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
)

func SendVerifyCode(c *gin.Context) {
	telephone := c.Query("telephone")
	valid1 := validation.Validation{}
	valid1.Required(telephone, "telephone").Message("telephone不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.SUCCESS
		util.SendSMS(telephone)
	} else {
		for _, err := range valid1.Errors {
			log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
		}
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]interface{}),
	})
}

func LoginByVC(c *gin.Context) {
	json := make(map[string]string)
	data := make(map[string]interface{})
	_ = c.BindJSON(&json)

	telephone := json["telephone"]
	verifyCode := json["verifyCode"]
	valid1 := validation.Validation{}
	valid1.Required(telephone, "telephone").Message("telephone不能为空")
	valid1.Required(verifyCode, "verifyCode").Message("verifyCode不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_VERIFYCODE_NOT_MATCH
		if util.CheckVerifyCode(telephone, verifyCode) {
			code = e.ERROR_NOT_EXIST
			if models.ExistUserByTelephone(telephone) {
				user := models.GetUserByTelephone(telephone)
				code = e.ERROR_CREATE_TOKEN
				token, err := util.GenerateToken(user.ID)
				if err == nil {
					code = e.SUCCESS
					data["token"] = token
					data["user"] = user
				}
			}
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

func LoginByPW(c *gin.Context) {
	json := make(map[string]string)
	data := make(map[string]interface{})
	_ = c.BindJSON(&json)

	telephone := json["telephone"]
	password := json["password"]
	valid1 := validation.Validation{}
	valid1.Required(telephone, "telephone").Message("telephone不能为空")
	valid1.Required(password, "password").Message("password不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		code = e.ERROR_NOT_EXIST
		if models.ExistUserByTelephone(telephone){
			code = e.ERROR_PASSWORD_NOT_MATCH
			user, b := models.Login(telephone, password)
			if b {
				code = e.ERROR_CREATE_TOKEN
				token, err := util.GenerateToken(user.ID)
				if err == nil {
					code = e.SUCCESS
					data["token"] = token
					data["user"] = user
				}
			}
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

func GetUserInfo(c *gin.Context) {
	var data interface{}
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS
	data = models.GetUserInfo(userId)


	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
}

func Register(c *gin.Context) {
	json := make(map[string]string)
	_ = c.BindJSON(&json)

	avatar := 1
	des := "还没有什么简介=="
	username := json["username"]
	password := json["password"]
	telephone := json["telephone"]
	verifyCode := json["verifyCode"]
	valid1 := validation.Validation{}
	valid1.Required(username, "username").Message("username不能为空")
	valid1.Required(password, "password").Message("password不能为空")
	valid1.Required(telephone, "telephone").Message("telephone不能为空")
	valid1.Required(verifyCode, "verifyCode").Message("verifyCode不能为空")
	code := e.INVALID_PARAMS
	if !valid1.HasErrors() {
		if models.ExistUserByTelephone(telephone) {
			code = e.ERROR_TELE_EXIST
		} else if !util.CheckVerifyCode(telephone, verifyCode) {
			code = e.ERROR_VERIFYCODE_NOT_MATCH
		} else {
			code = e.SUCCESS
			models.AddUser(username, password, telephone, des, avatar)
		}
	} else {
		for _, err := range valid1.Errors {
			log.Printf("err.key: %s, err.message: %s", err.Key, err.Message)
		}
		log.Println(json)



	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func UpdateUser(c *gin.Context) {
	token := c.Query("token")
	claims, _ := util.ParseToken(token)
	userId := claims.UserId
	code := e.SUCCESS

	if models.ExistUserByID(userId) {
		code = e.SUCCESS
		json := make(map[string]interface{})
		_ = c.BindJSON(&json)
		//avatar := json["avatar"]
		name := json["name"]
		des := json["description"]
		password := json["newpwd"]
		repassword := json["renewpwd"]
		verifyCode := json["code"]
		telephone := json["phone"]
		j := make(map[string]interface{})
		if name != nil {
			j["name"] = name
		}
		if des != nil {
			j["des"] = des
		}
		if password != nil && repassword != nil && verifyCode != nil && telephone != nil {
			if password != repassword {
				code = e.ERROR_REPASSWORD_NOT_MATCH
			} else if !util.CheckVerifyCode(telephone.(string), verifyCode.(string)) {
				code = e.ERROR_VERIFYCODE_NOT_MATCH
			} else {
				j["password"] = password
			}
		}
		//if avatar != nil {
		//	tmp1, _ := strconv.Atoi(avatar.(string))
		//	json["avatar"] = tmp1
		//}
		models.UpdateUser(userId, j)
	} else {
		code = e.ERROR_NOT_EXIST
	}

	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": make(map[string]string),
	})
}

func DeleteUser(c *gin.Context) {
	id := com.StrTo(c.Param("id")).MustInt()
	code := e.INVALID_PARAMS
	if models.ExistUserByID(id) {
		models.DeleteUser(id)
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