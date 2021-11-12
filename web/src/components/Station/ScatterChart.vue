<template>
  <div class="chart-template box-shadow-template" :id="id" />
</template>

<script>
// import * as ecStat from 'echarts-stat'
// import * as echarts from 'echarts';
export default {
  props: {
    Id: {
      type: String,
      default: 'template-chart'
    },
    option: {
      type: Object,
      default: {}
    }
  },
  watch: {
    option(val, oldVal){
      this.chart.setOption(val);
    } 
  },
  data() {
    return {
      id: this.Id + "-" + new Date().getTime(),
      chart: undefined
    }
  },

  mounted() {
    // echarts.registerTransform(ecStat.transform.clustering);
    this.chart = this.$echarts.init(document.getElementById(this.id))
    this.chart.setOption(this.option)
    window.addEventListener('resize', () => {
      this.chart.resize()
    })
  },

  beforeDestroy() {
    window.removeEventListener('resize', function(){})
    this.chart.clear()
  }
}
</script>

<style lang="scss" scoped>
.chart-template {
  width: 40%;
  height: 400px;
}
</style>
