<template>
  <div class="bcu-container">
    <div>
      <el-dialog title="添加BCU" :visible.sync="bcuCreateVisible" width="35%">
        <el-form :model="bcuCreate" ref="stationForm" label-width="100px" >
          <el-form-item label="BCU的ID">
            <el-input v-model="bcuCreate.id" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
          <el-form-item label="名称">
            <el-input v-model="bcuCreate.name" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
          <el-form-item label="所属储能站">
            <el-input v-model="bcuCreate.stationId" auto-complete="off" style="width:350px"></el-input>
            <!-- <el-select v-model="bcuForm.stationId" placeholder="请选择" style="width:300px">
              <el-option
                v-for="item in selectOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
              >
              </el-option>
            </el-select> -->
          </el-form-item>
        </el-form>
        <div slot="footer" class="dialog-footer">
          <el-button type="primary" @click="sureCreateSubmit">提 交</el-button>
        </div>
      </el-dialog>
    </div>

    <!-- 修改储能站信息弹框 -->
    <div>
      <el-dialog title="更新BCU信息" :visible.sync="bcuFormVisible" width="35%">
        <el-form :model="bcuForm" ref="stationForm" label-width="100px" >
          <el-form-item label="名称">
            <el-input v-model="bcuForm.name" auto-complete="off" style="width:350px"></el-input>
          </el-form-item>
          <el-form-item label="所属储能站">
            <el-input v-model="bcuForm.stationId" auto-complete="off" style="width:350px"></el-input>
            <!-- <el-select v-model="bcuForm.stationId" placeholder="请选择" style="width:300px">
              <el-option
                v-for="item in selectOptions"
                :key="item.value"
                :label="item.label"
                :value="item.value"
              >
              </el-option>
            </el-select> -->
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
        <Chart Id="bcu-status" :option="bcuStatOption" />
      </div>
      <div class="charts-wrapper2 flex-container">
        <Chart Id="warning-status" :option="warningStatOption" class="warning-wrapper"/>
        <div class="state-wrapper">
          <div class="state-wrapper-title">系统状态汇总</div>
          <div class="state-cols">
            <div class="state-col" style="width:45%">
              <Loader2 :value="parseSystemOnlineState(systemStat.online)" color="#26C5F3" name="在线状态" />
              <Loader2 :value="parseSystemChargeState(systemStat.charge)" color="#FAC348" name="充放电状态" />
              <Loader2 :value="parseSystemCCState(systemStat.cc)" color="#E55B5A" name="枪口状态" />
              <Loader2 :value="parseSystemHeatState(systemStat.heat)" color="#91CC75" name="加热制冷" />
            </div>
            <div class="state-col" style="width:55%">
              <Loader2 :value="parseSystemSystemState(systemStat.system)" color="#DA0D68" name="系统运行" />
              <Loader2 :value="parseSystemInteriorState(systemStat.interior)" color="#5470C6" name="内部通信" />
              <Loader2 :value="systemStat.voltage" color="#9A60B4" name="充电输入电压" />
              <Loader2 :value="systemStat.current" color="#ff8452" name="充电输入电流" />
            </div>
          </div>
        </div>
        <div class="single-wrapper">
          <div class="single-wrapper-title">单体数据汇总</div>
          <div class="single-cols">
            <div class="single-col" style="width: 50%">
              <Loader :value="singleStat.voltage.avg" color="#26C5F3" name="平均单体电压" :rank="2" />
              <Loader :value="singleStat.voltage.max" color="#FAC348" name="最高单体电压" :rank="4" />
              <Loader :value="singleStat.voltage.min" color="#E55B5A" name="最低单体电压" :rank="3" />
              <Loader :value="singleStat.voltage.dif" color="#000000" name="最高单体压差" :rank="1" />
            </div>
            <div class="single-col" style="width: 50%">
              <Loader :value="singleStat.temperature.avg" color="#26C5F3" name="平均单体温度" :rank="3"/>
              <Loader :value="singleStat.temperature.max" color="#FAC348" name="最高单体温度" :rank="4"/>
              <Loader :value="singleStat.temperature.min" color="#E55B5A" name="最低单体温度" :rank="2"/>
              <Loader :value="singleStat.temperature.dif" color="#000000" name="最高单体温差" :rank="1"/>
            </div>
          </div>
        </div>
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
            :data="slaveTable.slice((currentPage - 1) * pageSize, currentPage * pageSize)"
            @row-click="goBMU"
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
                    <div class="status gray-text">{{ scope.row.statusText }}</div>
                  </div>
                </div>
              </template>
            </el-table-column>
            <el-table-column
              prop="id"
              label="ID"
              align="center"
              width="110"
            ></el-table-column>
            <el-table-column
              prop="overview"
              label="概况"
              align="center"
              width="150"
            ></el-table-column>
            <el-table-column
              prop="statusText"
              label="运行状态"
              align="center"
              width="90"
            ></el-table-column>
            <el-table-column
              prop="onlineTime"
              label="上次上线时间"
              align="center"
              width="200"
            ></el-table-column>
            <el-table-column
              prop="warning"
              label="待处理报警"
              align="center"
              width="100"
            ></el-table-column>
            <el-table-column
              prop="socText"
              label="平均SOC"
              align="center"
              width="120"
            ></el-table-column>
                <!-- <template slot-scope="scope">
                <div class="progress" :class="{ low: scope.row.left < 50 }">
                  <div class="progress-bg">
                    <div
                      class="progress-value"
                      :style="`width: ${scope.row.left}%`"
                    ></div>
                  </div>
                </div>
              </template> -->
            
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

    <div v-if="bcuId" class="profile-container box-shadow-template">
      <div class="profile-top">
        <div class="profile-title">BCU信息</div>
        <div class="float-right profile-select">
          <el-select v-model="bcuId" placeholder="请选择" @change="selectBCU()">
            <el-option
              v-for="item in bcuOptions"
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
            <i class="el-icon-edit" @click="openBCUForm"></i>
          </div>
          <div class="float-right">
            <i class="el-icon-circle-plus-outline" style="margin-left: 0px" @click="openBCUCreate"></i>
          </div>
        </div>
        <div class="profile-avatar">
          <div class="avatar-wrapper">
            <v-gravatar
              :email="`${bcuId}@dazto.com`"
              hostname="gravatar.zeruns.tech"
            />
          </div>
          <div style="font-weight: bold">
            ID-{{ bcuId }}
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
            {{ bcuInfos[index].name }}
          </div>
          <div class="profile-info">
            <div
              class="status-wrapper"
              :class="{
                normal: bcuInfos[index].status == 1,
                warning: bcuInfos[index].status == 2,
                error: bcuInfos[index].status == 3,
                offline: bcuInfos[index].status == 0,
              }"
            >
              <div>{{ getStatusText(bcuInfos[index].status) }}---{{ getChargeText(bcuInfos[index].current) }}</div>
            </div>
            <div class="description-wrapper">
              <div>共{{ bcuInfos[index].bmuNum }}组BMU</div>
            </div>
          </div>

          <div class="current-electricity">
            <p class="gray-text">目前全部电池剩余电量占比如下</p>
            <div class="electricity-items">
              <div
                class="electrcity-item active"
                v-for="item in bcuInfos[index].soc"
                :key="item"
              ></div>
              <div
                class="electrcity-item"
                v-for="item in 10 - bcuInfos[index].soc"
                :key="10 + item"
              ></div>
            </div>
          </div>
          <!-- <div class="profile-blank"></div> -->

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              电压监测(V)
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BCUVoltage')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number" v-for="item in 3" :key="item">
                <div class="icon">
                  <i :class="icons[item - 1]" />
                </div>
                <div class="value">
                  {{ bcuVoltage[item - 1] }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[item - 1] }}
                </div>
              </div>
            </div>
          </div>

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              整体电流(A)
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BCUSOC')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number">
                <div class="icon">
                  <i :class="icons[3]" />
                </div>
                <div class="value">
                  {{ bcuInfos[index].current }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[3] }}
                </div>
              </div>
            </div>
          </div>

          <div class="profile-numbers-container">
            <div class="profile-numbers-title">
              <i class="el-icon-star-on profile-numbers-star"/>
              绝缘监测(ohm/V)
              <div class="float-right">
                <i class="el-icon-d-arrow-right" @click="goTable('BCUInsulation')"></i>
              </div>
            </div>
            <div class="profile-numbers">
              <div class="profile-number" v-for="item in 2" :key="item">
                <div class="icon">
                  <i :class="icons[3]" />
                </div>
                <div class="value">
                  {{ bcuInsulation[item - 1] }}
                </div>
                <div class="name gray-text">
                  {{ iconNames[item + 3] }}
                </div>
              </div>
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
import Loader from "@/components/BCU/loader.vue";
import Loader2 from "@/components/BCU/loader2.vue"

import {
  BCUDataStatOption,
  BCUWarningStatOption,
} from "@/utils/chartOpt";
import {
  getBCUDataStat,
  getBCUSystemStat,
  getBCUWarningStat,
  getBCUSingleStat,
  getBCUInfos,
  updateBCUInfo,
  addBCU
} from "@/api/bcu";
import { getBMUInfos } from "@/api/bmu"
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
  Battery_Le_ac_text,
  BCU_System_Online_Online_code,
  BCU_System_Online_Offline_code,
  BCU_System_Charge_No_code,
  BCU_System_Charge_Charge_code,
  BCU_System_Charge_Discharge_code,
  BCU_System_CC_No_code,
  BCU_System_CC_Use_code,
  BCU_System_CC_Connect_code,
  BCU_System_Heat_No_code,
  BCU_System_Heat_Hot_code,
  BCU_System_Heat_Cold_code,
  BCU_System_System_Stop_code,
  BCU_System_System_Work_code,
  BCU_System_Interior_Error_code,
  BCU_System_Interior_Work_code,
  BCU_System_Online_Online_text,
  BCU_System_Online_Offline_text,
  BCU_System_Charge_No_text,
  BCU_System_Charge_Charge_text,
  BCU_System_Charge_Discharge_text,
  BCU_System_CC_No_text,
  BCU_System_CC_Use_text,
  BCU_System_CC_Connect_text,
  BCU_System_Heat_No_text,
  BCU_System_Heat_Hot_text,
  BCU_System_Heat_Cold_text,
  BCU_System_System_Stop_text,
  BCU_System_System_Work_text,
  BCU_System_Interior_Error_text,
  BCU_System_Interior_Work_text
} from "@/utils/code"
export default {
  props: {
    propBCUId: {
      type: Number,
      default: () => {
        return 0;
      },
    },
    propBCUInfos: {
      type: Array,
      default: () => {
        return [{
          id: 0,
          stationId: 0,
          name: '默认BCU',
          status: 0,
          bmuNum: 0,
          soc: 0,
          b: 0,
          hv1: 0,
          hv2: 0,
          current: 0,
          positive: 0,
          negative: 0,
          order: 0,
          charge: 0,
          warning: 0,
          onlineTime: 0
        }];
      }
    },

  },
  name: "BCU",
  components: { Chart, Loader, Loader2},
  computed: {
    ...mapGetters(["name"]),
  },
  data() {
    return {
      bcuId: this.propBCUId,
      bcuInfos: this.propBCUInfos,
      icons: ["el-icon-download", "el-icon-fork-spoon", "el-icon-upload2", "el-icon-upload2", "el-icon-upload2", "el-icon-upload2", "el-icon-upload2"],
      iconNames: ["B+", "HV1", "HV2", "Current", "Positive", "Negative", "轻微", "严重"],
      socColors: [
        {color: '#f96e5b', percentage: 15},
        {color: '#ebac26', percentage: 40},
        {color: '#00abf3', percentage: 100}
      ],
      bcuVoltage: [],
      bcuInsulation: [],
      bcuStatOption: {},
      warningStatOption: {},
      systemStat: {},
      singleStat: {},
      slaveTable: [],
      searchContent: "",

      index: 0,
      bcuOptions: [],

      bcuFormVisible: false, //表单是否可见
      bcuForm: {
        name: "",
        stationId: 0,
      },
      bcuCreateVisible: false,
      bcuCreate: {
        id: 0,
        name: "默认BMU",
        stationId: 0,
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
      pageSize: 3, //每页的数据条数
      currentPage: 1, //默认开始页面
    };
  },
  mounted() {
    this.bcuOptions = []
    for(var i=0;i<this.bcuInfos.length;i++){
      if(this.bcuInfos[i].id==this.bcuId){
        this.index = i;
      }
      this.bcuOptions.push({
        value: this.bcuInfos[i].id,
        label: this.bcuInfos[i].name,
        key: this.bcuInfos[i].id+""
      })
    }
    this.bcuVoltage[0] = this.bcuInfos[this.index].b
    this.bcuVoltage[1] = this.bcuInfos[this.index].hv1
    this.bcuVoltage[2] = this.bcuInfos[this.index].hv2
    this.bcuInsulation[0] = this.bcuInfos[this.index].positive
    this.bcuInsulation[1] = this.bcuInfos[this.index].negative
    if(this.bcuId==0){
      this.getBCUInfo();
    } else {
      this.getData();
    }
  },
  watch: {
    '$route.params': {
	    handler (newVal) {
        if(this.$route.params.bcuId!=null){
          this.bcuId = this.$route.params.bcuId
          this.getBCUInfo();
        }
	    },
	    deep: true,
	    immediate: true,
	  }
  },
  methods: {
    getData() {
      getBCUDataStat(this.bcuId).then((res) => {
        this.bcuStatOption = BCUDataStatOption(res.data);
      });

      getBCUSystemStat(this.bcuId).then((res) => {
        this.systemStat = res.data
        this.systemStat.voltage += ""
        this.systemStat.current += ""
      });

      getBCUWarningStat(this.bcuId).then((res) => {
        this.warningStatOption = BCUWarningStatOption(res.data);
      });

      getBCUSingleStat(this.bcuId).then((res) => {
        this.singleStat = res.data;
        this.singleStat.voltage["dif"] = this.singleStat.voltage.max - this.singleStat.voltage.min
        this.singleStat.temperature["dif"] = this.singleStat.temperature.max - this.singleStat.temperature.min
      });

      this.slaveTable = []
      for(var i=0;i<this.bmuInfos.length;i++){
        if(this.bmuInfos[i].bcuId==this.bcuId){
          this.slaveTable.push(this.bmuInfos[i])
        }
      }
      console.log("!!!")
      console.log(this.slaveTable)
      for(var i=0;i<this.slaveTable.length;i++){
        this.slaveTable[i].overview = this.slaveTable[i].batteryNum+"串"+this.parseBatteryType(this.slaveTable[i].batteryType)+"电池"
        this.slaveTable[i].statusText = this.parseStatus(this.slaveTable[i].status)
        this.slaveTable[i].socText = this.slaveTable[i].soc.avg+"%"
      }
    },
    getBCUInfo(){
      getBCUInfos().then((res) => {
        if(res.data.bcus.length>0){
          this.bcuInfos = res.data.bcus
          if(this.bcuId==0){
            this.bcuId = this.bcuInfos[0].id
          }

          this.bcuOptions = []
          for(var i=0;i<this.bcuInfos.length;i++){
            if(this.bcuInfos[i].id==this.bcuId){
              this.index=i
            }

            this.bcuInfos[i].soc = parseInt(this.bcuInfos[i].soc/10)

            this.bcuOptions.push({
              value: this.bcuInfos[i].id,
              label: this.bcuInfos[i].name,
            })
          }

          this.bcuForm.name = this.bcuInfos[this.index].name
          this.bcuForm.stationId = this.bcuInfos[this.index].stationId+""
          this.bcuCreate.stationId = this.bcuInfos[this.index].stationId+""

          this.bcuVoltage[0] = this.bcuInfos[this.index].b
          this.bcuVoltage[1] = this.bcuInfos[this.index].hv1
          this.bcuVoltage[2] = this.bcuInfos[this.index].hv2
          this.bcuInsulation[0] = this.bcuInfos[this.index].positive
          this.bcuInsulation[1] = this.bcuInfos[this.index].negative
        }
        if(this.bcuId!=0){
          this.getData()
        }
      });
      getBMUInfos().then((res) => {
        this.bmuInfos = res.data.bmus
        this.slaveTable = []
        for(var i=0;i<this.bmuInfos.length;i++){
          if(this.bmuInfos[i].bcuId==this.bcuId){
            this.slaveTable.push(this.bmuInfos[i])
          }
        }
        for(var i=0;i<this.slaveTable.length;i++){
          this.slaveTable[i].overview = this.slaveTable[i].batteryNum+"串"+this.parseBatteryType(res.data[i].batteryType)+"电池"
          this.slaveTable[i].statusText = this.parseStatus(this.slaveTable[i].status)
          this.slaveTable[i].socText = this.slaveTable[i].soc.avg+"%"
        }
      });
    },
    selectBCU() {
      for(var i=0;i<this.bcuInfos.length;i++){
        if(this.bcuId==this.bcuInfos[i].id){
          this.index=i
        }
      }

      this.bcuForm.name = this.bcuInfos[this.index].name
      this.bcuForm.stationId = this.bcuInfos[this.index].stationId+""
      this.bcuCreate.stationId = this.bcuInfos[this.index].stationId+""
      
      this.bcuVoltage[0] = this.bcuInfos[this.index].b
      this.bcuVoltage[1] = this.bcuInfos[this.index].hv1
      this.bcuVoltage[2] = this.bcuInfos[this.index].hv2
      this.bcuInsulation[0] = this.bcuInfos[this.index].positive
      this.bcuInsulation[1] = this.bcuInfos[this.index].negative
      if(this.bcuId!=0){
        this.getData();
      }
    },
    goBMU(row){
      this.$router.push({
        name: 'BMU',
        params: {
          bmuId: row.id
        }
      })
    },
    goTable(type){
      if(type==='BCUVoltage'){
        this.$router.push({
          name: 'table',
          query: {
            device: 'BCU',
            deviceId: this.bcuId,
            valueType: 'BCUVoltage',
          }
        })
      }else if(type==='BCUSOC'){
        this.$router.push({
          name: 'table',
          query: {
            device: 'BCU',
            deviceId: this.bcuId,
            valueType: 'BCUSOC',
          }
        })
      }else if(type==='BCUInsulation'){
        this.$router.push({
          name: 'table',
          query: {
            device: 'BCU',
            deviceId: this.bcuId,
            valueType: 'BCUInsulation',
          }
        })
      }
    },
    parseBatteryType(batteryType){
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
    parseStatus(status){
      switch(status){
        case 0:
          return "当前离线"
        case 1:
          return "正常运行"
        case 2:
          return "轻微异常"
        case 3:
          return "严重异常"
      }
    },
    parseSystemOnlineState(code){
      switch(code){
        case BCU_System_Online_Online_code:
          return BCU_System_Online_Online_text;
        case  BCU_System_Online_Offline_code:
          return BCU_System_Online_Offline_text;
      }
    },
    parseSystemChargeState(code){
      switch(code){
        case BCU_System_Charge_No_code:
          return BCU_System_Charge_No_text;
        case BCU_System_Charge_Charge_code:
          return BCU_System_Charge_Charge_text;
          case BCU_System_Charge_Discharge_code:
          return BCU_System_Charge_Discharge_text;
      }
    },
    parseSystemCCState(code){
      switch(code){
        case BCU_System_CC_No_code:
          return BCU_System_CC_No_text;
        case BCU_System_CC_Use_code:
          return BCU_System_CC_Use_text;
        case BCU_System_CC_Connect_code:
          return BCU_System_CC_Connect_text;
      }
    },
    parseSystemHeatState(code){
      switch(code){
        case BCU_System_Heat_No_code:
          return BCU_System_Heat_No_text;
        case BCU_System_Heat_Hot_code:
          return BCU_System_Heat_Hot_text;
        case BCU_System_Heat_Cold_code:
          return BCU_System_Heat_Cold_text;
      }
    },
    parseSystemSystemState(code){
      switch(code){
        case BCU_System_System_Stop_code:
          return BCU_System_System_Stop_text;
        case BCU_System_System_Work_code:
          return BCU_System_System_Work_text;
      }
    },
    parseSystemInteriorState(code){
      switch(code){
        case BCU_System_Interior_Error_code:
          return BCU_System_Interior_Error_text;
        case BCU_System_Interior_Work_code:
          return BCU_System_Interior_Work_text;
      }
    },
    getStatusText(status) {
      if (status === 1) {
        return "正常运行";
      } else if (status === 2) {
        return "产生异常,需要尽快处理";
      } else if (status === 3){
        return "产生严重异常，务必尽快处理"
      } else if (status === 0){
        return "当前离线";
      }
    },
    getChargeText(current) {
      if (current == 0) {
        return "不进行充放电";
      } else if (current > 0) {
        return "正在充电";
      } else if (current < 0) {
        return "正在放电";
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
    // 打开对话框
    openBCUForm() {
      this.bcuFormVisible = true;
    },
    openBCUCreate() {
      this.bcuCreateVisible = true;
    },
    //提交表单,需要重写
    sureFormSubmit() {
      updateBCUInfo(this.bcuId, this.bcuForm).then((res) => {
        this.$message({
          message: 'BCU信息修改成功',
          type: 'success',
          duration: 3 * 1000
        })
        this.bcuFormVisible = false;
        this.getBCUInfo()
      })
      // let _this = this;
      // let api = "";
      // _this.axios.post(api, {}).then((res) => {
      //   console.log(res);
      // });
    },
    sureCreateSubmit() {
      addBCU(this.bcuCreate).then((res) => {
        this.$message({
          message: 'BCU创建成功',
          type: 'success',
          duration: 3 *1000
        })
        this.bcuCreateVisible = false;
        this.getBCUInfo();
      })
    },
    //返回上一页
    goBack(){
      this.$router.back();
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
.state-wrapper {
  width: 32.5%;
  padding: 10px 0px;
  border-radius: 15px;
  background: white;
  .state-wrapper-title {
    font-weight: bold;
    font-size: 18px;
    padding-top: 6px;
    padding-left: 35px;
    color: #343434;
  }
  .state-cols {
    display: flex;
    .state-col {
      margin-top: 10px;
    }
  }
}

.single-wrapper {
  width: 32.5%;
  padding: 10px 0px;
  border-radius: 15px;
  background: white;
  .single-wrapper-title {
    font-weight: bold;
    font-size: 18px;
    padding-top: 6px;
    padding-left: 35px;
    color: #343434;
  }
  .single-cols {
    display: flex;
    //margin-top: 4px;
    .single-col {
      margin-top: 10px;
    }
  }
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
    margin-top: 30px;
    height: 100px;
    border-radius: 15px;
    background-image: url("../../assets/profile-bg-orange.jpg");
    background-size: auto 100%;
    background-position: center;
    .float-right {
      margin: 10px;
      padding: 10px;
      background: white;
      border-radius: 10px;
      color: #f96e5b;
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
      color: #f96e5b;
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

.current-electricity {
  margin: 35px 0;
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

.profile-numbers-container{
  margin-top: 25px;
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
        background: #fad0bf;
        color: #ff6e5c;
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
}

.profile-warnings {
  margin: 0;
  padding: 15px;
  background: #fabea4;
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
    color: #343434;
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
