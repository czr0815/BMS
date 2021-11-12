<template>
  <div class="list-template box-shadow-template">
    <div class="title">
      {{ title }}
      <div class="float-right">
        <i class="el-icon-more-outline" />
      </div>
    </div>
    <div class="list-wrapper ">
      <div class="list-item float-shadow" v-for="(item, idx) in values" :key="idx">
        <div class="img-wrapper" :class="{round: round}">
          <img v-if="item.type" :src="item.img"/>
          <v-gravatar v-else :email="item.email" hostname="gravatar.zeruns.tech"/>
        </div>
        <div class="content-wrapper">
          <div class="content">
            {{ item.content }}
          </div>
          <div class="gray-text">
            {{ item.addition }}
          </div>
        </div>
        <div class="tag-wrapper">
          <el-tag :type="item.tagType" :effect="item.email ? 'light': 'dark'">
            {{ item.tagContent }}
          </el-tag>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    title: {
      type: String,
      default: "列表"
    },
    values: {
      type: Array,
      default: () => { return []}
    },
    round: {
      type: Boolean,
      default: false
    }
  },
  data() {
    return {

    }
  }
}
</script>

<style lang="scss" scoped>
.title {
  padding: 0 10px;
  font-weight: bold;
  color: #3A445F;
}
.list-wrapper {
  .list-item {
    display: flex;
    margin: 10px 0;
    align-items: center;
    padding: 10px;
    border-radius: 10px;
  }
  .img-wrapper {
    width: 50px;
    height: 50px;
    background: #c4f2e1;
    border-radius: 10px;
    padding: 5px;
    img {
      max-width: 100%;
      max-height: 100%;
      object-fit: cover;
    }
  }
  .content-wrapper {
    flex-grow: 1;
    margin: 0 10px;
    line-height: 22px;
    .gray-text {
      font-size: 14px;
    }
  }
}
</style>