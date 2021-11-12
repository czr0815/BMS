<template>
  <div class="collect-container">
    <div class="chart-table-conatiner">
      <div class="charts-wrapper1 flex-conatiner">
        <Chart Id="collect-line-week" :option="collectLineWeekOption"/>
        <Chart Id="collect-pie-overview" :option="collectPieOverviewOption"/>
      </div>

      <div class="collect-table-wrapper box-shadow-templat">
        <span class="table-head">数据上传记录</span>
        <el-input
          v-model="searchContent"
          size="mini"
          @input="change($event)"
          placeholder="搜索"
          class="search"
        ></el-input>
        <div class="table-container e">
          <el-table
            :data="getPageData()"
            :row-style="tableRowStyle"
            :header-cell-style="tableHeaderColor"
            ref="multipleTable"
          >
            <el-table-column prop="id" label="编号" align="center" width="180">
            </el-table-column>
            <el-table-column
              prop="valueType"
              label="类型"
              align="center"
              width="70"
            ></el-table-column>
            <el-table-column
              prop="value"
              label="数值"
              align="center"
              width="70"
            ></el-table-column>
            <el-table-column
              prop="pointId"
              label="采集点"
              align="center"
              width="100"
            ></el-table-column>
            <el-table-column
              prop="bmuId"
              label="BMU"
              align="center"
              width="100"
            ></el-table-column>
            <el-table-column
              prop="bcuId"
              label="BCU"
              align="center"
              width="100"
            ></el-table-column>
            <el-table-column
              prop="bsuId"
              label="BSU"
              align="center"
              width="100"
            ></el-table-column>
            <el-table-column
              prop="collTime"
              label="上传时间"
              align="center"
              width="140"
            ></el-table-column>
          </el-table>
        </div>
        <el-pagination
          @current-change="current_change"
          @size-change="handleSizeChange"
          :current-page="currentPage"
          :page-sizes="[3, 5, 10, 15]"
          :page-size="pageSize"
          :total="collectTable.length"
          layout="total, sizes, prev, pager, next, jumper"
          background
        >
        </el-pagination>
      </div>
    </div>
  </div>
</template>

<script>
import {mapGetters} from "vuex"
import { defineComponent } from '@vue/composition-api'
import Chart from "@/components/Charts"
import {
  CollectLineWeeOption,
  collectPieOverviewOption
} from "@/utils/chartOpt"
import {
  getCollectWeek,
  getCollectOverview,
  getCollectsByStationId,
  getStationsForUser,
  getStationInfo
} from "@/api/collect"

export default defineComponent({
  name: "Collect",
  components: {Chart},
  computed: {
    ...mapGetters(["name"])
  },
  data() {
    return {
      weekOption : {},
      overviewOption: {},
      collectTable: [],
      stationTable: [],
      stationInfo: {}
    };
  },
  mounted() {
    this.getData()
  },
  methods: {
    getData() {
      getCollectWeek().then((res) => {
        this.weekOption = CollectLineWeeOption(res.data);
      });
      getCollectOverview().then((res) => {
        this.overviewOption = collectPieOverviewOption(res.data);
      }),
      getCollectsByStationId().then((res) => {
        this.collectTable = res.data;
      }),
      getStationsForUser().then((res) => {
        this.stationTable = res.data;
      }),
      getStationInfo().then((res) => {
        this.stationInfo = res.data;
      })
    },
    getPageData() {
      var input = this.searchContent && this.searchContent.toLowerCase();
      var items = this.collectTable;
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
    current_change: function (currentPage) {
      this.currentPage = currentPage;
    },
    handleSizeChange: function (val) {
      this.pageSize = val;
    },
  }
})
</script>

<style lang="scss" scoped>
.collect-container {
  display: flex;
}
.flex-container {
  display: flex;
  width: 100%;
  justify-content: space-between;
  margin-bottom: 15px;
}
.charts-wrapper1 .chart-template {
  width: 49%;
}
.charts-wrapper2 .chart-template {
  width: 32.5%;
}
.collect {
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
  width: calc(100%);
  display: inline-block;
  vertical-align: top;
}
.collect-table-wrapper {
  background: white;
  padding: 15px;
  border-radius: 15px;
  .table-head {
    font-size: 20px;
    font-weight: bold;
    color: #2c3653;
    margin-bottom: 5px;
    padding-left: 5px;
  }
  .search {
    width: 300px;
    margin-left: 20px;
  }
  .progress {
    width: 100px;
    .progress-bg {
      position: relative;
      width: 95%;
      height: 5px;
      background: rgba(0, 139, 157, 0.2);
      border-radius: 5px;
    }
    .progress-value {
      position: absolute;
      left: 0;
      top: 0;
      height: 5px;
      background: #73c0de;
      border-radius: 5px;
    }
  }

  .progress.low {
    .progress-bg {
      background: rgba(249, 110, 91, 0.2);
    }
    .progress-value {
      background: #f96e5b;
    }
  }
}
</style>
