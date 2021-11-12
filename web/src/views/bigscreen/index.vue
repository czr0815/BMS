<template>
  <div class="screen-container">
    <div class="screen-bg"></div>
    <el-row :gutter="10">
      <el-col :span="6">
        <ChartWrapper title="订单交易流水">
          <div class="circle-dots" style="height: 160px">
            <div class="circle-col">
              <CircleDot
                :offset="1"
                :value="order.total"
                :color="'#00A3FC'"
                title="全部订单"
                desc="这是一段描述"
              />
              <CircleDot
                :offset="2"
                :value="order.finish"
                :color="'#2F55F8'"
                title="完成订单"
              />
            </div>
            <div class="circle-col">
              <CircleDot
                :offset="3"
                :value="order.process"
                :color="'#FFB642'"
                title="进行中订单"
              />
              <CircleDot
                :offset="4"
                :value="order.invalid"
                :color="'#FD6059'"
                title="无效订单"
              />
            </div>
          </div>
        </ChartWrapper>
        <ChartWrapper title="时间营业额">
          <Chart Id="screen-time-income" :option="orderOption" />
        </ChartWrapper>
        <ChartWrapper title="报警种类占比">
          <Chart Id="screen-pay-method" :option="warningOption" />
        </ChartWrapper>
      </el-col>
      <el-col :span="12"> <div class="screen-center-wrapper">
        <div class="total-numbers">
          <div class="total-number-item" v-for="(item, idx) in totalNumbers1" :key="idx">
            <div class="value">
              <countTo :endVal="item.value" :duration="3000"/>
            </div>
            <div class="name">{{item.name}}</div>
          </div>
        </div>    
        <div class="earth-container">
          <Earth style="height: 700px"/>
        </div>
        <div class="total-numbers" style="margin-top: 620px">
          <Radar v-for="(item, idx) in totalNumbers2" :key="idx" :name="item.name" :value="item.value"/>
        </div>
        </div></el-col>
      <el-col :span="6">
        <ChartWrapper title="储能站详情">
          <div class="circle-dots" style="height: 160px">
            <div class="circle-col" style="width: 49%">
              <Loader :value="stationOption[0].value" color="#26C5F3" :rank="1" :name="stationOption[0].name" />
              <Loader :value="stationOption[1].value" color="#FAC348" :rank="2" :name="stationOption[1].name" />
              <Loader :value="stationOption[2].value" color="#E55B5A" :rank="3" :name="stationOption[2].name" />
            </div>
            <div class="circle-col" style="width: 49%">
              <Loader :value="stationOption[3].value" color="#305FEC" :rank="4" :name="stationOption[3].name" />
              <Loader :value="stationOption[4].value" color="#32C95F" :rank="5" :name="stationOption[4].name" />
              <Loader :value="stationOption[5].value" color="#FAFAFA" :rank="6" :name="stationOption[5].name" />
            </div>
          </div>
        </ChartWrapper>

        <ChartWrapper title="电池电量">
          <div class="battery-wrapper">
            <Chart Id="battery-pie" :option="batteryOption" />
            <Loader2 color="#00A1F5" :value="batteryStat.abundant" name="电量充足" />
            <Loader2 color="#3262FE" :value="batteryStat.medium" name="电量一般" />
            <Loader2 color="#FFBE45" :value="batteryStat.less" name="电量略低" />
            <Loader2 color="#FE6664" :value="batteryStat.scare" name="电量极低" />
          </div>
        </ChartWrapper>
        <ChartWrapper title="访问统计">
          <div class="access-wrapper">
            <Chart Id="access-bar" :option="accessOption" />
          </div>
        </ChartWrapper>
      </el-col>
    </el-row>
  </div>
</template>

<script>
import ChartWrapper from "@/components/Screen/chartWrapper.vue";
import Chart from "@/components/Chart";
import countTo from "vue-count-to";
import CircleDot from "@/components/Screen/circleDot.vue";
import Loader from "@/components/Screen/loader.vue";
import Loader2 from "@/components/Screen/loader2.vue";
import Radar from "@/components/Screen/radar.vue";
import Earth from "@/components/Screen/earth.vue";
import {
  getScreenOrderStat,
  getScreenWarningStat,
  getScreenOverallStat,
  getScreenStationStat,
  getScreenBatteryStat
} from "@/api/bigscreen"
import {
  ScreenOrderIncomeOption,
  ScreenWarningTypeOption,
  ScreenBatteryOption,
  ScreenAccessOption,
} from "@/utils/chartOpt";
export default {
  components: { ChartWrapper, CircleDot, Chart, Loader, Loader2, countTo, Radar, Earth },
  data() {
    return {
      order: {
        total: 0,
        finish: 0,
        process: 0,
        invalid: 0
      },
      orderOption: {},
      warningOption: {},
      totalNumbers1: [{name: "--", value: 0}, {name: "--", value: 0}, {name: "--", value: 0}],
      totalNumbers2: [{name: "--", value: 0}, {name: "--", value: 0}, {name: "--", value: 0}],
      stationOption: [{name: "--", value: 0}, {name: "--", value: 0}, {name: "--", value: 0}, {name: "--", value: 0}, {name: "--", value: 0}, {name: "--", value: 0}],
      batteryOption: {},
      batteryStat: {abundant:0, medium:0, less:0, scare:0},
      accessOption: {},
    };
  },

  mounted() {
    this.getData();
  },

  methods: {
    getData() {
      getScreenOrderStat().then((res) => {
        this.order.total = res.data.total
        this.order.finish = res.data.finish
        this.order.process = res.data.process
        this.order.invalid = res.data.invalid
        this.orderOption = ScreenOrderIncomeOption(res.data.option);
      })

      getScreenWarningStat().then((res) => {
        this.warningOption = ScreenWarningTypeOption(res.data)
      })

      getScreenOverallStat().then((res) => {
        this.totalNumbers1 = []
        this.totalNumbers1.push(res.data.order)
        this.totalNumbers1.push(res.data.discharge)
        this.totalNumbers1.push(res.data.charge)
        this.totalNumbers2 = []
        this.totalNumbers2.push(res.data.station)
        this.totalNumbers2.push(res.data.bcu)
        this.totalNumbers2.push(res.data.bmu)
      })

      getScreenStationStat().then((res) => {
        var data = res.data
        this.stationOption=[]
        for(var i=0;i<data.length;i++){
          this.stationOption.push({value:Number(data[i].soc.toFixed(2)), name:data[i].stationName})
        }
        var len=this.stationOption.length
        for(var i=len;i<6;i++){
          this.stationOption.push({value:0, name:'--'})
        }
      })

      getScreenBatteryStat().then((res) => {
        this.batteryOption = ScreenBatteryOption(res.data)
        this.batteryStat = res.data
      })

      this.accessOption = ScreenAccessOption({});
    },
  },
};
</script>

<style lang="scss" scoped>
.screen-bg {
  position: fixed;
  background-color: #020507;
  background-image: url("../../assets/bg.png");
  left: 0;
  top: 0;
  bottom: 0;
  right: 0;
}
.screen-container {
  color: white;
  font-family: "vermin";
  padding: 15px;
  .chart-template {
    width: 100%;
    background: none;
    height: 240px;
  }
}

.circle-dots {
  display: flex;
  flex-wrap: wrap;
  justify-content: space-between;
}

.circle-dot-wrapper {
  margin: 10px 0;
  width: 50%;
}

.battery-wrapper {
  position: relative;

  .circle-wrapper-2 {
    position: absolute;

    &:nth-child(2),
    &:nth-child(4) {
      left: 0;
    }

    &:nth-child(3),
    &:nth-child(5) {
      right: 0;
    }

    &:nth-child(2),
    &:nth-child(3) {
      top: 20px;
    }

    &:nth-child(4),
    &:nth-child(5) {
      bottom: 20px;
    }
  }
}
.total-numbers {
    display: flex;
    justify-content: space-between;
    position: relative;
    z-index: 1001;
    .total-number-item {
        text-align: center;
        margin: 0 15px;
        padding: 15px;
        flex-grow: 1;
        background-image: url("../../assets/blue-bg-four.png");
        background-size: 100% 100%;
        .value {
            font-size: 24px;
            font-weight: bold;
        }
        .name {
            margin-top: 15px;
            opacity: 0.6;
        }
    }
}
.screen-center-wrapper {
    position: relative;
    .earth-container {
        position: absolute;
        top: 0;
        left: 0;
        width: 100%;
    }
}
</style>