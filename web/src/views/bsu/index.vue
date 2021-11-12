<template>
  <div class="bcu-container">
    <!-- 修改储能站信息弹框 -->
    <div>
      <el-dialog :title="stationformTitle" :visible.sync="stationFormVisible" width="35%">
        <el-form :model="stationform" ref="stationForm" label-width="100px" >
          <el-form-item label="名称">
            <el-input v-model="stationform.name" auto-complete="off" style="width:300px"></el-input>
          </el-form-item>
          <el-form-item label="经度">
            <el-input v-model="stationform.longitude" auto-complete="off" style="width:300px"></el-input>
          </el-form-item>
          <el-form-item label="纬度">
            <el-input v-model="stationform.latitude" auto-complete="off" style="width:300px"></el-input>
          </el-form-item>
          <el-form-item label="从属用户">
            <el-input v-model="stationform.user" auto-complete="off" style="width:300px"></el-input>
          </el-form-item>
          <el-form-item label="运行状态">
            <el-select v-model="value" placeholder="请选择" style="width:300px">
              <el-option
                v-for="item in selectOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
              >
              </el-option>
            </el-select>
          </el-form-item>
        </el-form>
        <div slot="footer" class="dialog-footer">
          <el-button type="primary" @click="sureFormSubmit">提 交</el-button>
        </div>
      </el-dialog>
    </div>

    <div class="chart-table-container">
      <!-- <div class="dashboard-text">仪表盘 Dashboard</div>
      <p class="gray-text">储能站的所有细节信息都在这里...</p> -->
      <div class="charts-wrapper1 flex-container">
        <Chart Id="bcu-status" :option="bcustatOption" />
      </div>
      <div class="charts-wrapper2 flex-container">
        <Chart Id="order-status" :option="orderstatOption" />
        <Chart Id="warning-status" :option="warningstatOption" />
        <Chart Id="charge-status" :option="chargestatOption" />
      </div>

      <div class="slave-table-wrapper box-shadow-templat">
        <span class="table-head">下级从属列表</span>
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
            :row-style="table - row"
            :header-cell-style="tableHeaderColor"
            ref="multipleTable"
          >
            <el-table-column label="名称" align="center" width="220">
              <template slot-scope="scope">
                <div class="name-wrapper">
                  <v-gravatar
                    :email="`${scope.row.name}@dazto.com`"
                    hostname="gravatar.zeruns.tech"
                  />
                  <div class="name">
                    <div class="content" :class="scope.row.status">
                      {{ scope.row.name }}
                    </div>
                    <div class="status gray-text">{{ scope.row.status }}</div>
                  </div>
                </div>
              </template>
            </el-table-column>
            <el-table-column prop="id" label="ID" align="center" width="110">
            </el-table-column>
            <el-table-column
              prop="administrator"
              label="管理员"
              align="center"
              width="150"
            ></el-table-column>
            <el-table-column
              prop="duration"
              label="运行时长"
              align="center"
              width="90"
            ></el-table-column>
            <el-table-column
              prop="online"
              label="上次上线时间"
              align="center"
              width="200"
            >
            </el-table-column>
            <el-table-column
              prop="warning"
              label="待处理报警"
              align="center"
              width="100"
            ></el-table-column>
            <el-table-column
              prop="left"
              label="当前电量"
              align="center"
              width="120"
              ><template slot-scope="scope">
                <div class="progress" :class="{ low: scope.row.left < 50 }">
                  <div class="progress-bg">
                    <div
                      class="progress-value"
                      :style="`width: ${scope.row.left}%`"
                    ></div>
                  </div>
                </div>
              </template>
            </el-table-column>
          </el-table>
        </div>
        <el-pagination
          @current-change="current_change"
          @size-change="handleSizeChange"
          :current-page="currentPage"
          :page-sizes="[3, 5, 10, 15]"
          :page-size="pageSize"
          :total="slaveTable.length"
          layout="total, sizes, prev, pager, next, jumper"
          background
        >
        </el-pagination>
      </div>
    </div>

    <div v-if="stationInfo.name" class="profile-container box-shadow-template">
      <div class="profile-top">
        BSU信息
        <div class="float-right">
          <i class="el-icon-arrow-right" @click="goBack"></i>
        </div>
      </div>
      <div class="profile-head">
        <div class="head-bg">
          <div class="float-right">
            <i class="el-icon-edit" @click="openStationForm"></i>
          </div>
        </div>
        <div class="profile-avatar">
          <div class="avatar-wrapper">
            <v-gravatar
              :email="`${stationInfo.name}@dazto.com`"
              hostname="gravatar.zeruns.tech"
            />
          </div>
          <div style="font-weight: bold">
            {{ stationInfo.name }}
            <i
              class="el-icon-star-on"
              style="
                color: #ff6e5c;
                font-size: 22px;
                position: relative;
                top: 2px;
              "
            />
          </div>
          <div class="bcuname">
            {{ bcuName }}
          </div>
          <div class="profile-info">
            <div
              class="status-wrapper"
              :class="{
                sight: stationInfo.status == 1,
                medium: stationInfo.status == 2,
                serious: stationInfo.status == -1,
              }"
            >
              <div>{{ getStatusText() }}</div>
            </div>
            <div class="position-wrapper">
              <div>{{ stationInfo.position }}</div>
            </div>
          </div>

          <div class="profile-numbers">
            <div class="profile-number" v-for="item in 3" :key="item">
              <div class="icon">
                <i :class="icons[item - 1]" />
              </div>
              <div class="value">
                {{ stationInfo.number[item - 1] }}
              </div>
              <div class="name gray-text">
                {{ iconNames[item - 1] }}
              </div>
            </div>
          </div>
        </div>

        <div class="profile-warnings">
          <div class="warning-title">近期报警记录</div>
          <div class="warning-box">
            <div
              class="warning-item float-shadow"
              v-for="(item, idx) in warningRecord"
              :key="idx"
            >
              <i :class="icons[2]" />
              <div
                style="margin-left: 8px"
                :class="{
                  sight: item.level == 1,
                  medium: item.level == 2,
                  serious: item.level == 3,
                }"
              >
                {{ item.warn }}
              </div>
              <div class="gray-text loc-right">{{ item.uploadTime }}</div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { mapGetters } from "vuex";
import Chart from "@/components/Chart";
import List from "@/components/List";

import {
  StationStatOption,
  OrderStatusOption,
  WarningStatusOption,
  ChargeStatusOption,
} from "@/utils/chartOpt";
import {
  getBcuStatData,
  getOrderStatus,
  getWarningStatus,
  getChargeStatus,
  getSlaveTable,
} from "@/api/bcu";
import { getStationInfo, getWarningRecord } from "@/api/station";
export default {
  name: "Bcu",
  components: { Chart, List },
  computed: {
    ...mapGetters(["name"]),
  },
  data() {
    return {
      //储能站弹出表单
      stationformTitle: "修改BSU信息", //表单标题
      stationFormVisible: false, //表单是否可见
      stationform: {
        //这个表之后可以直接用下边的 stationInfo表 这里先作为示例使用
        name: "",
        longitude:"",
        latitude:"",
        user:"",
      },
      selectOptions: [
        {
          value: "选项1",
          label: "online",
        },
        {
          value: "选项2",
          label: "offline",
        },
        
      ],
      value: "",
      //统计图表
      bcustatOption: {},
      orderstatOption: {},
      warningstatOption: {},
      chargestatOption: {},
      slaveTable: [],

      //BCU信息
      icons: ["el-icon-download", "el-icon-fork-spoon", "el-icon-upload2"],
      iconNames: ["累计放电量", "累计订单数", "累计充电量"],
      statusText: "当前离线",
      bcuName: "BCU ID-001",
      stationInfo: [],
      warningRecord: [],

      //分页
      pageSize: 3, //每页的数据条数
      currentPage: 1, //默认开始页面
    };
  },
  mounted() {
    this.getData();
  },
  methods: {
    // 打开对话框
    openStationForm() {
      this.stationFormVisible = true;
    },

    //提交表单,需要重写
    sureFormSubmit() {
      let _this = this;
      let api = "";
      _this.axios.post(api, {}).then((res) => {
        console.log(res);
      });
    },

    //返回上一页
    goBack(){
      this.$router.back();
    },
    
    getData() {
      getBcuStatData().then((res) => {
        this.bcustatOption = StationStatOption(res.data);
      });

      getOrderStatus().then((res) => {
        this.orderstatOption = OrderStatusOption(res.data);
      });

      getWarningStatus().then((res) => {
        this.warningstatOption = WarningStatusOption(res.data);
      });

      getChargeStatus().then((res) => {
        this.chargestatOption = ChargeStatusOption(res.data);
      });

      getSlaveTable().then((res) => {
        this.slaveTable = res.data;
      });

      getStationInfo().then((res) => {
        this.stationInfo = res.data;
      });

      getWarningRecord().then((res) => {
        this.warningRecord = res.data;
      });
    },
    getStatusText: function () {
      if (this.stationInfo.status === 1) {
        return "正常运行";
      } else if (this.stationInfo.status === 2) {
        return "产生较多异常";
      } else {
        return "当前离线";
      }
    },
    getPageData() {
      var input = this.searchContent && this.searchContent.toLowerCase();
      var items = this.slaveTable;
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
.bcu-container {
  display: flex;
  margin: 10px 10px;
}
.flex-container {
  display: flex;
  width: 100%;
  justify-content: space-between;
  margin-bottom: 15px;
}
.charts-wrapper1 .chart-template {
  width: 100%;
}
.charts-wrapper2 .chart-template {
  width: 32.5%;
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
  width: calc(100% - 385px);
  display: inline-block;
  vertical-align: top;
}

.profile-container {
  margin-left: 15px;
  vertical-align: top;
  display: inline-block;
  width: 370px;
  background: white;
  border-radius: 15px;
  padding: 15px;
}

.profile-head {
  .head-bg {
    margin: 30px 0;
    height: 100px;
    border-radius: 15px;
    background-image: url("../../assets/profile-bg-blue.jpg");
    background-size: auto 100%;
    background-position: center;
    .float-right {
      margin: 10px;
      padding: 10px;
      background: white;
      border-radius: 10px;
      color: #00abf3;
      .el-icon-edit {
        margin-left: 0;
      }
    }
  }
  .profile-avatar {
    text-align: center;
    transform: translateY(-55px);
    .avatar-wrapper {
      display: inline-block;
      overflow: hidden;
      width: 80px;
      height: 80px;
      border-radius: 50%;
    }
    img {
      width: 100%;
      height: 100%;
      object-fit: cover;
    }
    .bcuname {
      font-weight: bold;
      margin-top: 20px;
      font-size: 25px;
      color: #a13dcf;
    }
  }
}

.profile-info {
  text-align: center;
  margin: 25px 0;
  font: 14px;
  .status-wrapper {
    margin: 8px;
  }
  .sight {
    color: #00abf3;
    font-weight: bold;
  }
  .medium {
    color: #f96e5b;
    font-weight: bold;
  }
  .serios {
    color: gray;
    font-weight: bold;
  }
}

.current-electricity {
  margin: 30px 0;
  .electricity-items {
    margin: auto;
    width: 70%;
    display: flex;
    justify-content: space-between;
  }
  .electrcity-item {
    width: 9%;
    height: 8px;
    background: #e4e5e9;
    &.active {
      background: #00abf3;
    }
    &:first-child {
      border-top-left-radius: 5px;
      border-bottom-left-radius: 5px;
    }
    &:last-child {
      border-top-right-radius: 5px;
      border-bottom-right-radius: 5px;
    }
  }
}

.profile-numbers {
  margin: 0;
  display: flex;
  justify-content: space-evenly;
  .profile-number {
    padding: 0 15px;
    border-left: 1px solid gainsboro;
    &:first-child {
      border: none;
    }

    .icon {
      display: inline-block;
      padding: 10px 11px;
      background: #d29eeb;
      color: #a13dcf;
      font-size: 18px;
      border-radius: 50%;
    }

    .value {
      margin: 10px 0;
      font-size: 20px;
      font-weight: bold;
      color: #3a445f;
    }
  }
}

.profile-warnings {
  margin: 0;
  padding: 15px;
  background: #d29eeb;
  border-radius: 15px;
  .warning-title {
    font: 18px;
    margin-bottom: 8px;
  }
  .warning-box {
    .warning-item {
      font: 18px;
      padding: 8px 3px;
      align-items: center;
      background: white;
      border-radius: 10px;
      display: flex;
      margin: 9px 0;
      .loc-right {
        margin-left: auto;
      }
    }
    .sight {
      color: #3481b8;
      font-weight: bold;
    }
    .medium {
      color: #f96e5b;
      font-weight: bold;
    }
    .serious {
      color: red;
      font-weight: bold;
    }
  }
}

.divder {
  background: #c3e9fe;
  height: 5px;
  width: 100%;
}
.slave-table-wrapper {
  margin-top: 15px;
  background: white;
  padding: 15px;
  border-radius: 15px;
  width: 100%;
  .table-head {
    font-size: 17px;
    font-weight: bold;
    color: #2c3653;
    margin-bottom: 5px;
    padding-left: 5px;
  }
  .search {
    width: 300px;
    margin-left: 20px;
  }
  .name-wrapper {
    width: 220px;
    text-align: left;
    display: flex;
    align-items: center;
    img {
      width: 50px;
      height: 50px;
      border-radius: 10px;
    }
    .name {
      margin-left: 15px;
      .status {
        margin-top: 5px;
      }
      .content {
        position: relative;
        line-height: 20px;
        &::after {
          position: absolute;
          content: "";
          top: 5px;
          right: -15px;
          width: 7px;
          height: 7px;
          border-radius: 50%;
          background: #009d63;
        }
        &.offline::after {
          background: #f96e5b;
        }
      }
    }
  }
  .progress {
    width: 120px;
    .progress-bg {
      position: relative;
      width: 95%;
      height: 5px;
      background: rgba(0, 157, 99, 0.2);
      border-radius: 5px;
    }
    .progress-value {
      position: absolute;
      left: 0;
      top: 0;
      height: 5px;
      background: #009d63;
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
  .table-row {
    margin: 10px 0;
    padding: 10px;
    display: flex;
    align-items: center;
    text-align: center;
    border-radius: 15px;
    color: #959aa9;
    .delivered,
    .pending {
      color: #959aa9;
    }
  }
}

/* 弹框 */
.avatar-uploader .el-upload {
  border: 1px dashed #d9d9d9;
  border-radius: 6px;
  cursor: pointer;
  position: relative;
  overflow: hidden;
}
.avatar-uploader .el-upload:hover {
  border-color: #409eff;
}
.avatar {
  width: 100px;
  height: 100px;
  display: block;
}

.show-pwd {
  position: absolute;
  right: 10px;
  top: 7px;
  font-size: 16px;
  color: #889aa4;
  cursor: pointer;
  user-select: none;
}

.sendcode {
  width: 150px;
  height: 30px;
  background-color: #ff3000;
  border: 0;
  border-radius: 15px;
  color: #fff;
}
</style>
