<template>
  <div id="echarts-earth"></div>
</template>

<script>
import "echarts-gl";
import city from "../../../public/js/geoCitys.js";
import axios from "axios";

export default {
  props: {
    code: {
      type: String,
      default: "",
    },
  },
  data() {
    return {
      chart: undefined,
      cityCode: this.code,
    };
  },
  mounted() {
    this.chart = this.$echarts.init(document.getElementById("echarts-earth"));
    const _this = this;

    this.chart.on("click", function (param) {
      console.log(param);
      let data = param.data || {};
      let id = data.id;
      _this.cityCode = _this.cityCode + "," + id;
      _this.setCityChart();
    });
    this.setCityChart();
    console.log(city);
  },
  methods: {
    setCityChart() {
      let dataUrl = "";
      let cityCode = "";

      let cityCodeArr = this.cityCode.split(",");
      if (cityCodeArr.length == 3) {
        cityCode = cityCodeArr[2].slice(0, 4) + "00";
        dataUrl = `../../../js/geometryCouties/${cityCode}.json`;
      } else if (cityCodeArr.length == 2) {
        cityCode = cityCodeArr[1].slice(0, 2);
        dataUrl = `../../../js/geometryProvince/${cityCode}.json`;
      } else {
        dataUrl = "../../../js/china.json";
        cityCode = "china";
        this.cityCode = "";
      }
      this.chart.clear();
      this.chart.showLoading({
        maskColor: "rgba(255,255,255,0)",
      });

      const allAreaData = [];
      axios({ url: dataUrl, method: "get" }).then((res) => {
        let json = res.data;
        let data = json.features.map((item, index) => {
          const geoAreaName = item.properties.name; // geo文件中的地理名称
          const currentArea = allAreaData.find((item) => {
            return item.name == geoAreaName;
          });
          let taskNumber = index,
            volunteerNumber = index;
          if (currentArea) {
            taskNumber = currentArea.taskNumber;
            volunteerNumber = currentArea.volunteerNumber;
          }
          return {
            name: geoAreaName,
            value: volunteerNumber,
            taskNumber: taskNumber, // 活动总数
            volunteerNumber: volunteerNumber * 10, // 志愿者总数
            // coord: item.properties.center,
            coord: item.properties.centroid,
            selected: true,
            id: item.properties.id,
          };
        });
        this.$echarts.registerMap(cityCode, json);
        this.chart.hideLoading();
        this.setChartOption(data, cityCode);
      });
    },
    setChartOption(data, mapName) {
      let option = {
        selectedMode: "multiple", // 选中效果固话
        tooltip: {
          // 提示框
          show: true,
          trigger: "item",
          formatter: function (params) {
            return params.name;
          },
        },
        series: [
          {
            name: "map3D",
            type: "map3D", // map3D / map
            map: mapName,
            label: {
              // 标签的相关设置
              show: true, // (地图上的城市名称)是否显示标签 [ default: false ]
              textStyle: {
                color: "#ffffff", // 地图初始化区域字体颜色
                fontSize: 14, // 字体大小
                opacity: 1, // 字体透明度
                backgroundColor: "rgba(0,23,11,1)", // 字体背景色
              },
            },
            tooltip: {
              //提示框组件。
              alwaysShowContent: true,
              hoverAnimation: true,
              trigger: "item", //触发类型 散点图
              enterable: true, //鼠标是否可进入提示框
              transitionDuration: 1, //提示框移动动画过渡时间
              triggerOn: "click",
              formatter: function (params) {
                if (params.name) {
                  var str = `
                  <div class="map-tooltip">
                    <div class="city-name">${params.name}</div>
                    <div class="city-info">订单总数：<span class="city-num">${params.data.volunteerNumber}</span></div>
                    <div class="city-info">储能站总数：<span class="city-num">${params.data.taskNumber}</span></div>
                  </div>
                  `;
                  return str;
                }
              },
              borderWidth: "1px",
              borderRadius: "4",
              borderColor: "rgba(0,0,0,0.9)",
              backgroundColor: "rgba(0,0,0,0.7)",
              textStyle: {
                color: "rgba(255,255,255,1)",
              },
              padding: [5, 10],
            },
            itemStyle: {
              color: "#3d8596",
              opacity: 0.5,
              borderWidth: 3,
              borderColor: "#50ACB2",
            },
            emphasis: {
              itemStyle: {
                color: "#50ACB2",
                opacity: 1,
              },
            },
            data: data,
          },
        ],
      };
      this.chart.setOption(option, true);
    },
  },
};
</script>