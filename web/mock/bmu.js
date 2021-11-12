const BmuStatData = {
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
      number: '123',
      type:'1号电池',
      status: 'online',
      left: 35
    },
    {
        number: '12',
        type:'5号电池',
        status: 'online',
        left: 56
    },
    {
        number: '23',
        type:'5号电池',
        status: 'online',
        left: 87
    }
  ]
const BMUOutline = {
        current: {
            max: 5,
            min: 3.21,
            avg: 3.21,
            collTime: "2021-08-21T16:17:11+08:00"
        },
        smoke: {
            max: 23,
            min: 23,
            avg: 23,
            collTime: "2021-08-21T16:17:11+08:00"
        },
        soc: {
            max: 82.1,
            min: 71.2,
            avg: 76.65,
            collTime: "2021-08-21T16:17:11+08:00"
        },
        temperature: {
            max: 32.1,
            min: 32.1,
            avg: 32.1,
            collTime: "2021-08-21T16:17:11+08:00"
        },
        voltage: {
            max: 2.8,
            min: 2.3,
            avg: 2.55,
            collTime: "2021-08-21T16:17:11+08:00"
        }
    }
const BMUInfo = {
  code: 200,
  status: 1,
  stationName: '天津大学北洋园校区',
  batteryNum: 5,
  batteryType: '镍钴'
}
module.exports = [
    {
        url: '/bmu/getBMUOutline',
        type: 'get',
        response: config => {
          return {
            code: 20000,
            data: BMUOutline
        }
      }
    },
    {
      url: '/bmu/getBMUInfo',
      type: 'get',
      response: config => {
        return {
          code: 20000,
          data: BMUInfo
        }
      }
    },
    {
        url: '/bmu/bmustatData',
        type: 'get',
        response: config => {
          return {
            code: 20000,
            data: BmuStatData
          }
        }
      },

    {
        url: '/bmu/orderSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: OrderStatData
            }
        }
    },

    {
        url: '/bmu/warningSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: WarningStatData
            }
        }
    },

    {
        url: '/bmu/chargeSataus',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: ChargeStatData
            }
        }
    },

    {
        url: '/bmu/slaves',
        type: 'get',
        response: config => {
            return {
                code: 20000,
                data: slaveTable
            }
        }
    },

]