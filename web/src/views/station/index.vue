<template>
  <div class="station-container">
      <!-- <station-profile
        :stationInfo="stationInfo"
        :warningRecord="warningRecord"
      /> -->
    <div v-if="stationId" class="profile-container box-shadow-template">
      <div>
      <el-dialog
        title="修改储能站信息"
        :visible.sync="stationCreateVisible"
        width="35%"
      >
        <baidu-map :center="stationCreate.center" :zoom="zoom" @ready="createHandler" style="height:350px" @click="getCreateClickInfo" :scroll-wheel-zoom='true' ak="9alCQDxmgI288IP5kUT5rSO49YHolpsN">
          <!-- <bm-view style="width: 100%; height:100px; flex: 1"></bm-view> -->
        </baidu-map>
        <el-form :model="stationCreate" ref="stationForm" label-width="100px" style="margin-top: 25px">
          <el-form-item label="名称">
            <el-input
              v-model="stationCreate.name"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="位置">
            <el-input
              v-model="stationCreate.position"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="管理员ID">
            <el-input
              v-model="stationCreate.employeeId"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
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
        title="修改储能站信息"
        :visible.sync="stationFormVisible"
        width="35%"
      >
        <baidu-map :center="stationform.center" :zoom="zoom" @ready="modifyHandler" style="height:350px" @click="getClickInfo" :scroll-wheel-zoom='true' ak="9alCQDxmgI288IP5kUT5rSO49YHolpsN">
          <!-- <bm-view style="width: 100%; height:100px; flex: 1"></bm-view> -->
        </baidu-map>
        <el-form :model="stationform" ref="stationForm" label-width="100px" style="margin-top: 25px">
          <el-form-item label="名称">
            <el-input
              v-model="stationform.name"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="位置">
            <el-input
              v-model="stationform.position"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="管理员ID">
            <el-input
              v-model="stationform.employeeId"
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

    <!-- 储能站信息内容 -->
    <div class="profile-top">
      <div class="profile-title">储能站信息</div>
      <div class="float-right profile-select">
        <el-select v-model="stationId" placeholder="请选择" @change="selectStation()">
          <el-option
            v-for="item in stationOptions"
            :key="item.key"
            :label="item.label"
            :value="item.value">
          </el-option>
        </el-select>
      </div>
    </div>
    <div class="profile-head">
      <div class="head-bg">
        <div class="float-right">
          <i class="el-icon-edit" @click="openStationForm"></i>
        </div>
        <div class="float-right">
          <i class="el-icon-circle-plus-outline" style="margin-left: 0px" @click="openStationCreate"></i>
        </div>
      </div>
      <div class="profile-avatar">
        <div class="avatar-wrapper">
          <v-gravatar
            :email="`${stationInfos[index].id}@dazto.com`"
            hostname="gravatar.zeruns.tech"
          />
        </div>
        <div style="font-weight: bold">
          ID-{{ stationInfos[index].id }}
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
        <div class="station-name">
          {{ stationInfos[index].name }}
        </div>

        <div class="profile-info">
          <div
            class="status-wrapper"
            :class="{
              normal: stationInfos[index].status == 1,
              warning: stationInfos[index].status == 2,
              error: stationInfos[index].status == 3,
              offline: stationInfos[index].status == 0,
            }">
            <div>{{ parseStatus(stationInfos[index].status) }}</div>
          </div>
          <div class="position-wrapper">
            <div>{{ stationInfos[index].position }}</div>
          </div>
        </div>

        <div class="current-electricity">
          <p class="gray-text">目前全部电池剩余电量占比如下</p>
          <div class="electricity-items">
            <div
              class="electrcity-item active"
              v-for="item in stationInfos[index].soc"
              :key="item"
            ></div>
            <div
              class="electrcity-item"
              v-for="item in 10 - stationInfos[index].soc"
              :key="10 + item"
            ></div>
          </div>
        </div>

        <div class="profile-numbers">
          <div class="profile-number" v-for="item in 3" :key="item">
            <div class="icon">
              <i :class="icons[item - 1]" />
            </div>
            <div class="value">
              {{ profileNumbers[item - 1] }}
            </div>
            <div class="name gray-text">
              {{ iconNames[item - 1] }}
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- 储能站信息区 -->
    </div>
    <div class="chart-table-container">
      <div class="charts-wrapper-b flex-container">
        <div class="station-map-container chart-template box-shadow-template">
          <baidu-map
            class="map"
            :center="center"
            :zoom="zoom"
            :scroll-wheel-zoom="scrollWheelZoom"
            ak="9alCQDxmgI288IP5kUT5rSO49YHolpsN">
          </baidu-map>
        </div>
        <ScatterChart Id="discharge-scatter" :option="dischargeScatterOption" />
      </div>
      <div class="lists-wrapper-m flex-container">
        <List title="触发报警排名" :values="warningRank" />
        <List title="连接次数排名" :values="orderRank" />
        <List title="放电量排名" :values="dischargeRank" />
      </div>
      <div class="slave-table-wrapper box-shadow-template">
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
            :data="slaveTable"
            @row-click="goBCU"
            ref="multipleTable"
          >
            <el-table-column label="名称" align="center" width="200">
              <template slot-scope="scope">
                <div class="name-wrapper">
                  <v-gravatar
                    :email="`${scope.row.id}@dazto.com`"
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
              width="150"
            ></el-table-column>
            <el-table-column
              prop="charge"
              label="充放电状态"
              align="center"
              width="120"
            ></el-table-column>
            <el-table-column
              prop="order"
              label="订单状态"
              align="center"
              width="120"
            ></el-table-column>
            <el-table-column
              prop="onlineTime"
              label="上次上线时间"
              align="center"
            ></el-table-column>
            <el-table-column
              prop="warning"
              label="待处理报警"
              align="center"
              width="100"
            ></el-table-column>
            <el-table-column
              prop="soc"
              label="当前电量"
              align="center"
              width="100"
            ><template slot-scope="scope">
              <div class="progress" :class="{ low: scope.row.soc < 50 }">
                <div class="progress-bg">
                  <div
                    class="progress-value"
                    :style="`width: ${scope.row.soc}%`"
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
  </div>
</template>

<script>
import Chart from "@/components/Chart";
import List from "@/components/List";
import ScatterChart from "@/components/Station/ScatterChart.vue"
import {
  StationDischargeScatterOption,
} from "@/utils/chartOpt";
import {
  getStationDischargeScatter,
  getStationInfos,
  updateStation,
  addStation
} from "@/api/station";
import {
  getBCUInfos,
  getBCURank
} from "@/api/bcu"
import BaiduMap from 'vue-baidu-map/components/map/Map.vue'
import {
  BCU_System_Charge_No_code,
  BCU_System_Charge_Charge_code,
  BCU_System_Charge_Discharge_code,
  BCU_System_Order_No_code,
  BCU_System_Order_Work_code,
  BCU_System_Charge_No_text,
  BCU_System_Charge_Charge_text,
  BCU_System_Charge_Discharge_text,
  BCU_System_Order_No_text,
  BCU_System_Order_Work_text,
} from "@/utils/code"

export default {
  props: {
    propStationId: {
      type: Number,
      default: () => {
        return 0;
      },
    },
    propStationInfos: {
      type: Array,
      default: () => {
        return [{
          createTime: '0000',
          id: 0,
          lat: 0,
          lng: 0,
          name: '默认储能站',
          ownerId: 0,
          position: "默认储能站位置",
          status: 0,
          employeeId: 0
        }];
      }
    }
  },
  name: "Station",
  components: {
    Chart,
    List,
    ScatterChart,
    BaiduMap
  },
  data() {
    return {
      stationId: this.propStationId,
      stationInfos: this.propStationInfos,
      bcuInfos: [],
      icons: ["el-icon-download", "el-icon-fork-spoon", "el-icon-upload2"],
      profileNumbers: [0, 0, 0],
      iconNames: ["累计放电量", "累计订单数", "累计充电量"],
      statusText: "当前离线",
      center: {lng:0, lat:0},
      zoom: 15,
      scrollWheelZoom: true,
      dischargeScatterOption: {},
      warningRank: [],
      orderRank: [],
      dischargeRank: [],
      slaveTable: [],
      searchContent: "",

      index: 0,
      stationOptions: [],

      //储能站弹出表单
      stationFormVisible: false, //表单是否可见
      stationform: {
        //这个表之后可以直接用下边的 stationInfo表 这里先作为示例使用
        name: "",
        position: "",
        center: {lng:110, lat:60},
        employeeId: 0,
      },
      stationCreateVisible: false,
      stationCreate: {
        name: "默认储能站",
        position: "",
        center: {lng:0, lat:0},
        employeeId: 0,
      },

      modifyBMap: '',
      modifyMap: '',
      modifyGeoc: '',
      createBMap: '',
      createMap: '',
      createGeoc: '',

      pageSize: 3, //每页的数据条数
      currentPage: 1, //默认开始页面
    };
  },
  mounted() {
    this.stationOptions = []
    for(var i=0;i<this.stationInfos.length;i++){
      if(this.stationInfos[i].id==this.stationId){
        this.index = i;
      }
      this.stationOptions.push({
        value: this.stationInfos[i].id,
        label: this.stationInfos[i].name,
        key: this.stationInfos[i].id+""
      })
    }
    this.center.lat = this.stationInfos[this.index].lat
    this.center.lng = this.stationInfos[this.index].lng
    this.profileNumbers[0] = this.stationInfos[this.index].discharge
    this.profileNumbers[1] = this.stationInfos[this.index].order
    this.profileNumbers[2] = this.stationInfos[this.index].charge
    if(this.stationId==0){
      this.getStationInfo();
    } else {
      this.getData();
    }
  },
  watch: {
    '$route.params': {
	    handler (newVal) {
        if(this.$route.params.stationId!=null){
          this.stationId = this.$route.params.stationId
          this.getStationInfo();
        }
	    },
	    deep: true,
	    immediate: true,
	  }
  },
  methods: {
    getData() {
      getStationDischargeScatter(this.stationId).then((res) => {
        this.dischargeScatterOption =  StationDischargeScatterOption(res.data);
      });

      getBCURank(this.stationId).then((res) => {
        this.warningRank = []
        for(var i=0;i<res.data.warningRank.length;i++){
          var tagType = this.parseTagType(res.data.warningRank[i].status)
          this.warningRank.push({
            content: res.data.warningRank[i].bcuName,
            addition: 'ID '+res.data.warningRank[i].bcuId,
            type: 0,
            tagType: tagType,
            tagContent: res.data.warningRank[i].count,
            email: res.data.warningRank[i].bcuId + "@dazto.com"
          })
          if(res.data.warningRank[i].bcuName.length < 1){
            this.warningRank[i].content = "unknown"
          }
        }
        
        this.orderRank = []
        for(var i=0;i<res.data.orderRank.length;i++){
          var tagType = this.parseTagType(res.data.orderRank[i].status)
          this.orderRank.push({
            content: res.data.orderRank[i].bcuName,
            addition: 'ID '+res.data.orderRank[i].bcuId,
            type: 0,
            tagType: tagType,
            tagContent: res.data.orderRank[i].count,
            email: res.data.orderRank[i].bcuId + "@dazto.com"
          })
          if(res.data.orderRank[i].bcuName.length < 1){
            this.orderRank[i].content = "unknown"
          }
        }

        this.dischargeRank = []
        for(var i=0;i<res.data.dischargeRank.length;i++){
          var tagType = this.parseTagType(res.data.dischargeRank[i].status)
          this.dischargeRank.push({
            content: res.data.dischargeRank[i].bcuName,
            addition: 'ID '+res.data.dischargeRank[i].bcuId,
            type: 0,
            tagType: tagType,
            tagContent: Number((res.data.dischargeRank[i].count).toFixed(2)),
            email: res.data.dischargeRank[i].bcuId + "@dazto.com"
          })
          if(res.data.dischargeRank[i].bcuName.length < 1){
            this.dischargeRank[i].content = "unknown"
          }
        }
      });

      this.slaveTable = []
      for(var i=0;i<this.bcuInfos.length;i++){
        if(this.bcuInfos[i].stationId==this.stationId){
          this.slaveTable.push(this.bcuInfos[i])
        }
      }
      for(var i=0;i<this.slaveTable.length;i++){
        this.slaveTable[i]["statusText"] = this.parseBCUStatus(this.slaveTable[i].status)
        this.slaveTable[i].charge = this.parseBCUChargeState(this.slaveTable[i].charge)
        this.slaveTable[i].order = this.parseBCUOrderState(this.slaveTable[i].order)
      }

      // getStationSlaveTable(this.stationId).then((res) => {
      //   //this.slaveTitle = res.title;
      //   this.slaveTable = res.data;
      //   for(var i=0;i<this.slaveTable.length;i++){
      //     this.slaveTable[i]["statusText"] = this.parseBCUStatus(this.slaveTable[i].status)
      //     this.slaveTable[i].charge = this.parseBCUChargeState(this.slaveTable[i].charge)
      //     this.slaveTable[i].order = this.parseBCUOrderState(this.slaveTable[i].order)
      //   }
      // });
    },
    getStationInfo(){
      getStationInfos().then((res) => {
        if(res.data.stations.length>0){
          this.stationInfos = res.data.stations
          if(this.stationId==0){
            this.stationId = this.stationInfos[0].id
          }

          this.stationOptions = []
          for(var i=0;i<this.stationInfos.length;i++){
            if(this.stationInfos[i].id==this.stationId){
              this.index=i
            }
            
            this.stationInfos[i].soc = parseInt(this.stationInfos[i].soc/10)
            this.stationInfos[i].discharge = (this.stationInfos[i].discharge).toFixed(2)
            this.stationInfos[i].charge = (this.stationInfos[i].charge).toFixed(2)

            this.stationOptions.push({
              value: this.stationInfos[i].id,
              label: this.stationInfos[i].name,
            })
          }

          this.stationform.name = this.stationInfos[this.index].name + ""
          this.stationform.position = this.stationInfos[this.index].position + ""
          this.stationform.center = {lng: this.stationInfos[this.index].lng, lat:this.stationInfos[this.index].lat}
          this.stationform.employeeId = this.stationInfos[this.index].employeeId

          this.stationCreate.name = "默认储能站"
          this.stationCreate.position = this.stationInfos[this.index].position + ""
          this.stationCreate.center = {lng: this.stationInfos[this.index].lng, lat:this.stationInfos[this.index].lat}
          this.stationCreate.employeeId = this.stationInfos[this.index].employeeId

          this.center = {lng: this.stationInfos[this.index].lng, lat: this.stationInfos[this.index].lat}
          this.profileNumbers[0] = this.stationInfos[this.index].discharge
          this.profileNumbers[1] = this.stationInfos[this.index].order
          this.profileNumbers[2] = this.stationInfos[this.index].charge
        }
        if(this.stationId!=0){
          this.getData()
        }
      });
      getBCUInfos().then((res) => {
        this.bcuInfos = res.data.bcus
        this.slaveTable = []
        for(var i=0;i<this.bcuInfos.length;i++){
          if(this.bcuInfos[i].stationId==this.stationId){
           this.slaveTable.push(this.bcuInfos[i])
          }
        }
        for(var i=0;i<this.slaveTable.length;i++){
          this.slaveTable[i]["statusText"] = this.parseBCUStatus(this.slaveTable[i].status)
          this.slaveTable[i].charge = this.parseBCUChargeState(this.slaveTable[i].charge)
          this.slaveTable[i].order = this.parseBCUOrderState(this.slaveTable[i].order)
        }
      })
    },
    modifyHandler({BMap, map}){
      this.modifyBMap = BMap
      this.modifyMap = map
      var geolocation = new BMap.Geolocation()
      this.modifyGeoc = new BMap.Geocoder() // 地址解析对象
      var $this = this
      // 调用百度地图api 中的获取当前位置接口
      geolocation.getCurrentPosition(function (r) {
        let myGeo = new BMap.Geocoder()
        myGeo.getLocation(new BMap.Point(r.point.lng, r.point.lat), function (result) {
          if (result) {
            console.log(result)
            map.enableScrollWheelZoom(true) // 开启鼠标滚轮缩放,默认关闭
            $this.addPoint($this.modifyMap, $this.stationform)

            $this.stationform.center.lng = result.point.lng
            $this.stationform.center.lat = result.point.lat
          }
        })
      })
    },
    createHandler({BMap, map}){
      this.createBMap = BMap
      this.createMap = map
      var geolocation = new BMap.Geolocation()
      this.createGeoc = new BMap.Geocoder() // 地址解析对象
      var $this = this
      // 调用百度地图api 中的获取当前位置接口
      geolocation.getCurrentPosition(function (r) {
        let myGeo = new BMap.Geocoder()
        myGeo.getLocation(new BMap.Point(r.point.lng, r.point.lat), function (result) {
          if (result) {
            console.log(result)
            map.enableScrollWheelZoom(true) // 开启鼠标滚轮缩放,默认关闭
            $this.addPoint($this.createMap, $this.stationCreate)

            $this.stationCreate.center.lng = result.point.lng
            $this.stationCreate.center.lat = result.point.lat
          }
        })
      })
    },
    addPoint (map, form) {
      map.clearOverlays()
      var point = new BMap.Point(form.center.lng, form.center.lat)
      let zoom = map.getZoom()
      setTimeout(() => {
        map.centerAndZoom(point, zoom)
      }, 0)
      var marker = new BMap.Marker(point) // 创建标注
      map.addOverlay(marker) // 将标注添加到地图中
    },
    getClickInfo (e) {
      console.log(e)
      this.stationform.center = {lng: e.point.lng, lat: e.point.lat}
      // 获取点位信息
      let $this = this
      this.modifyGeoc.getLocation(e.point, function (rs) {
        var addComp = rs.addressComponents
        console.log(addComp)
        $this.stationform.position = addComp.province + addComp.city + addComp.district + addComp.street + addComp.streetNumber
        $this.addPoint($this.modifyMap, $this.stationform)
      })      
    },
    getCreateClickInfo (e) {
      console.log(e)
      this.stationCreate.center = {lng: e.point.lng, lat: e.point.lat}
      // 获取点位信息
      let $this = this
      this.createGeoc.getLocation(e.point, function (rs) {
        var addComp = rs.addressComponents
        console.log(addComp)
        $this.stationCreate.position = addComp.province + addComp.city + addComp.district + addComp.street + addComp.streetNumber
        $this.addPoint($this.createMap, $this.stationCreate)
      })      
    },
    goBCU(row){
      this.$router.push({
        name: 'BCU',
        params: {
          bcuId: row.id
        }
      })
    },
    selectStation() {
      for(var i=0;i<this.stationInfos.length;i++){
        if(this.stationId==this.stationInfos[i].id){
          this.index=i
        }
      }

      this.stationform.name = this.stationInfos[this.index].name + ""
      this.stationform.position = this.stationInfos[this.index].position + ""
      this.stationform.center = {lng: this.stationInfos[this.index].lng, lat:this.stationInfos[this.index].lat}
      this.modifyMap.clearOverlays()
      setTimeout(() => {
        this.modifyMap.centerAndZoom(new this.modifyBMap.Point(this.stationform.center.lng, this.stationform.center.lat), this.modifyMap.getZoom())
      }, 0)
      this.modifyMap.addOverlay(new this.modifyBMap.Point(this.stationform.center.lng, this.stationform.center.lat))
      this.stationform.employeeId = this.stationInfos[this.index].employeeId

      this.stationCreate.position = this.stationInfos[this.index].position + ""
      this.stationCreate.center = {lng: this.stationInfos[this.index].lng, lat:this.stationInfos[this.index].lat}
      this.createMap.clearOverlays()
      setTimeout(() => {
        this.createMap.centerAndZoom(new this.createBMap.Point(this.stationCreate.center.lng, this.stationCreate.center.lat), this.createMap.getZoom())
      }, 0)
      this.createMap.addOverlay(new this.createBMap.Point(this.stationCreate.center.lng, this.stationCreate.center.lat))
      this.stationCreate.employeeId = this.stationInfos[this.index].employeeId
      
      this.center = {lng: this.stationInfos[this.index].lng, lat: this.stationInfos[this.index].lat}
      this.profileNumbers[0] = this.stationInfos[this.index].discharge
      this.profileNumbers[1] = this.stationInfos[this.index].order
      this.profileNumbers[2] = this.stationInfos[this.index].charge
      if(this.stationId!=0){
        this.getData();
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
    parseTagType(status){
      switch(status){
        case 0:
          return "info"
        case 1:
          return "success"
        case 2:
          return "warning"
        case 3:
          return "danger"
      }
    },
    parseBCUStatus(status){
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
    parseBCUChargeState(code){
      switch(code){
        case BCU_System_Charge_No_code:
          return BCU_System_Charge_No_text;
        case BCU_System_Charge_Charge_code:
          return BCU_System_Charge_Charge_text;
          case BCU_System_Charge_Discharge_code:
          return BCU_System_Charge_Discharge_text;
      }
    },
    parseBCUOrderState(code) {
      switch(code){
        case BCU_System_Order_No_code:
          return BCU_System_Order_No_text;
        case BCU_System_Order_Work_code:
          return BCU_System_Order_Work_text;
      }
    },
    openStationForm() {
      this.stationFormVisible = true;
    },
    openStationCreate() {
      this.stationCreateVisible = true;
    },

    //提交表单,需要重写
    sureFormSubmit() {
      updateStation(this.stationId, this.stationform).then((res) => {
        this.$message({
          message: '储能站信息修改成功',
          type: 'success',
          duration: 3 * 1000
        })
        this.stationFormVisible = false;
        this.getStationInfo()
      })
    },
    sureCreateSubmit() {
      addStation(this.stationCreate).then((res) => {
        this.$message({
          message: '储能站创建成功',
          type: 'success',
          duration: 3 * 1000
        })
        this.stationCreateVisible = false;
        this.getStationInfo()
      })
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
.station-container {
  display: flex;
  margin: 15px 15px;
}

.profile-container {
  margin-top: 0px;
  margin-right: 15px;
  vertical-align: top;
  display: inline-block;
  width: 370px;
  background: white;
  border-radius: 15px;
  padding: 15px;
}
.profile-top{
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
    .station-name {
      font-weight: bold;
      margin-top: 25px;
      font-size: 22px;
      color: #00abf3;
    }
  }
}

.profile-info {
  text-align: center;
  margin: 30px 0;
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
  .position-wrapper {
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

.profile-numbers {
  margin: 50px 0 0 0;
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
      background: #c3e9fe;
      color: #00abf3;
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
  background: #c3e9fe;
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

.chart-table-container {
  width: calc(100% - 385px);
  display: inline-block;
  vertical-align: top;
  .station-map-container {
    width: 59%;
    height: 300px;
    .map {
      width: 100%;
      height: 100%;
    }
  }
  .chart-template {
    height: 300px;
  }
}

.flex-container {
  display: flex;
  width: 100%;
  justify-content: space-between;
}

.charts-wrapper-b {
  width: 100%;
}
.charts-wrapper-m .chart-template {
  width: 49.5%;
}
.charts-wrapper-s {
  width: 39%;
}
.lists-wrapper-m .list-template {
  width: 33%;
  margin-top: 15px;
}
.lists-wrapper-s {
  width: 29.5%;
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

.profile-container {
  display: flex;
  flex-direction: column;
}
</style>
