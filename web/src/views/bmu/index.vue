<template>
  <div class="bmu-container">
    <div>
      <el-dialog title="添加BMU" :visible.sync="bmuCreateVisible" width="35%">
        <el-form :model="bmuCreate" ref="stationForm" label-width="100px" >
          <el-form-item label="BCU的ID">
            <el-input v-model="bmuCreate.id" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
          <el-form-item label="名称">
            <el-input v-model="bmuCreate.name" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
          <el-form-item label="所属BCU">
            <el-input v-model="bmuCreate.bcuId" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
          <el-form-item label="电池数目">
            <el-input v-model="bmuCreate.batteryNum" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
          <el-form-item label="电池种类">
            <el-input v-model="bmuCreate.batteryType" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
        </el-form>
        <div slot="footer" class="dialog-footer">
          <el-button type="primary" @click="sureCreateSubmit">提 交</el-button>
        </div>
      </el-dialog>
    </div>

    <!-- 修改储能站信息弹框 -->
    <div>
      <el-dialog
        title="更新BMU信息"
        :visible.sync="bmuFormVisible"
        width="35%"
      >
        <el-form :model="bmuForm" ref="bmuForm" label-width="100px">
          <el-form-item label="名称">
            <el-input
              v-model="bmuForm.name"
              auto-complete="off"
              style="width: 350px"
            ></el-input>
          </el-form-item>
          <el-form-item label="所属BCU">
            <el-input
              v-model="bmuForm.bcuId"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="电池种类">
            <el-input
              v-model="bmuForm.batteryNum"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="电池类型">
            <el-input
              v-model="bmuForm.batteryType"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
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
        <Chart Id="bmu-data" :option="bmuDataStatOption" />
      </div>
      <div class="charts-wrapper2 flex-container">
        <Chart Id="warning-calendar" :option="warningCalendarOption" />
        <Chart Id="warning-status" :option="warningStatOption" />
        <Chart Id="soc-status" :option="socStatOption" />
      </div>
      <div class="charts-wrapper1 flex-container">
        <Chart Id="bmu-outline" :option="outlineStatOption" />
      </div>

      <div class="single-table-wrapper box-shadow-templat">
        <span class="table-head">BMU实时单体监测记录</span>
        <el-input
          v-model="searchContent"
          size="mini"
          @input="change($event)"
          placeholder="搜索"
          class="search"
        ></el-input>
        <div class="table-container e">
          <el-table
            :data="singleTable.slice((currentPage - 1) * pageSize, currentPage * pageSize)"
            ref="multipleTable"
          >
            <el-table-column
              prop="valueType"
              label="数据种类"
              align="center"
              width="200"
            >
            </el-table-column>
            <el-table-column
              prop="pointId"
              label="测量点编号"
              align="center"
              width="200"
            ></el-table-column>
            <el-table-column
              prop="value"
              label="数据值"
              align="center"
              width="200"
            ></el-table-column>
            <el-table-column
              prop="collTime"
              label="数据产生时间"
              align="center"
            ></el-table-column>
          </el-table>
        </div>
        <el-pagination
          @current-change="current_change"
          @size-change="handleSizeChange"
          :current-page="currentPage"
          :page-sizes="[6, 12, 18, 24]"
          :page-size="pageSize"
          :total="singleTable.length"
          layout="total, sizes, prev, pager, next, jumper"
          background
        >
        </el-pagination>
      </div>
    </div>

    <div v-if="bmuId" class="profile-container box-shadow-template">
      <div class="profile-top">
        <div class="profile-title">BMU信息</div>
        <div class="float-right profile-select">
          <el-select v-model="bmuId" placeholder="请选择" @change="selectBMU()">
            <el-option
              v-for="item in bmuOptions"
              :key="item.key"
              :label="item.label"
              :value="item.value">
            </el-option>
          </el-select>
        </div>
        <div class="float-right">
          <i class="el-icon-arrow-right" @click="goBack"></i>
        </div>
      </div>
      <div class="profile-head">
        <div class="head-bg">
          <div class="float-right">
            <i class="el-icon-edit" @click="openBMUForm"></i>
          </div>
          <div class="float-right">
            <i class="el-icon-circle-plus-outline" style="margin-left: 0px" @click="openBMUCreate"></i>
          </div>
        </div>
        <div class="profile-avatar">
          <div class="avatar-wrapper">
            <v-gravatar
              :email="`${bmuId}@dazto.com`"
              hostname="gravatar.zeruns.tech"
            />
          </div>
          <div style="font-weight: bold">
            ID-{{ bmuId }}
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
          <div class="bmuname">
            {{ bmuInfos[index].name }}
          </div>

          <div class="profile-info">
            <div
              class="status-wrapper"
              :class="{
                normal: bmuInfos[index].status == 1,
                warning: bmuInfos[index].status == 2,
                error: bmuInfos[index].status == 3,
                offline: bmuInfos[index].status == 0,
              }">
              <div>{{ getStatusText(bmuInfos[index].status) }}</div>
            </div>
            <div class="description-wrapper">
              <div>共{{ bmuInfos[index].batteryNum }}串{{ getBatteryType(bmuInfos[index].batteryType) }}电池</div>
            </div>
          </div>
          <div class="profile-blank"></div>

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              电池串单体电压(V)
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BMUVoltage')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number" v-for="item in 3" :key="item">
                <div class="icon">
                  <i :class="icons[item - 1]" />
                </div>
                <div class="value">
                  {{ bmuVoltage[item - 1] }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[item - 1] }}
                </div>
              </div>
            </div>
            <i class="profile-number-time grat-text">{{ bmuInfos[index].voltage.collTime }}</i>
          </div>

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              电池串单体SOC(%)
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BMUSOC')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number" v-for="item in 3" :key="item">
                <div class="icon">
                  <i :class="icons[item - 1]" />
                </div>
                <div class="value">
                  {{ bmuSoc[item - 1] }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[item - 1] }}
                </div>
              </div>
            </div>
            <i class="profile-number-time grat-text">{{ bmuInfos[index].soc.collTime }}</i>
          </div>

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              电流监测(A)
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BMUCurrent')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number">
                <div class="icon">
                  <i :class="icons[1]" />
                </div>
                <div class="value">
                  {{ bmuInfos[index].current.avg }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[1] }}
                </div>
              </div>
            </div>
            <i class="profile-number-time grat-text">{{ bmuInfos[index].current.collTime }}</i>
          </div>

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              烟雾监测
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BMUSmoke')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number" v-for="item in 3" :key="item">
                <div class="icon">
                  <i :class="icons[item - 1]" />
                </div>
                <div class="value">
                  {{ bmuSmoke[item - 1] }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[item - 1] }}
                </div>
              </div>
            </div>
            <i class="profile-number-time grat-text">{{ bmuInfos[index].smoke.collTime }}</i>
          </div>

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              温度监测(℃)
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BMUTemperature')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number" v-for="item in 3" :key="item">
                <div class="icon">
                  <i :class="icons[item - 1]" />
                </div>
                <div class="value">
                  {{ bmuTemperature[item - 1] }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[item - 1] }}
                </div>
              </div>
            </div>
            <i class="profile-number-time grat-text">{{ bmuInfos[index].temperature.collTime }}</i>
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
  BMUDataStatOption,
  BMUWarningStatOption,
  BMUWarningCalendarOption,
  BMUSOCStatOption,
  BMUOutlineStatOption
} from "@/utils/chartOpt";
import {
  getBMUDataStat,
  getBMUWarningStat,
  getBMUWarningCalendar,
  getBMUSOCStat,
  getSingleTable,
  getBMUInfos,
  getBMUOutlineStat,
  updateBMUInfo,
  addBMU
} from "@/api/bmu";
import {
  Battery_Ni_Cd_code,
  Battery_Ni_MH_code,
  Battery_LI_lon_code,
  Battery_LI_pol_code,
  Battery_Le_ac_code,
  Battery_Ni_Cd_text,
  Battery_Ni_MH_text,
  Battery_LI_lon_text,
  Battery_LI_pol_text,
  Battery_Le_ac_text
} from "@/utils/code";

export default {
  name: "BMU",
  components: { Chart, List },
  computed: {
    ...mapGetters(["name"]),
  },
  data() {
    return {
      bmuId: 0,
      index: 0,
      bmuInfos: [{
        id: 0,
        name: '默认BCU',
        bcuId: 0,
        status: 0,
        batteryNum: 0,
        batteryType: 0,
        voltage: {
          max: 0,
          avg: 0,
          min: 0,
          collTime: ""
        },
        soc: {
          max: 0,
          avg: 0,
          min: 0,
          collTime: ""
        },
        current: {
          avg: 0,
          collTime: ""
        },
        smoke: {
          max: 0,
          avg: 0,
          min: 0,
          collTime: ""
        },
        temperature: {
          max: 0,
          avg: 0,
          min: 0,
          collTime: ""
        }
      }],
      bmuOptions: [],
      icons: ["el-icon-download", "el-icon-fork-spoon", "el-icon-upload2"],
      iconNames: ["最大值", "平均值", "最小值"],
      bmuVoltage: [],
      bmuSoc: [],
      bmuSmoke: [],
      bmuTemperature: [],
      bmuDataStatOption: {},
      warningStatOption: {},
      warningCalendarOption: {},
      socStatOption: {},
      outlineStatOption: {},
      singleTable: [],
      searchContent: "",

      value: "",

      bmuFormVisible: false,
      bmuForm: {
        name: "",
        bcuId: 0+"",
        batteryNum: 0+"",
        batteryType: 0+"",
      },
      bmuCreateVisible: false,
      bmuCreate: {
        id: 0,
        name: "默认BMU",
        bcuId: 0,
        batteryNum: 0,
        batteryType: 0
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

      //分页
      pageSize: 6, //每页的数据条数
      currentPage: 1, //默认开始页面
    };
  },
  mounted() {
    this.bmuOptions = []
    for(var i=0;i<this.bmuInfos.length;i++){
      if(this.bmuInfos[i].id==this.bmuId){
        this.index = i;
      }
      this.bmuOptions.push({
        value: this.bmuInfos[i].id,
        label: this.bmuInfos[i].name,
        key: this.bmuInfos[i].id+""
      })
    }
    this.bmuVoltage[0] = this.bmuInfos[this.index].voltage.max
    this.bmuVoltage[1] = this.bmuInfos[this.index].voltage.avg
    this.bmuVoltage[2] = this.bmuInfos[this.index].voltage.min
    this.bmuSoc[0] = this.bmuInfos[this.index].soc.max
    this.bmuSoc[1] = this.bmuInfos[this.index].soc.avg
    this.bmuSoc[2] = this.bmuInfos[this.index].soc.min
    this.bmuSmoke[0] = this.bmuInfos[this.index].smoke.max
    this.bmuSmoke[1] = this.bmuInfos[this.index].smoke.avg
    this.bmuSmoke[2] = this.bmuInfos[this.index].smoke.min
    this.bmuTemperature[0] = this.bmuInfos[this.index].temperature.max
    this.bmuTemperature[1] = this.bmuInfos[this.index].temperature.avg
    this.bmuTemperature[2] = this.bmuInfos[this.index].temperature.min
    if(this.bmuId==0){
      this.getBMUInfo();
    } else {
      this.getData();
    }
  },
  watch: {
    '$route.params': {
	    handler (newVal) {
        if(this.$route.params.bmuId!=null){
          this.bmuId = this.$route.params.bmuId
          this.getBMUInfo();
        }
	    },
	    deep: true,
	    immediate: true,
	  }
  },
  methods: {
    // 打开对话框
    openBMUForm() {
      this.bmuFormVisible = true;
    },
    openBMUCreate() {
      this.bmuCreateVisible = true;
    },
    //提交表单,需要重写
    sureFormSubmit() {
      updateBMUInfo(this.bmuId, this.bmuForm).then((res) => {
        this.$message({
          message: 'BMU信息修改成功',
          type: 'success',
          duration: 3 * 1000
        })
        this.bmuFormVisible = false;
        this.getBMUInfo()
      })
    },
    sureCreateSubmit() {
      addBMU(this.bmuCreate).then((res) => {
        this.$message({
          message: 'BMU创建成功',
          type: 'success',
          duration: 3 * 1000
        })
        this.bmuCreateVisible = false;
        this.getBMUInfo()
      })
    },

    //返回上一页
    goBack() {
      this.$router.back();
    },
    goTable(type){
      if(type==='BMUVoltage'){
        this.$router.push({
          name: 'table',
          query: {
            device: 'BMU',
            deviceId: this.bmuId,
            valueType: 'BMUVoltage',
          }
        })
      }else if(type==='BMUSOC'){
        this.$router.push({
          name: 'table',
          query: {
            device: 'BMU',
            deviceId: this.bmuId,
            valueType: 'BMUSOC',
          }
        })
      }else if(type==='BMUCurrent'){
        this.$router.push({
          name: 'table',
          query: {
            device: 'BMU',
            deviceId: this.bmuId,
            valueType: 'BMUCurrent',
          }
        })
      }else if(type==='BMUSmoke'){
        this.$router.push({
          name: 'table',
          query: {
            device: 'BMU',
            deviceId: this.bmuId,
            valueType: 'BMUSmoke',
          }
        })
      }else if(type==='BMUTemperature'){
        this.$router.push({
          name: 'table',
          query:{
            device: 'BMU',
            deviceId: this.bmuId,
            valueType: 'BMUTemperature',
          }
        })
      }
    },
    getData() {
      getBMUDataStat(this.bmuId).then((res) => {
        this.bmuDataStatOption = BMUDataStatOption(res.data);
      });

      getBMUWarningStat(this.bmuId).then((res) => {
        this.warningStatOption = BMUWarningStatOption(res.data);
      });

      getBMUWarningCalendar(this.bmuId).then((res) => {
        this.warningCalendarOption = BMUWarningCalendarOption(res.data);
      });

      getBMUSOCStat(this.bmuId).then((res) => {
        this.socStatOption = BMUSOCStatOption(res.data);
      });

      getBMUOutlineStat(this.bmuId).then((res) => {
        this.outlineStatOption = BMUOutlineStatOption(res.data);
      })

      getSingleTable(this.bmuId).then((res) => {
        if(res.data == null){
          res.data = []
        }
        this.singleTable = res.data;
      });
    },
    getBMUInfo(){
      getBMUInfos().then((res) => {
        if(res.data.bmus.length>0){
          this.bmuInfos = res.data.bmus
          if(this.bmuId==0){
            this.bmuId = this.bmuInfos[0].id
          }

          this.bmuOptions = []
          for(var i=0;i<this.bmuInfos.length;i++){
            if(this.bmuInfos[i].id==this.bmuId){
              this.index=i
            }

            this.bmuOptions.push({
              value: this.bmuInfos[i].id,
              label: this.bmuInfos[i].name,
            })
          }
          
          this.bmuForm.name = this.bmuInfos[this.index].name
          this.bmuForm.bcuId = this.bmuInfos[this.index].bcuId+""
          this.bmuForm.batteryNum = this.bmuInfos[this.index].batteryNum+""
          this.bmuForm.batteryType = this.bmuInfos[this.index].batteryType+""
          this.bmuCreate.bcuId = this.bmuInfos[this.index].bcuId+""

          this.bmuVoltage[0] = this.bmuInfos[this.index].voltage.max
          this.bmuVoltage[1] = this.bmuInfos[this.index].voltage.avg
          this.bmuVoltage[2] = this.bmuInfos[this.index].voltage.min
          this.bmuSoc[0] = this.bmuInfos[this.index].soc.max
          this.bmuSoc[1] = this.bmuInfos[this.index].soc.avg
          this.bmuSoc[2] = this.bmuInfos[this.index].soc.min
          this.bmuSmoke[0] = this.bmuInfos[this.index].smoke.max
          this.bmuSmoke[1] = this.bmuInfos[this.index].smoke.avg
          this.bmuSmoke[2] = this.bmuInfos[this.index].smoke.min
          this.bmuTemperature[0] = this.bmuInfos[this.index].temperature.max
          this.bmuTemperature[1] = this.bmuInfos[this.index].temperature.avg
           this.bmuTemperature[2] = this.bmuInfos[this.index].temperature.min
        }
        if(this.bmuId!=0){
          this.getData()
        }
      });
    },
    selectBMU() {
      for(var i=0;i<this.bmuInfos.length;i++){
        if(this.bmuId==this.bmuInfos[i].id){
          this.index=i
        }
      }

      this.bmuForm.name = this.bmuInfos[this.index].name
      this.bmuForm.bcuId = this.bmuInfos[this.index].bcuId+""
      this.bmuForm.batteryNum = this.bmuInfos[this.index].batteryNum+""
      this.bmuForm.batteryType = this.bmuInfos[this.index].batteryType+""
      this.bmuCreate.bcuId = this.bmuInfos[this.index].bcuId+""
      
      this.bmuVoltage[0] = this.bmuInfos[this.index].voltage.max
      this.bmuVoltage[1] = this.bmuInfos[this.index].voltage.avg
      this.bmuVoltage[2] = this.bmuInfos[this.index].voltage.min
      this.bmuSoc[0] = this.bmuInfos[this.index].soc.max
      this.bmuSoc[1] = this.bmuInfos[this.index].soc.avg
      this.bmuSoc[2] = this.bmuInfos[this.index].soc.min
      this.bmuSmoke[0] = this.bmuInfos[this.index].smoke.max
      this.bmuSmoke[1] = this.bmuInfos[this.index].smoke.avg
      this.bmuSmoke[2] = this.bmuInfos[this.index].smoke.min
      this.bmuTemperature[0] = this.bmuInfos[this.index].temperature.max
      this.bmuTemperature[1] = this.bmuInfos[this.index].temperature.avg
      this.bmuTemperature[2] = this.bmuInfos[this.index].temperature.min
      if(this.bmuId!=0){
        this.getData();
      }
    },
    getStatusText(status) {
      if (status === 1) {
        return "正常运行";
      } else if (status === 2) {
        return "产生异常,请尽快处理";
      } else if (status === 3) {
        return "产生严重异常，务必尽快处理"
      } else if (status === 0) {
        return "当前离线";
      } else {
        return "--";
      }
    },
    getBatteryType(batteryType) {
      switch (batteryType) {
        case Battery_Ni_Cd_code:
          return Battery_Ni_Cd_text;
        case Battery_Ni_MH_code:
          return Battery_Ni_MH_text;
        case Battery_LI_lon_code:
          return Battery_LI_lon_text;
        case Battery_LI_pol_code:
          return Battery_LI_pol_text;
        case Battery_Le_ac_code:
          return Battery_Le_ac_text;
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
.bmu-container {
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

.profile-top {
  display: flex;
  align-items: center;
  .profile-title {
    font-size: 18px;
  }
  .profile-select {
    height: inherit;
    width: 200px;
    flex: 1;
    justify-content: flex-end;
    display: flex;
  }
}

.profile-head {
  .head-bg {
    margin: 30px 0;
    height: 100px;
    border-radius: 15px;
    background-image: url("../../assets/profile-bg-yellow.jpg");
    background-size: auto 100%;
    background-position: center;
    .float-right {
      margin: 10px;
      padding: 10px;
      background: white;
      border-radius: 10px;
      color: #ff6e5c;
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
    .bmuname {
      font-weight: bold;
      margin-top: 20px;
      margin-bottom: 30px;
      font-size: 25px;
      color: #ebac26;
    }
  }
}

.profile-blank {
  margin: 55px;
}
.profile-info {
  text-align: center;
  margin: 25px 0;
  font: 14px;
  .status-wrapper {
    margin: 8px;
    font-size: 18px;
  }
  .normal {
    color: #00abf3;
    font-weight: bold;
  }
  .warning {
    color: #ebac26;
    font-weight: bold;
  }
  .error {
    color: #f96e5b;
    font-weight: bold;
  }
  .offline {
    color: gray;
    font-weight: bold;
  }
  .description-wrapper {
    margin-top: 5px;
    font-size: 17px;
  }
}

.profile-numbers-container{
  margin-top: 25px;
  padding-bottom: 7px;
  .profile-numbers-title{
    font-weight: bold;
    text-align: left;
    .profile-numbers-star{
      color: #ff6e5c;
      font-size: 22px;
      position: relative;
      top: 2px;
    }
  }
  .profile-numbers {
    margin: 10px 0;
    display: flex;
    align-items: center;
    justify-content: space-evenly;
    .profile-number {
      padding: 0 15px;
      
      &:first-child {
        border: none;
      }

      .icon {
        display: inline-block;
        padding: 10px 11px;
        background: #f8d17d;
        color: #f5aa1f;
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
  .profile-number-time {
    float: right;
    text-align: right;
    font-size: 13px;
  }
}

.divder {
  background: #c3e9fe;
  height: 5px;
  width: 100%;
}
.single-table-wrapper {
  margin-top: 15px;
  background: white;
  padding: 15px;
  border-radius: 15px;
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
    width: 250px;
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
