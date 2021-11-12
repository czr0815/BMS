<template>
  <div class="warning-container">
    <div class="chart-table-container">
      <div class="charts-wrapper1 flex-container">
        <MixedChart Id="warning-line" :option="lineOption" />
        <Chart Id="warning-bar" :option="barOption" />
      </div>

      <div class="warning-table-wrapper box-shadow-templat">
        <div class="inputs-container">
          <div class="select1-container">
            <el-select v-model="warningForum.device" placeholder="请选择查询设备(默认储能站" @change="selectDeviceOptions()">
              <el-option
                v-for="item in deviceOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value">
              </el-option>
            </el-select>
          </div>
          <div class="input-container">
            <el-input v-model="warningForum.deviceId" placeholder="请输入设备ID(默认全选" class></el-input>
          </div>
          <div class="select2-container">
            <el-select v-model="warningForum.valueType" placeholder="请选择查询数据类型(默认全选">
              <el-option
                v-for="item in typeOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
                :disabled="item.disabled">
              </el-option>
            </el-select>
          </div>
          <div class="timerange-container">
            <el-date-picker
              v-model="timeRange"
              type="datetimerange"
              range-separator="至"
              start-placeholder="开始日期"
              end-placeholder="结束日期">
            </el-date-picker>
          </div>
          <div class="button-container">
            <el-button type="primary" plain @click="getWarningTable()">点击查询</el-button>
          </div>
        </div>
        <div class="table-container">
          <el-table
            v-loading="loading"
            :data="warningTable"
            style="width: 100%"
            :default-sort = "{prop: 'collTime', order: 'descending'}"
            :header-cell-style="{'text-align':'center'}"
          >
            <el-table-column
              prop="id"
              label="报警ID"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="valueType"
              label="报警种类"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="bcuId"
              label="BCUID"
              sortable
              width="180"
              align="center">
            </el-table-column>
            <el-table-column
              prop="bmuId"
              label="BMUID"
              sortable
              width="180"
              align="center">
            </el-table-column>
            <el-table-column
              prop="pointId"
              label="收集点ID"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="value"
              label="异常值"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="collTime"
              label="触发异常时间"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="operTime"
              label="处理异常时间"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="operator"
              label="异常处理人"
              sortable
              align="center">
            </el-table-column>
          </el-table>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { mapGetters } from "vuex";
import Chart from "@/components/Chart";
import MixedChart from "@/components/Warning/MixedChart.vue"
import {
  WarningMixedOption,
  WarningBarOption,
} from "@/utils/chartOpt";
import {
  getWarningStationStat,
  getWarningTimeStat,
  getWarnings,
} from "@/api/warning";
export default {
  name: "Warning",
  components: { Chart, MixedChart },
  computed: {
    ...mapGetters(["name"]),
  },
  data() {
    return {
      lineOption: {},
      barOption: {},

      deviceOptions: [{
        value: 'Station',
        label: '储能站',
      }, {
        value: 'BCU',
        label: 'BCU',
      }, {
        value: 'BMU',
        label: 'BMU',
      }],
      typeOptions: [{
        value: 'all',
        label: '全种类数据',
        disabled: false,
      }, {
        value: 'BCUVoltage',
        label: 'BCU总体电压',
        disabled: false,
      }, {
        value: 'BCUInsulation',
        label: 'BCU绝缘性',
        disabled: false,
      }, {
        value: 'BMUVoltage',
        label: 'BMU单点电压',
        disabled: false,
      }, {
        value: 'BMUCurrent',
        label: 'BMU单点电流',
        disabled: false,
      }, {
        value: 'BMUSmoke',
        label: 'BMU单点烟雾',
        disabled: false,
      }, {
        value: 'BMUTemperature',
        label: 'BMU单点温度',
        disabled: false,
      }],
      warningTable: [],
      loading: false,
      warningForum: {
        device: "Station",
        deviceId: 0,
        valueType: "all",
        timeRange: [parseInt(new Date(new Date().getTime()-(3*3600*24*1000)).getTime()/1000), parseInt(new Date().getTime/1000)]
      },
      timeRange: [new Date(new Date().getTime()-(3*3600*24*1000)), new Date()],

      pageSize: 3, //每页的数据条数
      currentPage: 1, //默认开始页面
    };
  },
  mounted() {
    this.getData();
  },
  methods: {
    getData() {
      getWarningStationStat().then((res) => {
        this.barOption = WarningBarOption(res.data);
      });

      getWarningTimeStat().then((res) => {
        this.lineOption = WarningMixedOption(res.data);
      })

      this.getWarningTable();
    },
    getWarningTable() {
      this.loading = true;
      this.warningForum.timeRange[0] = parseInt(this.timeRange[0].getTime()/1000)
      this.warningForum.timeRange[1] = parseInt(this.timeRange[1].getTime()/1000)
      if(this.warningForum.timeRange[0]>this.warningForum.timeRange[1]){
        var tmp = this.warningForum.timeRange[0]
        this.warningForum.timeRange[0] = this.warningForum.timeRange[1]
        this.warningForum.timeRange[1] = tmp
      }
      getWarnings(this.warningForum).then((res) => {
        this.warningTable = res.data;
      })
      this.loading = false;
    },
    selectDeviceOptions(){
        if(this.warningForum.device==='Station'){
          this.typeOptions[0].disabled=false;
          for(var i=1;i<7;i++){
            this.typeOptions[i].disabled=false;
          }
        } else if(this.warningForum.device==='BCU'){
          this.typeOptions[0].disabled=false;
          for(var i=1;i<3;i++){
            this.typeOptions[i].disabled=false;
          }
          for(var i=3;i<7;i++){
            this.typeOptions[i].disabled=true;
          }
        } else if(this.warningForum.device==='BMU'){
          this.typeOptions[0].disabled=false;
          for(var i=1;i<3;i++){
            this.typeOptions[i].disabled=true;
          }
          for(var i=3;i<7;i++){
            this.typeOptions[i].disabled=false;
          }
        }
      },
    getPageData() {
      var input = this.searchContent && this.searchContent.toLowerCase();
      var items = this.warningTable;
      var items1;
      if (input) {
        items1 = items.filter(function (item) {
          return Object.keys(item).some(function (key1) {
            return String(item[key1]).toLowerCase().match(input);
          });
        });
        items1 = items1.slice(
          (this.currentPage - 1) * this.pageSize,
          this.currentPage * this.pageSize
        );
      } else {
        items1 = items.slice(
          (this.currentPage - 1) * this.pageSize,
          this.currentPage * this.pageSize
        );
      }
      return items1;
    },
    change(e) {
      this.$forceUpdate();
    },
    //翻页
    current_change: function (currentPage) {
      this.currentPage = currentPage;
    },
    // 每页多少条
    handleSizeChange: function (val) {
      this.pageSize = val;
    },
  },
};
</script>

<style lang="scss" scoped>
.warning-container {
  display: flex;
  flex-direction: column;
}
.warning-container-top {
  display: flex;
}
.flex-container {
  display: flex;
  width: 100%;
  justify-content: space-between;
  margin-bottom: 15px;
}
.warning {
  &-container {
    margin: 15px 10px;
  }
  &-text {
    font-size: 30px;
    line-height: 46px;
    color: #1f2a49;
  }
}
.chart-template {
  width: 49.5%;
  height: 370px;
}
.chart-table-container {
  width: 100%;
  display: inline-block;
  vertical-align: top;
}
.warning-table-wrapper {
  background: white;
  padding: 15px;
  border-radius: 15px;
  .inputs-container {
    display: flex;
    .select1-container {
      margin-right: 10px;
    }
    .input-container {
      width: 300px;
      margin-left: 10px;
      margin-right: 10px;
    }
    .select2-container {
      margin-left: 10px;
      margin-right: 10px;
    }
    .timerange-container {
      margin-left: 10px;
      margin-right: 10px;
    }
    .button-container {
      margin-left: 10px;
    }
  }
}
</style>
