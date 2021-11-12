<template>
  <div v-if="stationInfo.name" class="profile-container box-shadow-template">
    <!-- 修改储能站信息弹框 -->
    <div>
      <el-dialog
        :title="stationformTitle"
        :visible.sync="stationFormVisible"
        width="35%"
      >
        <el-form :model="stationform" ref="stationForm" label-width="100px">
          <el-form-item label="名称">
            <el-input
              v-model="stationform.name"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="经度">
            <el-input
              v-model="stationform.longitude"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="纬度">
            <el-input
              v-model="stationform.latitude"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="从属用户">
            <el-input
              v-model="stationform.user"
              auto-complete="off"
              style="width: 300px"
            ></el-input>
          </el-form-item>
          <el-form-item label="运行状态">
            <el-select
              v-model="value"
              placeholder="请选择"
              style="width: 300px"
            >
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
    <!-- 储能站信息内容 -->
    <div class="profile-top">
      储能站信息
    </div>
    <div class="profile-head">
      <div class="head-bg">
        <div class="float-right">
            <i class="el-icon-edit" @click="openStationForm"></i>
          </div>
      </div>
      <div class="profile-avatar">
        <div class="avatar-wrapper">
          <v-gravatar :email="`${stationInfo.name}@dazto.com`" hostname="gravatar.zeruns.tech" />
        </div>
        <div style="font-weight: bold">
          {{ stationInfo.name }}
          <i class="el-icon-star-on" style="color: #FF6E5C; font-size:22px; position:relative; top: 2px" />
        </div>

        <div class="profile-info">
          <div class="status-wrapper" :class="{sight: stationInfo.status==1, medium: stationInfo.status==2, serious: stationInfo.status==-1}">
            <div>{{ getStatusText() }}</div>
          </div>
          <div class="position-wrapper">
            <div>{{ stationInfo.position }}</div>
          </div>
        </div>

        <div class="current-electricity">
          <p class="gray-text">目前全部电池剩余电量占比如下</p>
          <div class="electricity-items">
            <div class="electrcity-item active" v-for="item in stationInfo.left" :key="item"></div>
            <div class="electrcity-item" v-for="item in (10 - stationInfo.left)" :key="10 + item"></div>
          </div>
        </div>

        <!-- <div class="profile-numbers">
          <div class="profile-number" v-for="item in 3" :key="item">
            <div class="icon">
              <i :class="icons[item-1]" />
            </div>
            <div class="value">
              {{ stationInfo.number[item-1] }}
            </div>
            <div class="name gray-text">
              {{ iconNames[item-1] }}
            </div>
          </div>
        </div> -->
      </div>

      <!-- <div class="profile-warnings">
        <div class="warning-title">近期报警记录</div>
        <div class="warning-box">
          <div class="warning-item float-shadow" v-for="(item, idx) in warningRecord" :key="idx" >
            <i :class="icons[2]" />
            <div style="margin-left: 8px;" :class="{sight: item.level==1, medium:item.level==2, serious:item.level==3}">{{ item.warn }}</div>
            <div class="gray-text loc-right">{{ item.uploadTime }}</div>
          </div>
        </div>
      </div> -->
    </div>
  </div>
</template>

<script>
export default {
  props: {
    stationInfo: {
      type: Object,
      default: () => { return {} }
    },
    warningRecord: {
      type: Array,
      default: () => { return [] }
    }
  },
  data() {
    return {
      //储能站弹出表单
      stationformTitle: "修改储能站信息", //表单标题
      stationFormVisible: false, //表单是否可见
      stationform: {
        //这个表之后可以直接用下边的 stationInfo表 这里先作为示例使用
        name: "",
        longitude: "",
        latitude: "",
        user: "",
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
      //station信息
      icons: ["el-icon-download", "el-icon-fork-spoon", "el-icon-upload2"],
      iconNames: ["累计放电量", "累计订单数", "累计充电量"],
      statusText: '当前离线'
    }
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
    
    getStatusText: function() {
      if (this.stationInfo.status === 1) {
        return '正常运行'
      } else if (this.stationInfo.status === 2) {
        return '产生较多异常'
      } else {
        return '当前离线'
      }
    }
  }
}
</script>

<style lang="scss" scoped>
.profile-container {
  margin-top: 15px;
  margin-right: 15px;
  vertical-align: top;
  display: inline-block;
  width: 370px;
  background: white;
  border-radius: 15px;
  padding: 15px;
}

.profile-head {
  height: 360px;
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
    color:#00abf3;
    font-weight: bold;
  }
  .medium {
    color: #F96E5B;
    font-weight: bold;
  }
  .serios {
    color:gray;
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
      background: #E4E5E9;
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
      background: #c3e9fe;
      color: #00abf3;
      font-size: 18px;
      border-radius: 50%;
    }

    .value {
      margin: 10px 0;
      font-size: 20px;
      font-weight: bold;
      color: #3A445F;
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
      color:#3481B8;
      font-weight: bold;
    }
    .medium {
      color: #F96E5B;
      font-weight: bold;
    }
    .serious {
      color: red;
      font-weight: bold;
    }
  }
}

.divder {
  background:#c3e9fe;
  height: 5px;
  width: 100%;
}
</style>
