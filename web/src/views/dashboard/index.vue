<template>
  <div class="dashboard-container">
    <!-- 修改用户信息弹框 -->
    <div>
      <el-dialog :title="userformTitle" :visible.sync="userFormVisible">
        <el-form :model="userform" ref="userForm" label-width="100px">
          <!-- <el-form-item label="头像"> -->
            <!-- :data="upLoadData" -->
            <!-- <el-upload
              class="avatar-uploader"
              action="111"
              :on-success="handleAvatarSuccess"
              :before-upload="beforeAvatarUpload"
            >
              <img :src="userform.imgUrl" class="avatar" />
            </el-upload>
          </el-form-item> -->
          <el-form-item label="用户名" >
            <el-input v-model="userform.name" auto-complete="off"></el-input>
          </el-form-item>
          <el-form-item label="用户描述">
            <el-input v-model="userform.description" auto-complete="off"></el-input>
          </el-form-item>
        </el-form>
        <div slot="footer" class="dialog-footer">
          <el-button type="primary" @click="openPwdForm">修改密码</el-button>
          <el-button type="primary" @click="sureFormSubmit">确 定</el-button>
        </div>
      </el-dialog>
    </div>
    <!-- 修改密码弹框 -->
    <div>
      <el-dialog :title="pwdformTitle" :visible.sync="pwdFormVisible">
        <el-form
          :model="pwdform"
          :rules="pwdFormRules"
          ref="pwdForm"
          label-width="100px"
        >
          <el-form-item label="手机号">
            {{ userInfo.telephone }}
          </el-form-item>
          <el-form-item label="新密码" prop="newpwd">
            <el-input
              :key="passwordType"
              ref="newpwd"
              v-model="pwdform.newpwd"
              :type="passwordType"
              placeholder="Password"
              name="newpwd"
              auto-complete="off"
            />
            <span class="show-pwd" @click="showPwd">
              <svg-icon
                :icon-class="passwordType === 'password' ? 'eye' : 'eye-open'"
              />
            </span>
          </el-form-item>
          <el-form-item label="确认密码" prop="renewpwd">
            <el-input
              :key="repasswordType"
              ref="renewpwd"
              v-model="pwdform.renewpwd"
              :type="repasswordType"
              placeholder="Password"
              name="renewpwd"
              auto-complete="off"
            />
            <span class="show-pwd" @click="showRePwd">
              <svg-icon
                :icon-class="repasswordType === 'password' ? 'eye' : 'eye-open'"
              />
            </span>
          </el-form-item>
          <el-form-item label="验证码" prop="code">
            <el-input
              ref="code"
              v-model="pwdform.code"
              placeholder="Password"
              name="code"
              auto-complete="off"
            />
          </el-form-item>
        </el-form>
        <div slot="footer" class="dialog-footer">
          <el-button
            v-if="show"
            type="danger"
            style="background-color: #de8080; border-color: #de8080"
            @click="getPhoneCode"
            >获取验证码</el-button
          >
          <el-button
            v-if="!show"
            disabled="true"
            style="color: #707070; border-color: #707070"
            >{{ count }}s后重新获取</el-button
          >
          <el-button type="primary" @click="surePwdSubmit">确 定</el-button>
        </div>
      </el-dialog>
    </div>

    <div class="chart-table-container">
      <!-- <div class="dashboard-text">仪表盘 Dashboard</div>
      <p class="gray-text">储能站的所有细节信息都在这里...</p> -->
      <div class="charts-wrapper flex-container">
        <Chart Id="dashboard-line" :option="lineOption" />
        <Chart Id="dashboard-pie" :option="pieOption" />
      </div>
      <div class="lists-wrapper flex-container">
        <List title="报警信息" :values="warningRank" />
        <List title="充电量排名" :values="chargeRank" />
        <List title="放电量排名" :values="dischargeRank" />
      </div>
      <div class="station-table-wrapper box-shadow-templat">
        <span class="table-head">储能站列表</span>
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
            @row-click="goStation"
            ref="multipleTable"
          >
            <el-table-column label="名称" align="center" width="280">
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
                    <div class="status gray-text">{{ scope.row.status }}</div>
                  </div>
                </div>
              </template>
            </el-table-column>
            <el-table-column
              prop="employeeId"
              label="管理员ID"
              align="center"
              width="200"
            ></el-table-column>
            <el-table-column
              prop="coor"
              label="位置"
              align="center"
              width="250"
            >
              <template slot-scope="scope">
                <div>
                  [ {{ scope.row.lng }} , {{ scope.row.lat }} ]
                </div>
              </template>
            </el-table-column>
            <el-table-column
              prop="order"
              label="总订单数"
              align="center"
              width="90"
            >
            </el-table-column>
            <el-table-column
              prop="orderDeal"
              label="进行中订单"
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
          :total="stationInfos.length"
          layout="total, sizes, prev, pager, next, jumper"
          background
        >
        </el-pagination>
      </div>
    </div>
    <div v-if="userInfo.id" class="profile-container box-shadow-template">
      <div class="profile-top">
        个人信息
        <div class="float-right">
        </div>
      </div>

      <div class="profile-head">
        <div class="head-bg">
          <div class="float-right">
            <i class="el-icon-edit" @click="openUserForm"></i>
          </div>
        </div>
        <div class="profile-avatar">
          <div class="avatar-wrapper">
            <v-gravatar
              :email="`${ userInfo.id }@dazto.com`"
              hostname="gravatar.zeruns.tech"
            />
          </div>
          <div style="font-weight: bold">
            ID-{{ userInfo.id }}
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

          <div class="username">
            {{ userInfo.username }}
          </div>
          <div class="telephone">
            {{ userInfo.telephone }}
          </div>

          <div class="profile-numbers">
            <div class="profile-number" v-for="item in 2" :key="item">
              <div class="icon">
                <i :class="icons[item - 1]" />
              </div>
              <div class="value">
                {{ userInfo.number[item - 1] }}
              </div>
              <div class="name gray-text">
                {{ iconNames[item - 1] }}
              </div>
            </div>
          </div>

          
        </div>
      </div>

      <div class="profile-earnings">
        <div class="earning-top">
          收益
          <div class="float-right">
            <i class="el-icon-warning-outline" />
          </div>
        </div>

        <div class="earning-content">
          <i class="el-icon-money" />
          <div class="value">¥{{ userInfo.earning }}</div>
          <div class="name">最近一个月内的收益如上</div>
        </div>

        <div class="earning-action">
          <div class="gray-text">
            查看更多细节数据<br />
            点击下方按钮
          </div>
          <div>
            <el-button type="primary">收益详情</el-button>
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
  DashboardOrderOption,
  DashboardStatusOption,
} from "@/utils/chartOpt";
import {
  getStationRank,
  getStationInfos
} from "@/api/station"
import {
  getDashboardOrder,
  getDashboardStatus,
  getUserInfo,
} from "@/api/dashboard";
import {
  updateUser,
  sendPhoneCode
} from "@/api/user"
export default {
  name: "Dashboard",
  components: { Chart, List },
  computed: {
    ...mapGetters(["name"]),
  },
  data() {
    //校验规则
    var validatePass = (rule, value, callback) => {
      if (!value) {
        callback(new Error("请输入新密码"));
      } else if (value.toString().length < 6 || value.toString().length > 18) {
        callback(new Error("密码长度为6 - 18个字符"));
      } else {
        callback();
      }
    };
    var validatePass2 = (rule, value, callback) => {
      if (value === "") {
        callback(new Error("请再次输入密码"));
      } else if (value !== this.pwdform.newpwd) {
        callback(new Error("两次输入密码不一致!"));
      } else {
        callback();
      }
    };

    return {
      //用户表单
      userformTitle: "修改用户信息", //表单标题
      userFormVisible: false, //表单是否可见
      userform: {
        imgUrl: "../../assets/uploadimg.png",
        name: "",
        description:"",
      },
      //密码表单
      pwdformTitle: "修改用户密码", //表单标题
      pwdFormVisible: false, //表单是否可见
      pwdform: { 
        newpwd: "",
        renewpwd: "",
        code: "",
        phone: "",
      },
      pwdFormRules: {
        newpwd: [{ required: true, validator: validatePass, trigger: "blur" }],
        renewpwd: [
          { required: true, validator: validatePass2, trigger: "blur" },
        ],
      },
      passwordType: "password",
      repasswordType: "password",
      //获取验证码
      show: true,
      count: "",
      timer: null,
      //统计图表
      
      userInfo: {},
      icons: ["el-icon-download", "el-icon-fork-spoon", "el-icon-upload2"],
      iconNames: ["拥有储能站", "管理储能站"],
      lineOption: {},
      pieOption: {},
      warningRank: [],
      chargeRank: [],
      dischargeRank: [],
      stationInfos: [],
      searchContent: "",

      pageSize: 3, //每页的数据条数
      currentPage: 1, //默认开始页面,
      sell: 1.0,
      purchase: 0.425
    };
  },
  mounted() {
    this.getData();
  },
  methods: {
    //展示密码
    showPwd() {
      if (this.passwordType === "password") {
        this.passwordType = "";
      } else {
        this.passwordType = "password";
      }
      this.$nextTick(() => {
        this.$refs.newpwd.focus();
      });
    },
    showRePwd() {
      if (this.repasswordType === "password") {
        this.repasswordType = "";
      } else {
        this.repasswordType = "password";
      }
      this.$nextTick(() => {
        this.$refs.renewpwd.focus();
      });
    },
    // 打开对话框
    openUserForm() {
      this.userFormVisible = true;
    },
    openPwdForm() {
      this.pwdFormVisible = true;
    },
    //图片上传成功实现本地预览
    handleAvatarSuccess(res, file) {
      let _this = this;
      _this.form.imageUrl = URL.createObjectURL(file.raw);
    },
    //上传之前的钩子函数
    beforeAvatarUpload(file) {
      let fd = new FormData();
      fd.append("file", file); //传文件
      // fd.append('srid',this.upLoadData.srid);//传其他参数
      axios.post("/api/up/file", fd).then(function (res) {
        console.log("成功");
      });
      return false; //屏蔽了action的默认上传
    },
    //提交表单
    sureFormSubmit() {
      updateUser(this.userform).then((res) => {
        this.$message({
          message: '用户信息修改成功',
          type: 'success',
          duration: 3 * 1000
        })
        getUserInfo().then((res) => {
          this.userInfo = res.data;
          this.userInfo.earning = (this.userInfo.discharge * this.sell - this.userInfo.charge * this.purchase).toFixed(2)
          this.userInfo.number = []
          this.userInfo.number.push(this.userInfo.ownNum)
          this.userInfo.number.push(this.userInfo.manageNum)

          this.pwdform.phone = this.userInfo.telephone
          this.userform.name = this.userInfo.name
          this.userInfo.description = this.userInfo.des
        });
        this.userFormVisible = false;
      })
    },
    //获取验证码
    getPhoneCode() {
      let TIME_COUNT = 10;
      if (!this.timer) {
        this.count = TIME_COUNT;
        this.show = false;
        this.timer = setInterval(() => {
          if (this.count > 0 && this.count <= TIME_COUNT) {
            this.count--;
          } else {
            this.show = true;
            clearInterval(this.timer);
            this.timer = null;
          }
        }, 1000);
      } else {
        sendPhoneCode(this.pwdform.phone).then((res) => {
          
        })
      }
    },
    surePwdSubmit() {
      updateUser(this.pwdform).then((res) => {
        this.$message({
          message: '密码修改成功',
          type: 'success',
          duration: 3 * 1000
        })
        this.pwdFormVisible = false;
      })
    },

    getData() {
      getDashboardOrder().then((res) => {
        this.lineOption = DashboardOrderOption(res.data);
      });

      getDashboardStatus().then((res) => {
        this.pieOption = DashboardStatusOption(res.data);
      })


      getUserInfo().then((res) => {
        this.userInfo = res.data;
        this.userInfo.earning = (this.userInfo.discharge * this.sell - this.userInfo.charge * this.purchase).toFixed(2)
        this.userInfo.number = []
        this.userInfo.number.push(this.userInfo.ownNum)
        this.userInfo.number.push(this.userInfo.manageNum)

        this.pwdform.phone = this.userInfo.telephone
        this.userform.name = this.userInfo.name
        this.userInfo.description = this.userInfo.des
      });

      getStationRank().then((res) => {
        // let data = res.data;
        // data.map((item) => {
        //   item.img = require(`../../assets/${item.type}.png`);
        // });
        // this.warnings = data;
        this.warningRank = []
        for(var i=0;i<res.data.warningRank.length;i++){
          var tagType = this.parseTagType(res.data.warningRank[i].status)
          this.warningRank.push({
            content: res.data.warningRank[i].stationName,
            addition: 'ID '+res.data.warningRank[i].stationId,
            type: 0,
            tagType: tagType,
            tagContent: res.data.warningRank[i].count,
            email: res.data.warningRank[i].stationId + "@dazto.com"
          })
          if(res.data.warningRank[i].stationName.length < 1){
            this.warningRank[i].content = "unknown"
          }
        }

        this.chargeRank = []
        for(var i=0;i<res.data.chargeRank.length;i++){
          var tagType = this.parseTagType(res.data.chargeRank[i].status)
          this.chargeRank.push({
            content: res.data.chargeRank[i].stationName,
            addition: 'ID '+res.data.chargeRank[i].stationId,
            type: 0,
            tagType: tagType,
            tagContent: res.data.chargeRank[i].count,
            email: res.data.chargeRank[i].stationId + "@dazto.com"
          })
          if(res.data.chargeRank[i].stationName.length < 1){
            this.chargeRank[i].content = "unknown"
          }
        }

        this.dischargeRank = []
        for(var i=0;i<res.data.dischargeRank.length;i++){
          var tagType = this.parseTagType(res.data.dischargeRank[i].status)
          this.dischargeRank.push({
            content: res.data.dischargeRank[i].stationName,
            addition: 'ID '+res.data.dischargeRank[i].stationId,
            type: 0,
            tagType: tagType,
            tagContent: Number((res.data.dischargeRank[i].count).toFixed(2)),
            email: res.data.dischargeRank[i].stationId + "@dazto.com"
          })
          if(res.data.dischargeRank[i].stationName.length < 1){
            this.dischargeRank[i].content = "unknown"
          }
        }
      })

      getStationInfos().then((res) => {
        this.stationInfos = res.data.stations
      })
    },
    goStation(row){
      this.$router.push({
        name: 'Station',
        params: {
          bcuId: row.id
        }
      })
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
    getPageData() {
      var input = this.searchContent && this.searchContent.toLowerCase();
      var items = this.stationInfos;
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
.dashboard-container {
  display: flex;
}
.flex-container {
  display: flex;
  width: 100%;
  justify-content: space-between;
}
.charts-wrapper .chart-template {
  width: 49.5%;
}
.dashboard {
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
    background-image: url("../../assets/profile-bg.jpg");
    background-size: auto 100%;
    background-position: center;
    .float-right {
      margin: 10px;
      padding: 10px;
      background: white;
      border-radius: 10px;
      color: #009d63;
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
    .username {
      font-weight: bold;
      margin-top: 35px;
      font-size: 22px;
      color: #009d63;
    }
    .telephone {
      font-weight: bold;
      margin-top: 8px;
      font-size: 20px;
    }
  }
  .cureent-electricity {
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
        background: #009d63;
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
}
.profile-numbers {
  margin: 35px 0;
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
      background: #c4f2e1;
      color: #009d63;
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
.profile-earnings {
  
  padding: 15px;
  background: #c4f2e1;
  border-radius: 15px;
}
.earning-content {
  padding: 35px 0;
  margin: 10px 0;
  border-radius: 15px;
  background: white;
  text-align: center;
  color: #009d63;
  .el-icon-money {
    font-size: 70px;
  }
  .value {
    margin: 10px 0;
    font-size: 20px;
  }
}
.earning-action {
  text-align: center;
  .gray-text {
    line-height: 28px;
    margin: 20px 0;
  }
}

.lists-wrapper {
  margin: 20px 0;
}
.lists-wrapper .list-template {
  width: 33%;
}

.station-table-wrapper {
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
    width: 100px;
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
