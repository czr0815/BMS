const BcuStatData = {
    xAxis: ['6-12 0时', '4时', '8时', '12时', '16时', '20时', '6-13 0时', '4时', '8时', '12时', '16时', '20时', '6-14 0时', '4时', '8时', '12时', '16时', '20时'],
    connects: [5, 2, 4, 12, 18, 34, 6, 0, 5, 11, 19, 32, 5, 2, 5, 12, 19, 35],
    warns: [0, 0, 0, 1, 2, 5, 0, 0, 0, 2, 3, 5, 0, 1, 5, 4, 3, 8],
    uploads: [80, 80, 87, 94, 97, 116, 82, 80, 83, 86, 90, 110, 81, 80, 84, 91, 95, 112]
  }

  const OrderStatData = [
    { name: '已完成', value: 30 },
    { name: '进行中', value: 30 },
    { name: '无效', value: 30 },
]

const WarningStatData = [
    { name: '已处理', value: 30 },
    { name: '未处理', value: 30 },
]
const ChargeStatData = [
    { name: '充沛', value: 30 },
    { name: '较低', value: 30 },
    { name: '中等', value: 30 },
]
const slaveTable = [
    {
      name: '1号BCU',
      status: 'online',
      id: '00005584001',
      administrator: '咸水沽储能管理员',
      duration: '0.5h',
      online: '2021-06-13 10:41',
      warning: 0,
      left: 35
    },
    {
      name: '2号BCU',
      status: 'offline',
      id: '00005584002',
      administrator: '咸水沽储能管理员',
      duration: '8.9h',
      online: '2021-06-12 10:41',
      warning: 5,
      left: 85
    },
    {
      name: '3号BCU',
      status: 'warn',
      id: '00005584003',
      administrator: '咸水沽储能管理员',
      duration: '0.25h',
      online: '2021-06-12 20:41',
      warning: 1,
      left: 53
    }
  ]
const BCUOutline = {
  B: 128.3,
  HV1: 129.6,
  HV2: 128.7,
  SOC: 63.2,
  insulation: 850,
  sight: 2,
  serious: 0
}
const BCUInfo ={
  status: 1,
  bmuCount: 5,
  stationName: "天津大学站"
}
module.exports = [
  {
    url: '/bcu/getBCUOutline',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: BCUOutline
      }
    }
  },
  {
    url: '/bcu/getBCUInfo',
    type: 'get',
    response: config => {
      return {
        code: 20000,
        data: BCUInfo
      }
    }
  },

    {
        url: '/bcu/bcustatData',
        type: 'get',
        response: config => {
          return {
            code: 20000,
            data: BcuStatData
          }
        }
      },

    {
        url: '/bcu/orderSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: OrderStatData
            }
        }
    },

    {
        url: '/bcu/warningSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: WarningStatData
            }
        }
    },

    {
        url: '/bcu/chargeSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: ChargeStatData
            }
        }
    },

    {
        url: '/bcu/slaves',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: slaveTable
            }
        }
    },

]