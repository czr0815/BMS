<template>
  <div class="tables-container">
    <div class="inputs-container">
      <div class="select1-container">
        <el-select v-model="tableForum.device" placeholder="请选择查询设备(默认储能站" @change="selectDeviceOptions()">
          <el-option
            v-for="item in deviceOptions"
            :key="item.value"
            :label="item.label"
            :value="item.value">
          </el-option>
        </el-select>
      </div>
      <div class="input-container">
        <el-input v-model="tableForum.deviceId" placeholder="请输入设备ID(默认全选" class></el-input>
      </div>
      <div class="select2-container">
        <el-select v-model="tableForum.valueType" placeholder="请选择查询数据类型(默认全选">
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
        <el-button type="primary" plain @click="getData">点击查询</el-button>
      </div>
    </div>
    <div class="table-container">
      <el-table
        v-loading="loading"
        :data="tableData"
        style="width: 100%"
        :default-sort = "{prop: 'date', order: 'descending'}"
        :header-cell-style="{'text-align':'center'}"
      >
        <el-table-column
          prop="valueType"
          label="数据种类"
          sortable
          align="center">
        </el-table-column>
        <el-table-column
          prop="valueId"
          label="数据ID"
          sortable
          align="center">
        </el-table-column>
        <el-table-column
          prop="device"
          label="数据产生设备"
          width="180"
          align="center">
        </el-table-column>
        <el-table-column
          prop="deviceId"
          label="设备ID"
          sortable
          width="180"
          align="center">
        </el-table-column>
        <el-table-column
          prop="pointId"
          label="数据采集点ID"
          sortable
          align="center">
        </el-table-column>
        <el-table-column
          prop="value"
          label="数据值"
          sortable
          align="center">
        </el-table-column>
        <el-table-column
          prop="warning"
          label="产生报警"
          sortable
          align="center">
        </el-table-column>
        <el-table-column
          prop="collTime"
          label="采集时间"
          sortable
          align="center">
        </el-table-column>
        <el-table-column
          prop="operation"
          label="操作"
          align="center">
        </el-table-column>
      </el-table>
    </div>
  </div>
</template>

<script>

  import {
    getCollects
  } from "@/api/collect";
  export default {
    data() {
      return {
        deviceOptions: [{
          value: 'Station',
          label: '储能站',
        }, {
          value: 'BCU',
          label: 'BCU',
        }, {
          value: 'BMU',
          label: 'BMU'
        }],

        typeOptions: [{
          value: 'all',
          label: '全种类数据',
          disabled: true,
        }, {
          value: 'BCUVoltage',
          label: 'BCU总体电压',
          disabled: true,
        }, {
          value: 'BCUSOC',
          label: 'BCU总体SOC',
          disabled: true,
        }, {
          value: 'BCUInsulation',
          label: 'BCU绝缘性',
          disabled: true,
        }, {
          value: 'BMUVoltage',
          label: 'BMU单点电压',
          disabled: true,
        }, {
          value: 'BMUSOC',
          label: 'BMU单点SOC',
          disabled: true,
        }, {
          value: 'BMUCurrent',
          label: 'BMU单点电流',
          disabled: true,
        }, {
          value: 'BMUSmoke',
          label: 'BMU单点烟雾',
          disabled: true,
        }, {
          value: 'BMUTemperature',
          label: 'BMU单点温度',
          disabled: true,
        }],

        tableData: [],
        loading: false,
        tableForum: {
          device: "Station",
          deviceId: 0,
          valueType: "all",
          timeRange: [parseInt(new Date(new Date().getTime()-(3*3600*24*1000)).getTime()/1000), parseInt(new Date().getTime/1000)]
        },
        timeRange: [new Date(new Date().getTime()-(3*3600*24*1000)), new Date()]
      }
    },
    mounted() {
      this.getData();
    },
    watch: {
      '$route.query': {
	      handler (newVal) {
          this.tableForum.device = this.$route.query.device
          this.tableForum.deviceId = this.$route.query.deviceId
          this.tableForum.valueType = this.$route.query.valueType
          if(this.tableForum.device==undefined){
            this.tableForum.device="Station"
            this.tableForum.deviceId=0
            this.tableForum.valueType="all"
          }
          this.selectDeviceOptions()
	      },
	      deep: true,
	      immediate: true,
	    }
    },
    methods: {
      getData(){
        this.loading = true;
        this.tableForum.timeRange[0] = parseInt(this.timeRange[0].getTime()/1000)
        this.tableForum.timeRange[1] = parseInt(this.timeRange[1].getTime()/1000)
        if(this.tableForum.timeRange[0]>this.tableForum.timeRange[1]){
          var tmp = this.tableForum.timeRange[0]
          this.tableForum.timeRange[0] = this.tableForum.timeRange[1]
          this.tableForum.timeRange[1] = tmp
        }
        getCollects(this.tableForum).then((res) => {
          this.tableData = res.data;
        })
        this.loading = false;
      },
      selectDeviceOptions(){
        if(this.tableForum.device==='Station'){
          this.typeOptions[0].disabled=false;
          for(var i=0;i<9;i++){{
            this.typeOptions[i].disabled=true;
          }}
        } else if(this.tableForum.device==='BCU'){
          this.typeOptions[0].disabled=false;
          for(var i=1;i<4;i++){
            this.typeOptions[i].disabled=true;
          }
          for(var i=4;i<9;i++){
            this.typeOptions[i].disabled=true;
          }
        } else if(this.tableForum.device==='BMU'){
          this.typeOptions[0].disabled=false;
          for(var i=1;i<4;i++){
            this.typeOptions[i].disabled=true;
          }
          for(var i=4;i<9;i++){
            this.typeOptions[i].disabled=false;
          }
        }
      },
      clickButton(){
        alert(this.tableForum.deviceId)
      },
    }
  }
</script>
<style lang="scss" scoped>
.tables-container {
  margin: 15px 15px;
}
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
.table-container {
  width: 100%;
  padding-top: 15px;
  border-radius: 15px;
  border-color: black;
  border-width: 1px;
}
</style>