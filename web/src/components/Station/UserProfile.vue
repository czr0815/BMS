<template>
  <div v-if="userInfo.name" class="profile-container box-shadow-template">
    <div class="profile-top">
      我的储能站
    </div>
    <div class="profile-head">
      <!-- <div class="head-bg">
        <div class="float-right">
          <i class="el-icon-edit"></i>
        </div>
      </div>
      <div class="profile-avatar">
        <div class="avatar-wrapper">
          <img :src="userInfo.img" />
        </div>
        <div style="font-weight: bold">
          {{ userInfo.name }}
          <i class="el-icon-star-on" style="color: #FF6E5C; font-size:22px; position:relative; top: 2px" />
        </div>
      </div> -->
      <div class="profile-stations">
        <!-- <div class="station-title">我的储能站</div> -->
        <div class="station-box">
          <div class="station-item float-shadow" v-for="(item, idx) in userInfo.stationTable" :key="idx" >
            <i :class="icons[0]" />
            <div style="margin-left: 8px;" :class="{success: item.status==1, error: item.status==2, offline: item.status==0}">{{ item.name }}</div>
            <div class="gray-text loc-right">{{ getIdText(item.id) }}</div>
          </div>
        </div>
      </div>
    </div>

  </div>
</template>

<script>
export default {
  props: {
    userInfo: {
      type: Object,
      default: () => { return {} }
    }
  },
  data() {
    return {
      icons: ['el-icon-cpu']
    }
  },
  methods: {
    getIdText: function(id) {
      return 'ID-' + id
    }
  }
}
</script>

<style lang="scss" scoped>
.profile-container {
  margin-right: 15px;
  vertical-align: top;
  display: inline-block;
  width: 370px;
  background: white;
  border-radius: 15px;
  padding: 15px;
}
.profile-head {


  .profile-stations {
  border-radius: 15px;
  .station-title {
    font: 18px;
    margin-bottom: 8px;
  }
  .station-box {
    
    .station-item {
      font: 18px;
      padding: 8px 5px;
      align-items: center;
      background: #c3e9fe;
      border-radius: 10px;
      display: flex;
      margin: 10px 0px;
      .loc-right {
        margin-left: auto;
      }
    }
    .success {
      color:#00abf3;
      font-weight: bold;
    }
    .error {
      color: rgb(103,129,143);
      font-weight: bold;
    }
    .offline {
      color: gray;
      font-weight: bold;
    }
  }
}
}
</style>
