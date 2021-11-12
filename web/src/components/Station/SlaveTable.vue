<template>
  <div>
    <div class="table-head">
      {{ title }}
    </div>
    <div>
      <div class="table-container e">
        <el-table
          :data="getPageData()"
          :row-style="table - row"
          :header-cell-style="tableHeaderColor"
          ref="multipleTable"
        >
          <el-table-column label="名称" align="center" width="200">
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
            prop="user"
            label="管理员"
            align="center"
            width="150"
          ></el-table-column>
          <el-table-column
            prop="duration"
            label="运行时长"
            align="center"
            width="110"
          ></el-table-column>
          <el-table-column
            prop="online"
            label="上次上线时间"
            align="center"
            width="200"
          >
          </el-table-column>
          <el-table-column
            prop="pending"
            label="待处理报警"
            align="center"
            width="110"
          ></el-table-column>
          <el-table-column
            prop="left"
            label="当前电量"
            align="center"
            width="150"
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
        :page-sizes="[1, 5, 10, 15]"
        :page-size="pageSize"
        :total="slaveTable.length"
        layout="total, sizes, prev, pager, next, jumper"
        background
      >
      </el-pagination>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    title: {
      type: String,
      default: "下级从属列表",
    },
    slaveTable: {
      type: Array,
      default: () => {
        return [];
      },
    },
  },
};
</script>

<style lang="scss" scoped>
.table-head {
  font-size: 20px;
  font-weight: bold;
  color: #2c3653;
  margin-bottom: 5px;
  padding-left: 5px;
}
.table-row {
  margin: 10px 0;
  padding: 10px;
  display: flex;
  align-items: center;
  text-align: center;
  border-radius: 15px;
  .name-wrapper {
    width: 200px;
    text-align: left;
    display: flex;
    align-items: center;
    img {
      width: 50px;
      height: 50px;
      border-radius: 15px;
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
          background: gray;
        }
        &.warn::after {
          background: #f96e5b;
        }
      }
    }
  }
  .id {
    flex-grow: 0.4;
  }
  .administrator {
    flex-grow: 0.6;
  }
  .duration {
    width: 150px;
  }
  .online-moment {
    width: 200px;
  }
  .warning {
    width: 100px;
  }
  .sight {
    color: #009d63;
  }
  .medium {
    color: #f96e5b;
  }
  .serious {
    color: red;
  }

  .progress {
    width: 200px;
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

  &.th {
    color: #959aa9;
    .delivered,
    .pending {
      color: #959aa9;
    }
  }
}

.name-wrapper {
  width: 200px;
  text-align: left;
  display: flex;
  align-items: center;
  img {
    width: 50px;
    height: 50px;
    border-radius: 15px;
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
        background: gray;
      }
      &.warn::after {
        background: #f96e5b;
      }
    }
  }
}
</style>
