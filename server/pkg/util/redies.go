package util

import (
	"gin-example/pkg/setting"
	"github.com/go-redis/redis"
	"log"
	"time"
)

func GetRedisClient() *redis.Client {
	sec, err := setting.Cfg.GetSection("redis")
	addr := sec.Key("ADDR").String()
	//password := sec.Key("PASSWORD").String()
	db, err := sec.Key("DB").Int()

	client := redis.NewClient(&redis.Options{
		Addr: addr,
		//Password: password,
		DB: db,
	})

	pong, err := client.Ping().Result()
	log.Println(pong, err)
	return client
}

func SetRedisHour(key, value string, i int64) *redis.StatusCmd {
	client := GetRedisClient()
	result := client.Set(key, value, time.Duration(i) * time.Hour)
	return result
}

func SetRedisMinute(key, value string, i int64) *redis.StatusCmd {
	client := GetRedisClient()
	result := client.Set(key, value, time.Duration(i) * time.Minute)
	return result
}

func GetRedisValue(key string) string {
	client := GetRedisClient()
	value, _ := client.Get(key).Result()
	return value
}

func ExistRedisKey(key string) bool {
	client := GetRedisClient()
	_, err := client.Get(key).Result()
	return err != redis.Nil
}

func DelRedis(key string) bool {
	client := GetRedisClient()
	num, _ := client.Del(key).Result()
	return num > 0
}