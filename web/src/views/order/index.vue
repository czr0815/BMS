<template>
  <div class="order-container">
    <div class="chart-table-container">
      <!-- <div class="dashboard-text">仪表盘 Dashboard</div>
      <p class="gray-text">储能站的所有细节信息都在这里...</p> -->
      <div class="charts-wrapper1 flex-container">
        <Chart Id="week-line" :option="lineOption" />
        <Chart Id="day-line" :option="circleOption" />
      </div>

      <div class="order-table-wrapper box-shadow-templat">
        <div class="inputs-container">
          <div class="select1-container">
            <el-select v-model="orderForum.device" placeholder="请选择查询设备(默认储能站" @change="selectDeviceOptions()">
              <el-option
                v-for="item in deviceOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value">
              </el-option>
            </el-select>
          </div>
          <div class="input-container">
            <el-input v-model="orderForum.deviceId" placeholder="请输入设备ID(默认全选" class></el-input>
          </div>
          <div class="select2-container">
            <el-select v-model="orderForum.valueType" placeholder="请选择查询数据类型(默认全选">
              <el-option
                v-for="item in typeOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value">
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
            <el-button type="primary" plain @click="getOrderTable()">点击查询</el-button>
          </div>
        </div>
        <div class="table-container">
          <el-table
            v-loading="loading"
            :data="orderTable"
            style="width: 100%"
            :default-sort = "{prop: 'startTime', order: 'descending'}"
            :header-cell-style="{'text-align':'center'}"
          >
            <el-table-column
              prop="id"
              label="订单ID"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="orderType"
              label="订单种类"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="stationId"
              label="储能站ID"
              sortable
              width="180"
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
              prop="status"
              label="当前状态"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="electricity"
              label="订单电量"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="startTime"
              label="开始时间"
              sortable
              align="center">
            </el-table-column>
            <el-table-column
              prop="endTime"
              label="结束时间"
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
import {
  OrderLineOption,
  OrderCircleOption,
} from "@/utils/chartOpt";
import {
  getOrderStationStat,
  getOrderTimeStat,
  getOrders,
} from "@/api/order";
export default {
  name: "Order",
  components: { Chart },
  computed: {
    ...mapGetters(["name"]),
  },
  data() {
    return {
      lineOption: {},
      circleOption: {},

      deviceOptions: [{
        value: 'Station',
        label: '储能站',
      }, {
        value: 'BCU',
        label: 'BCU',
      }],
      typeOptions: [{
        value: 'all',
        label: '全种类数据',
      }, {
        value: 'dischareg',
        label: '放电订单',
      },{
        value: 'charge',
        label: '充电订单',
      }],
      orderTable: [],
      loading: false,
      orderForum: {
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
      getOrderStationStat().then((res) => {
        this.circleOption = OrderCircleOption(res.data);
      });

      getOrderTimeStat().then((res) => {
        this.lineOption = OrderLineOption(res.data);
      })

      this.getOrderTable();
    },
    getOrderTable() {
      this.loading = true;
      this.orderForum.timeRange[0] = parseInt(this.timeRange[0].getTime()/1000)
      this.orderForum.timeRange[1] = parseInt(this.timeRange[1].getTime()/1000)
      if(this.orderForum.timeRange[0]>this.orderForum.timeRange[1]){
        var tmp = this.orderForum.timeRange[0]
        this.orderForum.timeRange[0] = this.orderForum.timeRange[1]
        this.orderForum.timeRange[1] = tmp
      }
      getOrders(this.orderForum).then((res) => {
        this.orderTable = res.data;
      })
      this.loading = false;
    },
    getPageData() {
      var input = this.searchContent && this.searchContent.toLowerCase();
      var items = this.orderTable;
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
.order-container {
  display: flex;
}
.flex-container {
  display: flex;
  width: 100%;
  justify-content: space-between;
  margin-bottom: 15px;
}
.chart-template {
  width: 49.5%;
  height: 370px;
}
.order {
  &-container {
    margin: 15px 10px;
  }
  &-text {
    font-size: 30px;
    line-height: 46px;
    color: #1f2a49;
  }
}
.chart-table-container {
  width: 100%;
  display: inline-block;
  vertical-align: top;
}
.profile-container {
  display: flex;
  flex-direction: column;
}
.order-table-wrapper {
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
