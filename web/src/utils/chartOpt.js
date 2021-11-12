import * as echarts from 'echarts';

const GREEN_COLOR = "#009D63"
const RED_COLOR = "#F96E5B"
const BLUE_COLOR = "#3481B8"
const BLUE_RGB = '52,129,184'
const RED_RGB = '248,111,92'
const GREEN_RGB = '0,157,99'

const NEW_GREEN1_COLOR = "#91CC75"
const NEW_GREEN2_COLOR = "#3BA272"
const NEW_GREEN3_COLOR = "#ACD897"
const NEW_RED_COLOR = "#EE6666"
const NEW_BLUE1_COLOR = "#5470C6"
const NEW_BLUE2_COLOR = "#73C0DE"
const NEW_YELLOW_COLOR = "#FAC858"
const NEW_ORANGE_COLOR = "#FC8452"
const NEW_PURPLE_COLOR = "#9A60B4"
const NEW_PINK_COLOR = "#DA0D68"
const NEW_GRAY_COLOR = "#A9A9A9"

const NEW_GREEN1_RGB = "145,204,117"
const NEW_GREEN2_RGB = "59,162,114"
const NEW_GREEN3_RGB = "172,216,151"
const NEW_RED_RGB = "238,102,102"
const NEW_BLUE1_RGB = "84,112,198"
const NEW_BLUE2_RGB = "115,192,222"
const NEW_YELLOW_RGB = "250,200,88"
const NEW_ORANGE_RGB = "252,132,82"
const NEW_PURPLE_RGB = "154,96,180"
const NEW_PINK_RGB = "218,13,104"


const colors = [GREEN_COLOR, RED_COLOR, `rgba(${GREEN_RGB}, 0.3)`]
const newcolors = [NEW_GREEN1_COLOR, NEW_BLUE2_COLOR, NEW_RED_COLOR, NEW_YELLOW_COLOR, NEW_GREEN2_COLOR, NEW_ORANGE_COLOR, NEW_BLUE1_COLOR, NEW_PURPLE_COLOR, NEW_GREEN3_COLOR, NEW_PINK_COLOR]

export function DashboardOrderOption(data) {
    var xAxis = ["周一", "周二", "周三", "周四", "周五", "周六", "周日"]
    const { curr, last } = data;
    var currWeek = []
    var lastWeek = []
    currWeek.push(curr.mon)
    currWeek.push(curr.tue)
    currWeek.push(curr.wed)
    currWeek.push(curr.thu)
    currWeek.push(curr.fri)
    currWeek.push(curr.sat)
    currWeek.push(curr.sun)
    lastWeek.push(last.mon)
    lastWeek.push(last.tue)
    lastWeek.push(last.wed)
    lastWeek.push(last.thu)
    lastWeek.push(last.fri)
    lastWeek.push(last.sat)
    lastWeek.push(last.sun)
    return {
        title: {
            text: "充电量趋势",
            left: 30
        },
        tooltip: {
            trigger: 'axis',
            axisPointer: {
                type: 'cross',
                label: {
                    backgroundColor: '#6a7985'
                }
            }
        },
        grid: {
            left: 30,
            right: 30,
            top: 30,
            bottom: 30,
        },
        xAxis: {
            type: 'category',
            data: xAxis,
            boundaryGap: false,
            splitNumber: 7,
            axisLine: {
                show: false
            },
            axisTick: {
                show: false
            },
            splitLine: {
                show: true
            }
        },
        legend: {
            data: ["上周", "本周"],
            right: 30
        },
        yAxis: {
            type: 'value'
        },
        series: [{
            data: currWeek,
            name: '本周',
            type: 'line',
            smooth: true,
            itemStyle: {
                color: GREEN_COLOR
            },
            lineStyle: {
                width: 3
            },
            showSymbol: false,
            areaStyle: {
              color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                  offset: 0,
                  color: `rgba(${GREEN_RGB},0.8)`
              }, {
                  offset: 0.9,
                  color: `rgba(${GREEN_RGB},0)`
              }])
          }
        }, {
            data: lastWeek,
            name: '上周',
            type: 'line',
            smooth: true,
            itemStyle: {
                color: RED_COLOR
            },
            lineStyle: {
                width: 3
            },
            showSymbol: false,
        }]
    }
}

export function DashboardStatusOption(data) {
  const batteryStatus = [
      { name: '充电中', value: data.now.charge },
      { name: '放电中', value: data.now.discharge },
      { name: '待机中', value: data.now.sum - data.now.charge - data.now.discharge },
  ]
  const covert = [parseInt((data.now.charge-data.before.charge)/data.now.sum), parseInt((data.now.discharge-data.before.discharge)/data.now.sum)];

  const leftData = []
  let total = 0
  batteryStatus.map(item => { total += item.value })
  for (var i = 0; i < batteryStatus.length; i++) {
      leftData.push({
          value: batteryStatus[i].value,
          name: batteryStatus[i].name,
          itemStyle: {
              normal: {
                  color: colors[i],
                  borderRadius: 15,
                  shadowBlur: 10,
                  shadowColor: colors[i]
              }
          }
      }, {
          value: Math.round(total * 0.02),
          name: '',
          itemStyle: {
              normal: {
                  label: {
                      show: false
                  },
                  labelLine: {
                      show: false
                  },
                  color: 'rgba(0, 0, 0, 0)',
                  borderColor: 'rgba(0, 0, 0, 0)',
                  borderWidth: 0
              }
          }
      })
  }

  const rich = (color) => {
      return {
          d: {
              color: color,
              fontSize: 18,
              fontWeight: "bold",
          },
          b: {
              color: color
          }
      }
  }
  return {
      title: {
          text: "电池状态总览",
          left: 30
      },
      legend: {
          data: batteryStatus.map(item => item.name),
          right: 30
      },
      tooltip: {
          show: false,
      },
      series: [
          // 背景圆
          {
              type: 'pie',
              radius: [0, '75%'],
              center: ['25%', '55%'],
              color: "rgba(255,255,255,0.5)",
              data: [100],
              silent: true,
              itemStyle: {
                  normal: {
                      shadowBlur: 30,
                      shadowColor: `rgba(${GREEN_RGB}, 0.2)`,
                      color: "#fff"
                  }
              },
          },
          {
              type: 'pie',
              radius: ['45%', '60%'],
              center: ['25%', '55%'],
              label: {
                  show: false,
                  position: 'center'
              },
              name: '访问来源',
              avoidLabelOverlap: false,
              emphasis: {
                  label: {
                      show: true,
                      color: '#8F94A4',
                      fontSize: '20',
                      fontWeight: 'bold',
                      formatter: '{d}%\n{b}'
                  }
              },
              labelLine: {
                  show: false
              },
              data: leftData
              // No encode specified, by default, it is '2012'.
          },
          // 左大饼图中间白色的圆
          {
              type: 'pie',
              radius: [0, '32%'],
              center: ['25%', '55%'],
              color: "#fff",
              data: [100],
              silent: true,
              itemStyle: {
                  normal: {
                      shadowBlur: 25,
                      shadowColor: 'rgba(0,0,0,0.1)',
                      color: "#fff"
                  }
              },
          },
          {
              type: 'pie',
              radius: ['20%', '32%'],
              center: ['60%', '35%'],
              startAngle: covert[0] * 0.01 * 180,
              data: [
                  {
                      name: "充电中",
                      value: covert[0],
                      itemStyle: {
                          color: colors[0]
                      },
                      label: {
                          show: true,
                          formatter: '{d|+{d}%}\n{b|一小时内待机 -> 充电}',
                          rich: rich(GREEN_COLOR)
                      }

                  },
                  {
                      name: '待机中',
                      value: 100 - covert[0], itemStyle: {
                          color: colors[2]
                      }
                  }
              ],
              labelLine: {
                  show: false,
                  length2: 0,
              },
              label: {
                  show: false
              },
              itemStyle: {
                  borderWidth: 5,
                  borderRadius: 5,
                  borderColor: '#fff',
              }
          }, {
              type: 'pie',
              radius: ['20%', '32%'],
              center: ['60%', '80%'],
              startAngle: covert[1] * 0.01 * 180,
              data: [
                  {
                      name: "放电中",
                      value: covert[1],
                      itemStyle: {
                          color: colors[1]
                      },
                      label: {
                          show: true,
                          formatter: '{d|+{d}%}\n{b|一小时内待机 -> 放电}',
                          rich: rich(RED_COLOR)
                      }
                  },
                  {
                      name: '待机中',
                      value: 100 - covert[1], itemStyle: {
                          color: colors[2]
                      }
                  }
              ],
              label: {
                  show: false
              },
              labelLine: {
                  show: false,
                  length2: 0,
              },
              itemStyle: {
                  borderWidth: 5,
                  borderRadius: 5,
                  borderColor: '#fff'
              }
          }]
  }
}


export function OrderLineOption(data) {
  const {xAxis, charges, discharges} = data
  return {
    title: {
      text: '24小时充放电与时间段相关分布',
    },
    grid: {
      left: 30,
      right: 30,
      top: 45,
      bottom: 30,
    },
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'cross'
      }
    },
    xAxis: {
      type: 'category',
      boundaryGap: false,
      // prettier-ignore
      data: xAxis
    },
    yAxis: {
      type: 'value',
      axisLabel: {
        formatter: '{value} W'
      },
      axisPointer: {
        snap: true
      }
    },
    visualMap: {
      show: false,
      dimension: 0,
    },
    series: [
      {
        name: '放电量',
        type: 'line',
        smooth: true,
        // prettier-ignore
        data: discharges,
        markArea: {
          itemStyle: {
            color: 'rgba(255, 173, 177, 0.4)'
          },
          data: [
            [
              {
                name: 'Morning Peak',
                xAxis: '06:00'
              },
              {
                xAxis: '10:00'
              }
            ],
            [
              {
                name: 'Evening Peak',
                xAxis: '16:00'
              },
              {
                xAxis: '22:00'
              }
            ]
          ]
        },
        itemStyle: {
          color: NEW_GREEN1_COLOR
        }
      },
      {
        name: '充电量',
        type: 'line',
        smooth: true,
        // prettier-ignore
        data: charges,
      }
    ]
  };
}

export function OrderCircleOption(data) {
  var stationId = [], discharge = [], charge = []
  for(var i=0;i<data.length;i++){
    stationId.push('ID'+data[i].stationId)
    discharge.push(Number(data[i].dischargeSum.toFixed(2)))
    charge.push(data[i].chargeSum)
  }
  return {
    title: {
      text: '24小时内充放电与储能站相关统计',
      left: 20
    },
    color: [NEW_GREEN1_COLOR, NEW_YELLOW_COLOR, NEW_PINK_COLOR],
    angleAxis: {},
    radiusAxis: {
      type: 'category',
      data: stationId,
      z: 10
    },
    polar: {
      center: ['50%', '50%'],
      radius: ['12%', '70%'],
    },
    series: [
      {
        type: 'bar',
        data: discharge,
        coordinateSystem: 'polar',
        name: '放电订单',
        stack: 'a',
        emphasis: {
          focus: 'series'
        }
      },
      {
        type: 'bar',
        data: charge,
        coordinateSystem: 'polar',
        name: '充电订单',
        stack: 'a',
        emphasis: {
          focus: 'series'
        }
      }
    ],
    legend: {
      show: true,
      data: ['放电订单', '充电订单'],
      top: 'bottom'
    }
  };
}

export function WarningMixedOption(data) {
  var { xAxis, bcuVoltages, bcuInsulations, bmuVoltages, bmuCurrents, bmuTemperatures, bmuSmokes } = data;
  xAxis.unshift("time")
  bcuVoltages.unshift("BCU电压")
  bcuInsulations.unshift("BCU绝缘")
  bmuVoltages.unshift("BMU电压")
  bmuCurrents.unshift("BMU电流")
  bmuTemperatures.unshift("BMU温度")
  bmuSmokes.unshift("BMU烟雾")
  return {
    title: {
      text: "近7天报警类型统计",
      left: 20
    },
    legend: {
      top: 'bottom'
    },
    tooltip: {
      trigger: 'axis',
      showContent: false
    },
    dataset: {
      source: [
        xAxis, bcuVoltages, bcuInsulations, bmuVoltages, bmuCurrents, bmuTemperatures, bmuSmokes
      ]
    },
    xAxis: { type: 'category' },
    yAxis: { gridIndex: 0 },
    grid: { top: '55%' },
    series: [
      {
        type: 'line',
        smooth: true,
        seriesLayoutBy: 'row',
        emphasis: { focus: 'series' }
      },
      {
        type: 'line',
        smooth: true,
        seriesLayoutBy: 'row',
        emphasis: { focus: 'series' }
      },
      {
        type: 'line',
        smooth: true,
        seriesLayoutBy: 'row',
        emphasis: { focus: 'series' }
      },
      {
        type: 'line',
        smooth: true,
        seriesLayoutBy: 'row',
        emphasis: { focus: 'series' }
      },
      {
        type: 'line',
        smooth: true,
        seriesLayoutBy: 'row',
        emphasis: { focus: 'series' }
      },
      {
        type: 'line',
        smooth: true,
        seriesLayoutBy: 'row',
        emphasis: { focus: 'series' }
      },
      {
        type: 'pie',
        id: 'pie',
        radius: '30%',
        center: ['50%', '25%'],
        emphasis: {
          focus: 'self'
        },
        label: {
          formatter: '{b}: {@'+xAxis[7]+'} ({d}%)'
        },
        encode: {
          itemName: 'time',
          value: xAxis[7],
          tooltip: xAxis[7]
        }
      }
    ]
  }
}

export function WarningBarOption(data) {
  var stationIds = [], bcuVoltages = [], bcuInsulations = [], bmuVoltages = [], bmuCurrents = [], bmuTemperatures = [], bmuSmokes = []
  for(var i=0;i<data.length;i++){
    stationIds.push('ID'+data[i].stationId)
    bcuVoltages.push(data[i].bcuVoltage)
    bcuInsulations.push(data[i].bcuInsulation)
    bmuVoltages.push(data[i].bmuVoltage)
    bmuCurrents.push(data[i].bmuCurrent)
    bmuTemperatures.push(data[i].bmuTemperature)
    bmuSmokes.push(data[i].bmuSmoke)
  }
  return {
    title: {
      text: '24小时内各储能站报警统计',
    },  
    grid: {
      left: 30,
      right: 30,
      top: 45,
      bottom: 30,
      containLabel: true
    },
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        // Use axis to trigger tooltip
        type: 'shadow' // 'shadow' as default; can also be 'line' or 'shadow'
      }
    },
    legend: {
      top: 'bottom'
    },
    xAxis: {
      type: 'value'
    },
    yAxis: {
      type: 'category',
      data: stationIds
    },
    series: [
      {
        name: 'BCU电压',
        type: 'bar',
        stack: 'total',
        label: {
          show: true
        },
        emphasis: {
          focus: 'series'
        },
        data: bcuVoltages
      },
      {
        name: 'BCU绝缘',
        type: 'bar',
        stack: 'total',
        label: {
          show: true
        },
        emphasis: {
          focus: 'series'
        },
        data: bcuInsulations
      },
      {
        name: 'BMU电压',
        type: 'bar',
        stack: 'total',
        label: {
          show: true
        },
        emphasis: {
          focus: 'series'
        },
        data: bmuVoltages
      },
      {
        name: 'BMU电流',
        type: 'bar',
        stack: 'total',
        label: {
          show: true
        },
        emphasis: {
          focus: 'series'
        },
        data: bmuCurrents
      },
      {
        name: 'BMU温度',
        type: 'bar',
        stack: 'total',
        label: {
          show: true
        },
        emphasis: {
          focus: 'series'
        },
        data: bmuTemperatures
      },
      {
        name: 'BMU烟雾',
        type: 'bar',
        stack: 'total',
        label: {
          show: true
        },
        emphasis: {
          focus: 'series'
        },
        data: bmuSmokes
      }
    ] 
  }
}

export function WarningStatusOption(data) {
    const orderStatus = data
    const leftData = []
    const warningcolor=[NEW_RED_COLOR,NEW_ORANGE_COLOR]
    let total = 0
    orderStatus.map(item => { total += item.value })
    for (var i = 0; i < orderStatus.length; i++) {
        leftData.push({
            value: orderStatus[i].value,
            name: orderStatus[i].name,
            itemStyle: {
                normal: {
                    color: warningcolor[i],
                    borderRadius: 15,
                    shadowBlur: 10,
                    shadowColor: warningcolor[i]
                }
            }
        }, {
            value: Math.round(total * 0.02),
            name: '',
            itemStyle: {
                normal: {
                    label: {
                        show: false
                    },
                    labelLine: {
                        show: false
                    },
                    color: 'rgba(0, 0, 0, 0)',
                    borderColor: 'rgba(0, 0, 0, 0)',
                    borderWidth: 0
                }
            }
        })
    }

    const rich = (color) => {
        return {
            d: {
                color: color,
                fontSize: 18,
                fontWeight: "bold",
            },
            b: {
                color: color
            }
        }
    }
    return {
        title: {
            text: "报警状态总览",
            left: 20
        },
        legend: {
            data: orderStatus.map(item => item.name),
            top: 'bottom'
        },
        tooltip: {
            show: false,
        },
        series: [
            // 背景圆
            {
                type: 'pie',
                radius: [0, '60%'],
                center: ['50%', '50%'],
                color: "rgba(255,255,255,0.5)",
                data: [100],
                silent: true,
                itemStyle: {
                    normal: {
                        shadowBlur: 30,
                        shadowColor: `rgba(${GREEN_RGB}, 0.2)`,
                        color: "#fff"
                    }
                },
            },
            {
                type: 'pie',
                radius: ['40%', '50%'],
                center: ['50%', '50%'],
                label: {
                    show: false,
                    position: 'center'
                },
                name: '访问来源',
                avoidLabelOverlap: false,
                emphasis: {
                    label: {
                        show: true,
                        color: '#8F94A4',
                        fontSize: '20',
                        fontWeight: 'bold',
                        formatter: '{d}%\n{b}'
                    }
                },
                labelLine: {
                    show: false
                },
                data: leftData
                // No encode specified, by default, it is '2012'.
            },
            // 左大饼图中间白色的圆
            {
                type: 'pie',
                radius: [0, '25%'],
                center: ['50%', '50%'],
                color: "#fff",
                data: [100],
                silent: true,
                itemStyle: {
                    normal: {
                        shadowBlur: 25,
                        shadowColor: 'rgba(0,0,0,0.1)',
                        color: "#fff"
                    }
                },
            },
        ]
    }
}

export function ChargeStatusOption(data) {
    const chargeStatus = data
    const leftData = []
    let total = 0
    chargeStatus.map(item => { total += item.value })
    for (var i = 0; i < chargeStatus.length; i++) {
        leftData.push({
            value: chargeStatus[i].value,
            name: chargeStatus[i].name,
            itemStyle: {
                normal: {
                    color: colors[i],
                    borderRadius: 15,
                    shadowBlur: 10,
                    shadowColor: colors[i],
                }
            }
        }, {
            value: Math.round(total * 0.02),
            name: '',
            itemStyle: {
                normal: {
                    label: {
                        show: false
                    },
                    labelLine: {
                        show: false
                    },
                    color: 'rgba(0, 0, 0, 0)',
                    borderColor: 'rgba(0, 0, 0, 0)',
                    borderWidth: 0
                }
            }
        })
    }

    const rich = (color) => {
        return {
            d: {
                color: color,
                fontSize: 18,
                fontWeight: "bold",
            },
            b: {
                color: color
            }
        }
    }
    return {
        title: {
            text: "电量状态总览",
            left: 20
        },
        legend: {
            data: chargeStatus.map(item => item.name),
            top: 'bottom'
        },
        tooltip: {
            show: false,
        },
        series: [
            // 背景圆
            {
                type: 'pie',
                radius: [0, '60%'],
                center: ['50%', '50%'],
                color: "rgba(255,255,255,0.5)",
                data: [100],
                silent: true,
                itemStyle: {
                    normal: {
                        shadowBlur: 30,
                        shadowColor: `rgba(${GREEN_RGB}, 0.2)`,
                        color: "#fff"
                    }
                },
            },
            {
                type: 'pie',
                radius: ['40%', '50%'],
                center: ['50%', '50%'],
                label: {
                    show: false,
                    position: 'center'
                },
                name: '访问来源',
                avoidLabelOverlap: false,
                emphasis: {
                    label: {
                        show: true,
                        color: '#8F94A4',
                        fontSize: '20',
                        fontWeight: 'bold',
                        formatter: '{d}%\n{b}'
                    }
                },
                labelLine: {
                    show: false
                },
                data: leftData
                // No encode specified, by default, it is '2012'.
            },
            // 左大饼图中间白色的圆
            {
                type: 'pie',
                radius: [0, '25%'],
                center: ['50%', '50%'],
                color: "#fff",
                data: [100],
                silent: true,
                itemStyle: {
                    normal: {
                        shadowBlur: 25,
                        shadowColor: 'rgba(0,0,0,0.1)',
                        color: "#fff"
                    }
                },
            },
        ]
    }
}

export function StationBatteryOption(data) {
    const batteryStatus = [
        { name: '充电中', value: 20 },
        { name: '放电中', value: 30 },
        { name: '待机中', value: 10 },
    ]
    const covert = [33, 29];

    const leftData = []
    let total = 0
    batteryStatus.map(item => { total += item.value })
    for (var i = 0; i < batteryStatus.length; i++) {
        leftData.push({
            value: batteryStatus[i].value,
            name: batteryStatus[i].name,
            itemStyle: {
                normal: {
                    color: colors[i],
                    borderRadius: 15,
                    shadowBlur: 10,
                    shadowColor: colors[i]
                }
            }
        }, {
            value: Math.round(total * 0.02),
            name: '',
            itemStyle: {
                normal: {
                    label: {
                        show: false
                    },
                    labelLine: {
                        show: false
                    },
                    color: 'rgba(0, 0, 0, 0)',
                    borderColor: 'rgba(0, 0, 0, 0)',
                    borderWidth: 0
                }
            }
        })
    }

    const rich = (color) => {
        return {
            d: {
                color: color,
                fontSize: 18,
                fontWeight: "bold",
            },
            b: {
                color: color
            }
        }
    }
    return {
        title: {
            text: "电池状态总览",
            left: 30
        },
        legend: {
            data: batteryStatus.map(item => item.name),
            right: 30
        },
        tooltip: {
            show: false,
        },
        series: [
            // 背景圆
            {
                type: 'pie',
                radius: [0, '75%'],
                center: ['25%', '55%'],
                color: "rgba(255,255,255,0.5)",
                data: [100],
                silent: true,
                itemStyle: {
                    normal: {
                        shadowBlur: 30,
                        shadowColor: `rgba(${GREEN_RGB}, 0.2)`,
                        color: "#fff"
                    }
                },
            },
            {
                type: 'pie',
                radius: ['45%', '60%'],
                center: ['25%', '55%'],
                label: {
                    show: false,
                    position: 'center'
                },
                name: '访问来源',
                avoidLabelOverlap: false,
                emphasis: {
                    label: {
                        show: true,
                        color: '#8F94A4',
                        fontSize: '20',
                        fontWeight: 'bold',
                        formatter: '{d}%\n{b}'
                    }
                },
                labelLine: {
                    show: false
                },
                data: leftData
                // No encode specified, by default, it is '2012'.
            },
            // 左大饼图中间白色的圆
            {
                type: 'pie',
                radius: [0, '32%'],
                center: ['25%', '55%'],
                color: "#fff",
                data: [100],
                silent: true,
                itemStyle: {
                    normal: {
                        shadowBlur: 25,
                        shadowColor: 'rgba(0,0,0,0.1)',
                        color: "#fff"
                    }
                },
            },
            {
                type: 'pie',
                radius: ['20%', '32%'],
                center: ['60%', '35%'],
                startAngle: covert[0] * 0.01 * 180,
                data: [
                    {
                        name: "充电中",
                        value: covert[0],
                        itemStyle: {
                            color: colors[0]
                        },
                        label: {
                            show: true,
                            formatter: '{d|+{d}%}\n{b|一小时内待机 -> 充电}',
                            rich: rich(GREEN_COLOR)
                        }

                    },
                    {
                        name: '待机中',
                        value: 100 - covert[0], itemStyle: {
                            color: colors[2]
                        }
                    }
                ],
                labelLine: {
                    show: false,
                    length2: 0,
                },
                label: {
                    show: false
                },
                itemStyle: {
                    borderWidth: 5,
                    borderRadius: 5,
                    borderColor: '#fff',
                }
            }, {
                type: 'pie',
                radius: ['20%', '32%'],
                center: ['60%', '80%'],
                startAngle: covert[1] * 0.01 * 180,
                data: [
                    {
                        name: "放电中",
                        value: covert[1],
                        itemStyle: {
                            color: colors[1]
                        },
                        label: {
                            show: true,
                            formatter: '{d|+{d}%}\n{b|一小时内待机 -> 放电}',
                            rich: rich(RED_COLOR)
                        }
                    },
                    {
                        name: '待机中',
                        value: 100 - covert[1], itemStyle: {
                            color: colors[2]
                        }
                    }
                ],
                label: {
                    show: false
                },
                labelLine: {
                    show: false,
                    length2: 0,
                },
                itemStyle: {
                    borderWidth: 5,
                    borderRadius: 5,
                    borderColor: '#fff'
                }
            }]
    }
}

export function StationDischargeScatterOption(data) {
  var CLUSTER_COUNT = 5;
  var DIENSIION_CLUSTER_INDEX = 2;
  var COLOR_ALL = [
    '#37A2DA', '#e06343', '#37a354', '#b55dba', '#b5bd48', '#8378EA', '#96BFFF'
  ];
  var pieces = [];
  for (var i = 0; i < CLUSTER_COUNT; i++) {
    pieces.push({
        value: i,
        label: 'cluster ' + i,
        color: COLOR_ALL[i]
    });
  }
  var dataset = [];
  for(var i=0;i<data.length;i++){
    var time = data[i].hour + 1.0*data[i].minute/60
    dataset.push([Number(time.toFixed(2)), data[i].electricity])
  }

  var now = new Date()
  var past = new Date(new Date().getTime()-(3*3600*24*1000))
  var t1 = Number((past.getHours()*1.0+1.0*past.getMinutes()/60).toFixed(2))
  var t2 = Number((now.getHours()*1.0+1.0*now.getMinutes()/60).toFixed(2))

  return {
    title: {
      text: "订单散点统计",
      left: 30
    },
    tooltip: {
      trigger: 'item',
    },
    grid: {
      left: 30,
      right: 30,
      top: 40,
      bottom: 30,
    },
    xAxis: {
      min: t1,
      max: t2,
      type: 'value',
      splitLine: {
        show: true,
        lineStyle: {
          type: 'dashed',
          color: '#55b9b4'
        }
      }
    },
    yAxis: {
      min: 0,
      max: 100,
      type: 'value',
      splitLine: {
        show: true,
        lineStyle: {
          type: 'dashed',
          color: '#55b9b4'
        }
      }
    },
    series: [{
      symbolSize: 8,
      data: dataset,
      type: 'scatter'
    }]
  }
}

export function BCUDataStatOption(data) {
    var colors = [
        {
            color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                offset: 0, color: "rgba(255,103,114, 1)"
            },
            {
                offset: 1, color: "rgba(243,152,158, 0)"
            }
            ]),
            borderColor: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                offset: 0, color: "rgba(158,5,16, 1)"
            },
            {
                offset: 1, color: "rgba(255,171,172, 1)"
            }
            ]),
            borderWidth: 2
        },
        {
            color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
                {
                    offset: 0, color: 'rgba(0,157,99,1)'
                },
                {
                    offset: 1, color: 'rgba(15,197,243,0)'
                }
            ]),
            borderColor: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
                {
                    offset: 0, color: 'rgba(0,0,0, 1)'
                },
                {
                    offset: 1, color: 'rgba(15,197,243,1)'
                }
            ]),
            borderWidth: 2
        },
        {
            color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
                {
                    offset: 0, color: 'rgba(61,126,235, 1)'
                },
                {
                    offset: 1, color: 'rgba(15,197,243,0)'
                }
            ]),
            borderColor: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
                {
                    offset: 0, color: 'rgba(0,0,0, 1)'
                },
                {
                    offset: 1, color: 'rgba(61,126,235, 1)'
                }
            ]),
            borderWidth: 2
        }
    ];
    var barWidth = 10;

    const { xAxis, orders, warnings, uploads } = data;
    return {
        title: {
            text: "近3日数据统计",
            left: 30
        },
        tooltip: {
            trigger: 'axis',
            axisPointer: {
                type: 'shadow',
                label: {
                    backgroundColor: '#6a7985'
                }
            }
        },
        grid: {
            left: 30,
            right: 30,
            top: 30,
            bottom: 30,
        },
        xAxis: {
            type: 'category',
            boundaryGap: false,
            data: xAxis,
            axisLine: {
                lineStyle: {
                    color: 'black' //左侧显示线
                }
            },
            axisLabel: {
                textStyle: {
                    fontFamily: 'Arial'
                }
            },
        },
        legend: {
            data: ["订单次数", "异常次数", "上传次数"],
            right: 45
        },
        yAxis: {
            type: 'value',
            axisLine: {
                show: true,
                lineStyle: {
                    color: 'black' //左侧显示线
                }
            },
            splitLine: {
                show: true,
                lineStyle: {
                    type: 'dashed',
                    color: 'grey'
                }
            },
            axisLabel: {
                fontSize: 14
            }
        },
        series: [{
            data: uploads,
            name: '上传次数',
            type: 'line',
            //barWidth: barWidth,
            areaStyle: {},
            itemStyle: colors[2]
        }, {
            data: orders,
            name: '订单次数',
            type: 'line',
            //barWidth: barWidth,
            itemStyle: colors[1]
        }, {
            data: warnings,
            name: '异常次数',
            type: 'line',
            //barWidth: barWidth,
            itemStyle: colors[0]
        }]
    }
}

export function BCUWarningStatOption(data) {
  const warningStat = data
  const leftData = []
  const warningcolor=[NEW_RED_COLOR,NEW_ORANGE_COLOR]
  let total = 0
  warningStat.map(item => { total += item.value })
  for (var i = 0; i < warningStat.length; i++) {
      leftData.push({
          value: warningStat[i].value,
          name: warningStat[i].name,
          itemStyle: {
              normal: {
                  color: warningcolor[i],
                  borderRadius: 15,
                  shadowBlur: 10,
                  shadowColor: warningcolor[i]
              }
          }
      }, {
          value: Math.round(total * 0.02),
          name: '',
          itemStyle: {
              normal: {
                  label: {
                      show: false
                  },
                  labelLine: {
                      show: false
                  },
                  color: 'rgba(0, 0, 0, 0)',
                  borderColor: 'rgba(0, 0, 0, 0)',
                  borderWidth: 0
              }
          }
      })
  }

  return {
      title: {
          text: "报警状态总览",
          left: 20
      },
      legend: {
          data: warningStat.map(item => item.name),
          top: 'bottom'
      },
      tooltip: {
          show: false,
      },
      series: [
          // 背景圆
          {
              type: 'pie',
              radius: [0, '60%'],
              center: ['50%', '50%'],
              color: "rgba(255,255,255,0.5)",
              data: [100],
              silent: true,
              itemStyle: {
                  normal: {
                      shadowBlur: 30,
                      shadowColor: `rgba(${GREEN_RGB}, 0.2)`,
                      color: "#fff"
                  }
              },
          },
          {
              type: 'pie',
              radius: ['40%', '50%'],
              center: ['50%', '50%'],
              label: {
                  show: false,
                  position: 'center'
              },
              name: '访问来源',
              avoidLabelOverlap: false,
              emphasis: {
                  label: {
                      show: true,
                      color: '#8F94A4',
                      fontSize: '20',
                      fontWeight: 'bold',
                      formatter: '{d}%\n{b}'
                  }
              },
              labelLine: {
                  show: false
              },
              data: leftData
              // No encode specified, by default, it is '2012'.
          },
          // 左大饼图中间白色的圆
          {
              type: 'pie',
              radius: [0, '25%'],
              center: ['50%', '50%'],
              color: "#fff",
              data: [100],
              silent: true,
              itemStyle: {
                  normal: {
                      shadowBlur: 25,
                      shadowColor: 'rgba(0,0,0,0.1)',
                      color: "#fff"
                  }
              },
          },
      ]
  }
}

export function BMUDataStatOption(data) {
  var colors = [
      {
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
              offset: 0, color: "rgba(255,103,114, 1)"
          },
          {
              offset: 1, color: "rgba(243,152,158, 0)"
          }
          ]),
          borderColor: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
              offset: 0, color: "rgba(158,5,16, 1)"
          },
          {
              offset: 1, color: "rgba(255,171,172, 1)"
          }
          ]),
          borderWidth: 2
      },
      {
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
              {
                  offset: 0, color: 'rgba(0,157,99,1)'
              },
              {
                  offset: 1, color: 'rgba(15,197,243,0)'
              }
          ]),
          borderColor: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
              {
                  offset: 0, color: 'rgba(0,0,0, 1)'
              },
              {
                  offset: 1, color: 'rgba(15,197,243,1)'
              }
          ]),
          borderWidth: 2
      },
      {
          color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
              {
                  offset: 0, color: 'rgba(61,126,235, 1)'
              },
              {
                  offset: 1, color: 'rgba(15,197,243,0)'
              }
          ]),
          borderColor: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
              {
                  offset: 0, color: 'rgba(0,0,0, 1)'
              },
              {
                  offset: 1, color: 'rgba(61,126,235, 1)'
              }
          ]),
          borderWidth: 2
      }
  ];
  var barWidth = 10;

  const { xAxis, warnings, batteries, currents, temperatures, smokes} = data;
  return {
      title: {
          text: "近3日数据统计",
          left: 30
      },
      tooltip: {
          trigger: 'axis',
          axisPointer: {
              type: 'shadow',
              label: {
                  backgroundColor: '#6a7985'
              }
          }
      },
      grid: {
          left: 30,
          right: 30,
          top: 30,
          bottom: 30,
      },
      xAxis: {
          type: 'category',
          boundaryGap: false,
          data: xAxis,
          axisLine: {
              lineStyle: {
                  color: 'black' //左侧显示线
              }
          },
          axisLabel: {
              textStyle: {
                  fontFamily: 'Arial'
              }
          },
      },
      legend: {
          data: ["异常次数", "电池串上传次数", "电流上传次数", "温度上传次数", "烟雾上传次数"],
          right: 45
      },
      yAxis: {
          type: 'value',
          axisLine: {
              show: true,
              lineStyle: {
                  color: 'black' //左侧显示线
              }
          },
          splitLine: {
              show: true,
              lineStyle: {
                  type: 'dashed',
                  color: 'grey'
              }
          },
          axisLabel: {
              fontSize: 14
          }
      },
      series: [{
          data: batteries,
          name: '电池串上传次数',
          type: 'line',
          //barWidth: barWidth,
          itemStyle: newcolors[1]
      }, {
        data: currents,
        name: '电流上传次数',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[2]
      }, {
        data: temperatures,
        name: '温度上传次数',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[3]
      }, {
        data: smokes,
        name: '烟雾上传次数',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[4]
      }, {
        data: warnings,
        name: '异常次数',
        type: 'line',
        //barWidth: barWidth,
        areaStyle: {},
        itemStyle: colors[0]
      }]
  }
}

export function BMUWarningStatOption(data) {
  const warningStat = data
  const leftData = []
  const warningcolor=[NEW_RED_COLOR,NEW_ORANGE_COLOR]
  let total = 0
  warningStat.map(item => { total += item.value })
  for (var i = 0; i < warningStat.length; i++) {
      leftData.push({
          value: warningStat[i].value,
          name: warningStat[i].name,
          itemStyle: {
              normal: {
                  color: warningcolor[i],
                  borderRadius: 15,
                  shadowBlur: 10,
                  shadowColor: warningcolor[i]
              }
          }
      }, {
          value: Math.round(total * 0.02),
          name: '',
          itemStyle: {
              normal: {
                  label: {
                      show: false
                  },
                  labelLine: {
                      show: false
                  },
                  color: 'rgba(0, 0, 0, 0)',
                  borderColor: 'rgba(0, 0, 0, 0)',
                  borderWidth: 0
              }
          }
      })
  }

  return {
      title: {
          text: "报警状态总览",
          left: 20
      },
      legend: {
          data: warningStat.map(item => item.name),
          top: 'bottom',
          orient: 'horizontal'
      },
      tooltip: {
          show: false,
      },
      series: [
          // 背景圆
          {
              type: 'pie',
              radius: [0, '60%'],
              center: ['50%', '50%'],
              color: "rgba(255,255,255,0.5)",
              data: [100],
              silent: true,
              itemStyle: {
                  normal: {
                      shadowBlur: 30,
                      shadowColor: `rgba(${GREEN_RGB}, 0.2)`,
                      color: "#fff"
                  }
              },
          },
          {
              type: 'pie',
              radius: ['40%', '50%'],
              center: ['50%', '50%'],
              label: {
                  show: false,
                  position: 'center'
              },
              name: '访问来源',
              avoidLabelOverlap: false,
              emphasis: {
                  label: {
                      show: true,
                      color: '#8F94A4',
                      fontSize: '20',
                      fontWeight: 'bold',
                      formatter: '{d}%\n{b}'
                  }
              },
              labelLine: {
                  show: false
              },
              data: leftData
              // No encode specified, by default, it is '2012'.
          },
          // 左大饼图中间白色的圆
          {
              type: 'pie',
              radius: [0, '25%'],
              center: ['50%', '50%'],
              color: "#fff",
              data: [100],
              silent: true,
              itemStyle: {
                  normal: {
                      shadowBlur: 25,
                      shadowColor: 'rgba(0,0,0,0.1)',
                      color: "#fff"
                  }
              },
          },
      ]
  }
}

export function BMUWarningCalendarOption(data){
  var fullMonth = new Date().getFullYear() + "-"
  var month = new Date().getMonth() + 1;
  if(month<10){
    month  = "0" + month
  }
  fullMonth += month

  var warningCalendar = data
  var leftData = [], index = 0
  const day = new Date().getDate()
  if(warningCalendar.length === 0){
    for(var i = 1; i <= day; i++){
      var tmp = i
      if(tmp<10)  tmp="0"+tmp
      leftData.push([
        fullMonth+"-"+tmp, 0
      ])
    }
  }else{
    warningCalendar.sort(function(a, b){
      return parseInt(a.date)-parseInt(b.date);
    })
    for (var i = 1; i <= day; i++) {
      var tmp = i
      if(tmp<10)  tmp="0"+tmp
      if(index >= warningCalendar.length || parseInt(warningCalendar[index].date) != i){
        leftData.push([
          fullMonth+"-"+tmp, 0
        ])
      }else{
        leftData.push([
          fullMonth+'-'+tmp, warningCalendar[index++].value
        ])
      }
    }
  }

  return {
    title: {
      text: "当月报警统计",
      left: 20
    },
    tooltip: {
      position: 'top'
    },
    visualMap: [{
      min: 0,
      max: 1000,
      calculable: true,
      seriesIndex: [0],
      inRange: {
        color: [NEW_GREEN1_COLOR, NEW_YELLOW_COLOR, NEW_ORANGE_COLOR, NEW_RED_COLOR ]
      },
      show: false
    }],
    calendar: [{
      orient: 'vertical',
      yearLabel: {
        show: false
      },
      dayLabel: {
        firstDay: 1,
        nameMap: ['周日', '周一', '周二', '周三', '周四', '周五', '周六']
      },
      monthLabel: {
        show: false
      },
      cellSize: 35,
      top: 80,
      left:20, 
      right: 20,
      range: fullMonth
    }],

    series: [{
      type: 'heatmap',
      coordinateSystem: 'calendar',
      calendarIndex: 0,
      data: leftData
  }]
  }
}

export function BMUSOCStatOption(data) {
  const socStat = data
  const leftData = []
  const socColors = [NEW_RED_COLOR, NEW_YELLOW_COLOR, NEW_GREEN1_COLOR, NEW_GREEN2_COLOR, NEW_GRAY_COLOR]
  let total = 0
  socStat.map(item => { total += item.value })
  for (var i = 0; i < socStat.length; i++) {
    leftData.push({
      value: socStat[i].value,
      name: socStat[i].name,
      itemStyle: {
        normal: {
          color: socColors[i],
          borderRadius: 15,
          shadowBlur: 10,
          shadowColor: socColors[i]
        }
      }
    }, {
      value: Math.round(total * 0.02),
      name: '',
      itemStyle: {
        normal: {
          label: {
            show: false
          },
          labelLine: {
            show: false
          },
          color: 'rgba(0, 0, 0, 0)',
          borderColor: 'rgba(0, 0, 0, 0)',
          borderWidth: 0
        }
      }
    })
  }

  return {
    title: {
      text: "SOC状态总览",
      left: 20
    },
    legend: [{
      data: ["<20%", "20~50%"],
      bottom: 20,
      orient: 'horizontal'
    },{
      data: ["50~80%", ">80%", "unknown"],
      top: 'bottom',
      orient: 'horizontal'
    }],
    tooltip: {
      show: false,
    },
    series: [
      // 背景圆
      {
        type: 'pie',
        radius: [0, '60%'],
        center: ['50%', '50%'],
        color: "rgba(255,255,255,0.5)",
        data: [100],
        silent: true,
        itemStyle: {
          normal: {
            shadowBlur: 30,
            shadowColor: `rgba(${GREEN_RGB}, 0.2)`,
            color: "#fff"
          }
        },
      },
      {
        type: 'pie',
        radius: ['40%', '50%'],
        center: ['50%', '50%'],
        label: {
          show: false,
          position: 'center'
        },
        name: '访问来源',
        avoidLabelOverlap: false,
        emphasis: {
          label: {
            show: true,
            color: '#8F94A4',
            fontSize: '20',
            fontWeight: 'bold',
            formatter: '{d}%\n{b}'
          }
        },
        labelLine: {
          show: false
        },
        data: leftData
        // No encode specified, by default, it is '2012'.
      },
      // 左大饼图中间白色的圆
      {
        type: 'pie',
        radius: [0, '25%'],
        center: ['50%', '50%'],
        color: "#fff",
        data: [100],
        silent: true,
        itemStyle: {
          normal: {
            shadowBlur: 25,
            shadowColor: 'rgba(0,0,0,0.1)',
            color: "#fff"
          }
        }
      }
    ]
  }
}

export function BMUOutlineStatOption(data) {
  const { xAxis, voltages, socs, currents, smokes, temperatures} = data;
  return {
      title: {
          text: "近12小时数据变化趋势",
          left: 30
      },
      tooltip: {
          trigger: 'axis',
          axisPointer: {
              type: 'shadow',
              label: {
                  backgroundColor: '#6a7985'
              }
          }
      },
      grid: {
          left: 30,
          right: 30,
          top: 30,
          bottom: 30,
      },
      xAxis: {
          type: 'category',
          boundaryGap: false,
          data: xAxis,
          axisLine: {
              lineStyle: {
                  color: 'black' //左侧显示线
              }
          },
          axisLabel: {
              textStyle: {
                  fontFamily: 'Arial'
              }
          },
      },
      legend: {
          data: ["平均单体电压", "平均单体SOC", "电流", "平均温度", "平均烟雾"],
          right: 45
      },
      yAxis: {
          type: 'value',
          axisLine: {
              show: true,
              lineStyle: {
                  color: 'black' //左侧显示线
              }
          },
          splitLine: {
              show: true,
              lineStyle: {
                  type: 'dashed',
                  color: 'grey'
              }
          },
          axisLabel: {
              fontSize: 14
          }
      },
      series: [{
        data: voltages,
        name: '平均单体电压',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[1]
      }, {
        data: socs,
        name: '平均单体SOC',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[2]
    }, {
        data: currents,
        name: '电流',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[3]
      }, {
        data: temperatures,
        name: '平均温度',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[4]
      }, {
        data: smokes,
        name: '平均烟雾',
        type: 'line',
        //barWidth: barWidth,
        itemStyle: newcolors[5]
      }]
  }
}

export function ScreenOrderIncomeOption(data) {
  const {xAxis, discharges, charges} = data
    return {
        title: {
            show: false
        },
        tooltip: {
            trigger: 'axis',
            axisPointer: {
                lineStyle: {
                    color: {
                        type: 'linear',
                        x: 0,
                        y: 0,
                        x2: 0,
                        y2: 1,
                        colorStops: [{
                            offset: 0,
                            color: 'rgba(0,179,244,0)'
                        }, {
                            offset: 0.5,
                            color: 'rgba(255, 255, 255,1)',
                        }, {
                            offset: 1,
                            color: 'rgba(0,179,244,0)'
                        }],
                        global: false
                    }
                },
            },
        },
        grid: {
            top: '5%',
            left: '5%',
            right: '5%',
            bottom: '10%',
            // containLabel: true
        },
        xAxis: [{
            type: 'category',
            axisLine: {
                show: true,
                color: '#09364F'
            },
            splitArea: {
                // show: true,
                color: '#f00',
                lineStyle: {
                    color: '#f00'
                },
            },
            axisLabel: {
                color: '#4C626E'
            },
            splitLine: {
                show: false
            },
            boundaryGap: false,
            //data: ['9:00', '10:00', '11:00', '12:00', '13:00', '14:00'],
            data: xAxis

        }],

        yAxis: [{
            type: 'value',
            min: 0,
            // max: 140,
            splitNumber: 4,
            splitLine: {
                show: true,
                lineStyle: {
                    color: 'rgba(255,255,255,0.1)'
                }
            },
            axisLine: {
                show: true,
            },
            axisLabel: {
                margin: 20,
                textStyle: {
                    color: '#d1e6eb',
                },
            },

        }],
        series: [{
            name: '放电总量',
            type: 'line',
            smooth: true, //是否平滑
            showAllSymbol: true,
            // symbol: 'image://./static/images/guang-circle.png',
            symbol: 'circle',
            symbolSize: 15,
            lineStyle: {
                normal: {
                    color: "#00b3f4",
                    shadowColor: 'rgba(0, 0, 0, .3)',
                    shadowBlur: 0,
                    shadowOffsetY: 5,
                    shadowOffsetX: 5,
                },
            },
            label: {
                show: false,
            },
            itemStyle: {
                color: "#000",
                borderColor: "#00b3f4",
                borderWidth: 3,
                shadowColor: 'rgba(0, 0, 0, .3)',
                shadowBlur: 0,
                shadowOffsetY: 2,
                shadowOffsetX: 2,
            },
            tooltip: {
                show: true
            },
            areaStyle: {
                normal: {
                    color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                        offset: 0,
                        color: 'rgba(0,179,244,0.3)'
                    },
                    {
                        offset: 1,
                        color: 'rgba(0,179,244,0)'
                    }
                    ], false),
                    shadowColor: 'rgba(0,179,244, 0.9)',
                    shadowBlur: 20
                }
            },
            //data: [502.84, 205.97, 332.79, 281.55, 398.35, 214.02,]
            data: discharges
        }
        ]
    }
}

export function ScreenWarningTypeOption(data) {
  const {bcuVoltageNum, bcuInsulationNum, bmuVoltageNum, bmuCurrentNum, bmuSmokeNum, bmuTemperatureNum} = data
  var left = []
  left.push({value: bcuVoltageNum, name:"BCU电压"})
  left.push({value: bcuInsulationNum, name:"BCU绝缘性"})
  left.push({value: bmuVoltageNum, name:"BMU电压"})
  left.push({value: bmuCurrentNum, name:"BMU电流"})
  left.push({value: bmuSmokeNum, name:"BMU烟雾"})
  left.push({value: bmuTemperatureNum, name:"BMU温度"})
  let leg = ["BCU电压", "BCU绝缘性", "BMU电压", "BMU电流", "BMU烟雾", "BMU温度"]
    let legend = {
        "BCU电压": bcuVoltageNum,
        "BCU绝缘性": bcuInsulationNum,
        "BMU电压": bmuVoltageNum,
        "BMU电流": bmuCurrentNum,
        "BMU烟雾": bmuSmokeNum,
        "BMU温度": bmuTemperatureNum
    }
    return {
        tooltip: {
            trigger: 'item',
            formatter: "{a} <br/>{b} : {c} ({d}%)",
            textStyle: {
                fontSize: 16,
            },
        },
        legend: {
            data: leg,
            orient: 'vertical',
            right: '5%',
            top: '10%',
            itemWidth: 20,
            itemHeight: 20,
            itemGap: 10,
            borderRadius: 15,
            textStyle: {
                padding: [0, 0, 2, 4],
                color: "#ffffff",
                rich: {
                  a: {
                    color: "#fff",
                    fontSize: "18px",
                },
                b: {
                    color: "#386d88",
                    lineHeight: 24,
                    fontSize: "14px"
                }
                }
            }
        },
        series: [{
            name: '报警占比',
            type: 'pie',
            clockwise: false,
            startAngle: 90,
            radius: ['35%', '80%'],
            center: ['35%', '54%'],
            roseType: 'area', //area
            data: left,
            itemStyle: {
                normal: {
                    borderColor: '#273454',
                    borderWidth: '5',
                },
            },
            label: {
                show: false,
            },
            labelLine: {
                show: false
            },
        }],
    }
}

export function ScreenBatteryOption(data) {
  let color = ['#00A1F5', '#3262FE', '#FFBE45', '#FE6664', '#0034ff'];
  let chartData = [
      { value: data.abundant },
      { value: data.medium },
      { value: data.less },
      { value: data.scare }
  ];
  let sum = 0;
  let pieSeries = [];

  // 数据处理
  chartData.forEach((v, i) => {
      sum = sum + v.value;
  })

  // 图表option整理
  chartData.forEach((v, i) => {
      pieSeries.push({
          name: '电池剩余电量信息',
          type: 'pie',
          hoverAnimation: false,
          radius: [75 - i * 20 + '%', 67 - i * 20 + '%'],
          center: ["50%", "50%"],
          label: {
              show: false
          },
          data: [{
              value: v.value,
              name: i + "",
              itemStyle: {
                  borderRadius: 10,
                  borderColor: '#062942',
                  borderWidth: 2
              }
          }, {
              value: sum - v.value,
              name: '',
              itemStyle: {
                  color: "rgba(0,0,0,0)"
              }
          }]
      });
      pieSeries.push({
          name: '',
          type: 'pie',
          silent: true,
          z: 1,
          hoverAnimation: false, //鼠标移入变大
          radius: [75 - i * 20 + '%', 67 - i * 20 + '%'],
          center: ["50%", "50%"],
          label: {
              show: false
          },
          data: [{
              value: 10,
              itemStyle: {
                  color: "#062942"
              }
          }]
      });
  })

  let option = {
      color: color,
      grid: {
          top: '12%',
          bottom: '54%',
          left: "40%",
          containLabel: false
      },
      xAxis: [{
          show: false
      }],
      series: pieSeries
  };

  return option
}

export function OrderTypeOption(data) {

    let legend = data
    return {
        title: {
            text: "订单类型总览",
            left: 20
        },
        color: newcolors,
        shadowColor: newcolors,

        tooltip: {
            trigger: 'item',
            formatter: "{a} <br/>{b} : {c} ({d}%)",
            textStyle: {
                fontSize: 16,
            },
        },
        legend: {
            data: ['微信支付', '支付宝', '银联支付'],
            orient: 'vertical',
            left: 20,
            top: 'bottom',
            itemWidth: 20,
            itemHeight: 20,
            itemGap: 20,
            borderRadius: 15,
            formatter: function (name) {
                return `{a|${legend[name]}%}\n{b|${name}}`
            },
            textStyle: {
                padding: [0, 0, 2, 4],
                rich: {
                    a: {
                        color: "#F96E5B",
                        fontSize: "18px",
                    },
                    b: {
                        color: "#386d88",
                        lineHeight: 24,
                        fontSize: "14px"
                    }
                }
            }
        },
        series: [{
            name: '今日订单',
            type: 'pie',
            clockwise: false,
            startAngle: 90,
            radius: ['20%', '60%'],
            center: ['70%', '55%'],
            roseType: 'area', //area
            data: [{
                value: 435,
                name: '微信支付'
            },
            {
                value: 310,
                name: '支付宝'
            },
            {
                value: 234,
                name: '银联支付'
            }
            ],
            itemStyle: {
                normal: {
                    borderRadius: 25,
                    shadowBlur: 3,

                }
            },

            label: {
                show: false,
            },
            labelLine: {
                show: false
            },
        }],
    }
}

export function OrderBmuOption(data) {

    const bmu = data
    return {
        title: {
            text: "BMU订单统计",
            left: 20
        },
        color: newcolors,
        shadowColor: newcolors,
        tooltip: {
            trigger: 'item',
            formatter: "{a} <br/>{b} : {c} ({d}%)",
            textStyle: {
                fontSize: 16,
            },
        },
        legend: {
            data: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10'],

            top: 'bottom',
            itemWidth: 10,
            itemHeight: 10,
            borderRadius: 15,
            formatter: function (name) {
                return `{a|${name}}`
            },
            textStyle: {
                padding: [0, 0, 2, 4],
                rich: {

                    a: {
                        color: "#386d88",
                        lineHeight: 24,
                        fontSize: "14px"
                    }
                }
            }
        },
        series: [{
            name: 'BMU',
            type: 'pie',
            startAngle: 90,
            radius: ['10%', '45%'],
            center: ['50%', '45%'],
            roseType: 'area', //area
            data: bmu,
            itemStyle: {
                normal: {
                    borderRadius: 20,
                    shadowBlur: 2,

                }
            },

        }],
    }
}

export function WarningTypeOption(data) {
    const warningcolor=[NEW_RED_COLOR,NEW_ORANGE_COLOR,NEW_YELLOW_COLOR]
    let legend = data
    return {
        title: {
            text: "报警类型总览",
            left: 20
        },
        color: warningcolor,
        shadowColor: warningcolor,

        tooltip: {
            trigger: 'item',
            formatter: "{a} <br/>{b} : {c} ({d}%)",
            textStyle: {
                fontSize: 16,
            },
        },
        legend: {
            data: ['温度', '电压', '烟雾'],
            orient: 'vertical',
            left: 20,
            top: 'bottom',
            itemWidth: 20,
            itemHeight: 20,
            itemGap: 20,
            borderRadius: 15,
            formatter: function (name) {
                return `{a|${legend[name]}%}\n{b|${name}}`
            },
            textStyle: {
                padding: [0, 0, 2, 4],
                rich: {
                    a: {
                        color: "#F96E5B",
                        fontSize: "18px",
                    },
                    b: {
                        color: "#386d88",
                        lineHeight: 24,
                        fontSize: "14px"
                    }
                }
            }
        },
        series: [{
            name: '今日报警',
            type: 'pie',
            clockwise: false,
            startAngle: 90,
            radius: ['20%', '60%'],
            center: ['70%', '55%'],
            roseType: 'area', //area
            data: [{
                value: 235,
                name: '温度'
            },
            {
                value: 300,
                name: '电压'
            },
            {
                value: 134,
                name: '烟雾'
            }
            ],
            itemStyle: {
                normal: {
                    borderRadius: 25,
                    shadowBlur: 3,

                }
            },

            label: {
                show: false,
            },
            labelLine: {
                show: false
            },
        }],
    }
}

export function WarningBmuOption(data) {

    const bmu = data
    return {
        title: {
            text: "BMU报警统计",
            left: 20
        },
        color: newcolors,
        shadowColor: newcolors,
        tooltip: {
            trigger: 'item',
            formatter: "{a} <br/>{b} : {c} ({d}%)",
            textStyle: {
                fontSize: 16,
            },
        },
        legend: {
            data: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10'],

            top: 'bottom',
            itemWidth: 10,
            itemHeight: 10,
            borderRadius: 15,
            formatter: function (name) {
                return `{a|${name}}`
            },
            textStyle: {
                padding: [0, 0, 2, 4],
                rich: {

                    a: {
                        color: "#386d88",
                        lineHeight: 24,
                        fontSize: "14px"
                    }
                }
            }
        },
        series: [{
            name: 'BMU',
            type: 'pie',
            startAngle: 90,
            radius: ['10%', '45%'],
            center: ['50%', '45%'],
            roseType: 'area', //area
            data: bmu,
            itemStyle: {
                normal: {
                    borderRadius: 20,
                    shadowBlur: 2,

                }
            },

        }],
    }
}

export function ScreenAccessOption(data) {
    var xData = ["6:00", "7:00", "8:00", "9:00", "10:00", "11:00"];
    const dataBase = {
        'APP访问': [400, 500, 600, 300, 200, 100],
        '微信小程序': [350, 450, 550, 550, 300, 200],
    }
    const color = ['#00ADFF', '#315FFC', '#0CEBEA', '#1fa9e3']
    let serviceList = []
    let index = 0
    for (let key in dataBase) {
        let item = {
            name: key,
            type: 'bar',
            barWidth: '15%',
            itemStyle: {
                normal: {
                    color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [{
                        offset: 0,
                        color: color[index]
                    }, {
                        offset: 1,
                        color: 'rgba(18,155,249,0)'
                    }]),
                    barBorderRadius: 11,
                }
            },
            data: dataBase[key]
        }
        index++
        serviceList.push(item)
    }
    var option = {
        tooltip: {
            trigger: 'axis',
            backgroundColor: 'none',
            padding: 5,
            formatter: function (param) {
                var resultTooltip = "<div style='background:rgba(13,5,30,.6);border:1px solid rgba(255,255,255,.2);padding:5px;border-radius:3px;color:#fff'>" +
                    "<div style='text-align:center;'>" + param[0].name + "</div>" +
                    "<div style='padding-top:5px;color:#fff'>";
                for (var i = 0; i < param.length; i++) {
                    resultTooltip +=
                        "<span style='display:inline-block;margin-right:5px;border-radius:10px;width:10px;height:10px;background-color:" + param[i].color.colorStops[0].color + ";'></span>" +
                        "<span> " + param[i].seriesName + ": </span>" +
                        "<span>" + param[i].value + "</span></span></br>"

                }
                resultTooltip += "</div>";

                return resultTooltip
            }
        },
        grid: {
            left: '30',
            top: '10%',
            right: '0%',
            bottom: '10%',
        },
        legend: {
            show: false,
            icon: 'circle',
        },
        xAxis: [{
            type: 'category',
            data: xData,
            axisLabel: {
                show: true,
                fontSize: 10,
                textStyle: {
                    color: "#C9C8CD" //X轴文字颜色
                }
            },
            axisTick: {
                show: false //不显示刻度
            },
            // boundaryGap: false,
            splitLine: {
                show: false,
                width: 0.08,
                lineStyle: {
                    type: "solid",
                    color: "#03202E"
                }
            }
        }],
        yAxis: [{
            type: 'value',
            axisTick: {
                show: false
            },
            axisLabel: {
                textStyle: {
                    color: "#C9C8CD" //X轴文字颜色
                }
            },
            axisLine: {
                inside: false,
                show: true,
            },
            splitLine: {
                show: false
            }
        }
        ],
        series: serviceList
    };
    return option
}