<template>
    <div class="chart-template box-shadow-template" :id="id">
    </div>
</template>


<script>
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
          this.chart.on('updateAxisPointer', function (event) {
            const xAxisInfo = event.axesInfo[0];
            if (xAxisInfo) {
              const dimension = xAxisInfo.value + 1;
              this.chart.setOption({
                series: {
                  id: 'pie',
                  label: {
                    formatter: '{b}: {@[' + dimension + ']} ({d}%)'
                  },
                  encode: {
                    value: dimension,
                    tooltip: dimension
                  }
                }
              });
            }
          });
          this.chart.setOption(val);
        } 
    },
    data() {
        return {
            id: this.Id + "-" + new Date().getTime(),
            chart: undefined,
        }
    },

    mounted() {
        this.chart = this.$echarts.init(document.getElementById(this.id))
        this.chart.on('updateAxisPointer', function (event) {
          const xAxisInfo = event.axesInfo[0];
          if (xAxisInfo) {
            const dimension = xAxisInfo.value + 1;
            this.chart.setOption({
              series: {
                id: 'pie',
                label: {
                  formatter: '{b}: {@[' + dimension + ']} ({d}%)'
                },
                encode: {
                  value: dimension,
                  tooltip: dimension
                }
              }
            });
          }
        });
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

</style>