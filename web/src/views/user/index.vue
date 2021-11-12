<template>
  <div class="tables-container">
    <div class="inputs-container">
    </div>
    <div class="table-container">
      <el-table
        v-loading="loading"
        :data="tableData"
        style="width: 100%"
        :default-sort = "{prop: 'startTime', order: 'descending'}"
        :header-cell-style="{'text-align':'center'}"
      >
        <el-table-column
          prop="employerId"
          label="雇主ID"
          align="center">
        </el-table-column>
        <el-table-column
          prop="employerName"
          label="雇主名字"
          align="center">
        </el-table-column>
        <el-table-column
          prop="employerTelephone"
          label="雇主电话号码"
          width="180"
          align="center">
        </el-table-column>
        <el-table-column
          prop="employeeId"
          label="雇员ID"
          align="center">
        </el-table-column>
        <el-table-column
          prop="employeeName"
          label="雇员名字"
          align="center">
        </el-table-column>
        <el-table-column
          prop="employeeTelephone"
          label="雇员电话号码"
          width="180"
          align="center">
        </el-table-column>
        <el-table-column
          prop="stationId"
          label="储能站ID"
          width="180"
          align="center">
        </el-table-column>
        <el-table-column
          prop="stationName"
          label="储能站名称"
          align="center">
        </el-table-column>
        <el-table-column
          prop="startTime"
          label="开始时间"
          sortable
          align="center">
        </el-table-column>
      </el-table>
    </div>
  </div>
</template>

<script>
  import {
    getEmployments
  } from "@/api/employment";
  export default {
    data() {
      return {
        tableData: [],
        loading: false
      }
    },
    mounted() {
      this.getData();
    },
    methods: {
      getData(){
        this.loading = true;
        getEmployments().then((res) => {
          this.tableData = res.data;
        })
        this.loading = false;
      },
      // clickButton(){
      //   alert(this.tableForum.deviceId)
      // },
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