package e

var MsgFlags = map[int]string {
	SUCCESS : "ok",
	ERROR : "fail",
	INVALID_PARAMS : "请求参数错误",
	ERROR_CREATE_TOKEN : "token生成错误",
	ERROR_PARSE_TOKEN: "token解析错误",
	ERROR_TIMEOUT_TOKEN: "token验证超时",
	ERROR_CHECK_TOKEN : "token查验错误",
	ERROR_VERIFYCODE_NOT_MATCH : "短信验证码错误",
	ERROR_PASSWORD_NOT_MATCH : "用户名密码匹配错误",
	ERROR_REPASSWORD_NOT_MATCH : "两次输入不匹配",
	ERROR_TELE_EXIST: "电话号码已被注册",
	ERROR_NO_AUTHORITY: "没有对应的操作权限",
	ERROR_NOT_EXIST : "对象不存在",
	ERROR_AUTH_CHECK_TOKEN_FAIL : "Token鉴权失败",
	ERROR_AUTH_CHECK_TOKEN_TIMEOUT : "Token已超时",
	ERROR_AUTH_TOKEN : "Token生成失败",
	ERROR_AUTH : "Token错误",
	ERROR_FILE_UPLOAD : "文件上传错误",
	ERROR_FILE_DIR: "文件路径错误",
	ERROR_FILE_STORGE: "文件保存错误",
}

func GetMsg(code int) string {
	msg, ok := MsgFlags[code]
	if ok {
		return msg
	}

	return MsgFlags[ERROR]
}