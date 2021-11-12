package util

import (
	"encoding/json"
	"fmt"
	"gin-example/pkg/setting"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/errors"
	"github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/common/profile"
	sms "github.com/tencentcloud/tencentcloud-sdk-go/tencentcloud/sms/v20210111"
	"log"
	"math/rand"
	"time"
)

var request *sms.SendSmsRequest
var client *sms.Client

func InitSMS() {
	var (
		err                                               error
		secretId, secretKey, sdkAppId, signName, templateId string
	)

	sec, err := setting.Cfg.GetSection("sms")
	if err != nil {
		log.Fatal(2, "Fail to get section 'database': %v", err)
	}

	secretId = sec.Key("SECRET_ID").String()
	secretKey = sec.Key("SECRET_KEY").String()
	sdkAppId = sec.Key("SDK_APP_ID").String()
	signName = sec.Key("SIGN_NAME").String()
	templateId = sec.Key("TEMPLATE_ID").String()

	credential := common.NewCredential(
		secretId,
		secretKey )

	cpf := profile.NewClientProfile()
	client, _ = sms.NewClient(credential, "ap-guangzhou", cpf)

	request = sms.NewSendSmsRequest()
	request.SmsSdkAppId = common.StringPtr(sdkAppId)    //短信应用ID
	request.SignName = common.StringPtr(signName)   //短信签名内容
	request.TemplateId = common.StringPtr(templateId)   //模板ID
}

func SendSMS(telephone string) {
	var (
		err                                               error
		secretId, secretKey, sdkAppId, signName, templateId string
	)

	sec, err := setting.Cfg.GetSection("sms")
	if err != nil {
		log.Fatal(2, "Fail to get section 'database': %v", err)
	}

	secretId = sec.Key("SECRET_ID").String()
	secretKey = sec.Key("SECRET_KEY").String()
	sdkAppId = sec.Key("SDK_APP_ID").String()
	signName = sec.Key("SIGN_NAME").String()
	templateId = sec.Key("TEMPLATE_ID").String()

	credential := common.NewCredential(
		secretId,
		secretKey )

	cpf := profile.NewClientProfile()
	client, _ := sms.NewClient(credential, "ap-guangzhou", cpf)

	request := sms.NewSendSmsRequest()
	request.SmsSdkAppId = common.StringPtr(sdkAppId)    //短信应用ID
	request.SignName = common.StringPtr(signName)   //短信签名内容
	request.TemplateId = common.StringPtr(templateId)   //模板ID

	var verifyCode string
	if existVerifyCode(telephone) {
		verifyCode = getVerifyCode(telephone)
	} else {
		verifyCode = createVerifyCode()
	}
	request.ExtendCode = common.StringPtr("")
	request.TemplateParamSet = common.StringPtrs([]string{verifyCode})   //参数，验证码
	request.PhoneNumberSet = common.StringPtrs([]string{"+86"+telephone})  //下发手机号
	SetRedisMinute(telephone, verifyCode, 15)

	response, err := client.SendSms(request)
	if _, ok := err.(*errors.TencentCloudSDKError); ok {
		fmt.Printf("An API error has returned: %s", err)
		return
	}
	if err != nil {
		panic(err)
	}
	b, _ := json.Marshal(response.Response)
	// 打印返回的json字符串
	fmt.Printf("%s", b)
}

func createVerifyCode() string {
	rnd := rand.New(rand.NewSource(time.Now().UnixNano()))
	verifyCode := fmt.Sprintf("%06v", rnd.Int31n(1000000))
	return verifyCode
}

func CheckVerifyCode(telephone, verifyCode string) bool {
	if GetRedisValue(telephone) != verifyCode{
		return false
	} else {
		DelRedis(telephone)
		return true
	}
}

func existVerifyCode(telephone string) bool {
	if ExistRedisKey(telephone) {
		return true
	} else {
		return false
	}
}

func getVerifyCode(telephone string) string {
	return GetRedisValue(telephone)
}