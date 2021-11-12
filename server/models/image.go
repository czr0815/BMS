package models

type Image struct {
	Model
	Url string `json:"url"`
}

func ExistImageById(id int) bool {
	var image Image
	db.Select("id").Where("id=?", id).First(&image)
	return image.ID > 0
}

func GetImage(id int) (image Image) {
	db.Select("id").Where("id=?", id).First(&image)
	return
}

func AddImage(url string) int {
	db.Create(&Image{
		Url: url,
	})
	var ids []int
	db.Raw("select LAST_INSERT_ID() as id").Pluck("id", &ids)
	return ids[0]
}

func DeleteImage(id int) bool {
	db.Where("id = ?", id).Delete(&Image{})
	return true
}
