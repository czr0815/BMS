package v1

import (
	"fmt"
	"gin-example/models"
	"gin-example/pkg/e"
	"github.com/gin-gonic/gin"
	"net/http"
	"os"
	"path/filepath"
	"strconv"
	"time"
)

func AddImage(c *gin.Context) {
	fileHeader, err := c.FormFile("file")
	code := e.ERROR_FILE_UPLOAD
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"code": code,
			"msg": e.GetMsg(code),
			"data": make(map[string]string),
		})
		return
	}

	now := time.Now()
	fileType := "other"
	fileExt := filepath.Ext(fileHeader.Filename)
	if fileExt == ".jpg" || fileExt == ".png" || fileExt == ".gif" || fileExt == ".jpef=g" {
		fileType = "img"
	}
	fileDir := fmt.Sprintf("public/upload/%s/%d/%d/%d", fileType, now.Year(), now.Month(), now.Day())
	err = os.MkdirAll(fileDir, os.ModePerm)
	code = e.ERROR_FILE_DIR
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"code": code,
			"msg": e.GetMsg(code),
			"data": make(map[string]string),
		})
		return
	}

	timestamp := time.Now().Unix()
	fileName := fmt.Sprintf("%d-%s", timestamp, fileHeader.Filename)
	filePathStr := filepath.Join(fileDir, fileName)
	c.SaveUploadedFile(fileHeader, filePathStr)
	//code = e.ERROR_FILE_STORGE
	id := models.AddImage(filePathStr)

	code = e.SUCCESS
	data := make(map[string]string)
	data["imageId"] = strconv.Itoa(id)
	c.JSON(http.StatusOK, gin.H{
		"code": code,
		"msg": e.GetMsg(code),
		"data": data,
	})
	/*
	file, fileHeader, err := c.Request.FormFile("file")
	code := e.ERROR_FILE_UPLOAD
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"code": code,
			"msg": e.GetMsg(code),
			"data": make(map[string]string),
		})
		return
	}

	fileBuf := make([]byte, fileHeader.Size)
	_, err = file.Read(fileBuf)
	code = e.ERROR_FILE_READ
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"code": code,
			"msg": e.GetMsg(code),
			"data": make(map[string]string),
		})
		return
	}

	fileExt := strings.ToLower(path.Ext(fileHeader.Filename))
	fileSplit := strings.Split(fileExt, ".")
	url := strconv.FormatInt(time.Now().UnixNano(), 10) + "." + fileSplit[len(fileSplit)-1]
	newFile, err := os.Create(url)   //url
	code = e.ERROR_FILE_STORGE
	if err != nil {
		c.JSON(http.StatusOK, gin.H{
			"code": code,
			"msg": e.GetMsg(code),
			"data": make(map[string]string),
		})
		return
	}
	newFile.Write(fileBuf)
	defer newFile.Close()
	*/
}
